/*
 * CALF C++ Actor Light Framework
 * https://github.com/smallfriex/calf
 * Copyright (2020) Craig Phillips
 * License: MIT License (see file)
 * File Description: 
 * 
 * File History:
 * Created 2020-08-28 Craig Phillips
 * Changed
 */

#include "actorBase.hpp"
#include "actorLog.hpp"
#include "messageLog.hpp"

#include <exception>
#include <chrono>
#include <assert.h>
#include <algorithm>
#include <iomanip>

#define FREE std::string("free")

int actorBase::useRequest(messageHolder& message)
{
    messageLock_.lock();
    size_t size = input_.size();
    if (size > 0) {
        message = input_.front();
        input_.pop();
    }
    messageLock_.unlock();
    return size;
}

int actorBase::requeueRequest(messageHolder& message) {   
    requeuedCount_++;
    message.requeued_++;
    messageLock_.lock();
    input_.push(message);
    messageLock_.unlock();
    return BH_SUCCESS;
}

int actorBase::sendRequest(messageHolder& input, bool blocking) {
    bool lock = true;
    if (blocking)
        messageLock_.lock(); 
    else
        lock = messageLock_.try_lock();
    if (lock) {
        input_.push(input);
        messageLock_.unlock();
        return BH_SUCCESS;
    } 
    return BH_FAILURE;
}

int actorBase::postRequest(messageHolder& input, bool blocking) {
    auto retVal = sendRequest(input, blocking);
    if (retVal == BH_SUCCESS) {
        messagesArrived_.notify_one(); 
    }
    return retVal;
}

int actorBase::start() {
    thisThread_ = new std::thread(std::bind(&actorBase::execute,this));
    setThreadName();
    return BH_SUCCESS;
}

int actorBase::finish() {
    running_ = false;
    thisThread_->join();
    return BH_SUCCESS;
}

// TODO: could make an xml version
void actorBase::reportActivity(std::stringstream& output, int level) {
    for (int t=0; t<level; t++) {output << "\t";}
    std::string messageSummary = std::to_string(input_.size()) + "/" + std::to_string(requeuedCount_) + "/" + std::to_string(processedCount_); 
    double percentage = (0.0001f * (double) tickTimings_.mean()) / WAIT_TIME; 
    output << actorName_ << " | status: " << status_ << " " << userStatus_
           << " | messages: " << std::setw(6) << std::setfill(' ') << messageSummary      
           << " | load: "     << std::setw(7) << std::setfill('0') << std::fixed << std::setprecision(3) << percentage << "%\n";
    for (auto& pool : childPools_) {
        for (int t=0; t<level; t++) {output << "\t";}
        output << "- POOL: " << pool.first << "\n";
        for (auto& pool_child : pool.second.childActors_) {
            pool_child.second->reportActivity(output, level + 1);
        }
    }
    if (childActors_.size() > 0) {
        for (int t=0; t<level; t++) {output << "\t";}
        output << "- CHILDREN: " << "\n";
        for (auto& child : childActors_) {
            child.second->reportActivity(output, level + 1);
        }
    }
}

void actorBase::execute()
{
    status_ = STARTED;
    // start child threads we know about
    for (auto& child : childActors_) {
        child.second->start();
    }   
    // main body
    try {
        while (running_) {
            std::unique_lock<std::mutex> ulock(workLock_);
            messagesArrived_.wait_for(ulock, std::chrono::milliseconds(WAIT_TIME));
            tick();
        }
        status_ = STOPPING;
        // consume remaining messages - not sure if this is necessary
        while (!input_.empty()) {
            tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Actor: " << actorName_ << " caught exception: " 
                  << e.what() << " - exiting thread!" << std::endl;
        show_stackframe();
        status_ = ERRORED;
    }
    if (status_ != ERRORED) {
        status_ = STOPPING;
    }
    // join child threads to our thread
    for (auto& child : childActors_) {
        child.second->finish(); 
    }
    for (auto& pool : childPools_) {
        for (auto& child : pool.second.childActors_) {
        child.second->finish(); 
        }
    }
    if (status_ != ERRORED) {
        status_ = STOPPED;
    }
}

void actorBase::tick() 
{   
    struct timespec frame_start, frame_end;
    clock_gettime(CLOCK_MONOTONIC, &frame_start);

    // check for time-based events first
    mseRolling_.update(); // get time now
    if (mseRolling_ > minutesSinceEpoch_) {
        minutesSinceEpoch_ = mseRolling_;
        auto a = actions_.find(eventNewMinute::typeName);
        if (a != actions_.end())  {
            a->second();
        }
    }
    
    auto b = actions_.find(eventEachTick::typeName);
    if (b != actions_.end())  {
        b->second();
    }
    
    // now check for new messages on our queue
    // get message
    std::string pool_name = actorName_ + "-POOL-";

    unsigned int requeued = (unsigned long long) requeuedCount_;
    while (requeuedCount_ == (unsigned long long) requeued) {   
        unsigned int messages_onqueue = useRequest(inbuffer_);
        if (messages_onqueue > 0 ) {
            //
            // is it for us?
            //
            if (inbuffer_.dest_.compare(actorName_) == 0) {
                //
                // consume this message
                //
                auto c = actions_.find(inbuffer_.payload_->type_);
                if (c == actions_.end()) {
                    std::stringstream ss;
                    ss << "Actor: " << getActorName() << " cannot process"
                    << " Message: " << inbuffer_.payload_->type_()
                    << " From: " << inbuffer_.from_;
                    std::cerr << ss.str() << std::endl;
                }
                else {
                    processedCount_++;
                    // call our action function
                    c->second();
                }
            }
            //
            // Is it for one of our pool children?
            //
            else if (inbuffer_.dest_.compare(0, pool_name.length(), pool_name) == 0) {
                // find correct pool
                for (auto& pool: childPools_) {
                    if (inbuffer_.dest_.compare(0, pool.first.length(), pool.first) == 0) {
                        //
                        // found correct pool
                        //
                        actorPool& apool = const_cast<actorPool&>(pool.second);
                        //
                        // first check if it's a command for the pool controller - us
                        //
                        if (inbuffer_.dest_.compare(pool.first) == 0) {                           
                            // consume this message
                            if (inbuffer_.isType(eventFlushActor::typeName)) {
                                
                                // flush has returned from POOL - free this actor for reuse
                                size_t index = translateIndex(inbuffer_.from_); 
                                assert (index < apool.childActorsLookup_.size()); 
                                apool.childActorsLookup_[index] = FREE;

                                // we've consumed the message, so wipe
                                WipeMessageHolder<eventFlushActor>(inbuffer_);

                            } else {
                                std::cerr << actorName_ << " didn't understand " << inbuffer_.payload_->type_() << " pool command" << std::endl;
                                // NB: Not good, we have a potential memory leak here
                            }
                        }
                        else {
                            //
                            // Must be for one of this pool's children
                            //
                           
                            // check there is a thread to receive the message, and if not create one                            
                            std::vector<ID>::iterator it = std::find(apool.childActorsLookup_.begin(),
                                                                     apool.childActorsLookup_.end(),
                                                                       inbuffer_.dest_);    
                            if (it == apool.childActorsLookup_.end()) {

                                // not found - try to assign existing free
                                it = std::find(apool.childActorsLookup_.begin(),
                                               apool.childActorsLookup_.end(),
                                               FREE);  
                                if (it != apool.childActorsLookup_.end()) { 
                                    //
                                    // lets reuse this one 
                                    //
                                    *it = inbuffer_.dest_;

                                }
                                else {

                                    // failing that, create a new thread and kick it off
                                    // add to lookup first
                                    apool.childActorsLookup_.push_back(inbuffer_.dest_); 
                                    // then find it - keeps semantics simple
                                    it = std::find(apool.childActorsLookup_.begin(),
                                                   apool.childActorsLookup_.end(), 
                                                   inbuffer_.dest_); 
                                    // then proceed as usual, but create thread first
                                    ID new_id = translateID(pool.first, std::distance(apool.childActorsLookup_.begin(),it));
                                    actorBase* newActor = apool.childConstructor_();
                                    newActor->actorName_ = new_id;
                                    apool.childActors_.insert(std::make_pair(new_id, newActor));
                                    newActor->start();
                                }     
                            }

                            // translate the dest from the lookup value               
                            inbuffer_.dest_ = translateID(pool.first, std::distance(apool.childActorsLookup_.begin(),it));

                            //
                            // post message to pool child's queue
                            //
                            auto pool_kid = apool.childActors_.find(inbuffer_.dest_);
                            assert (pool_kid != apool.childActors_.end());
                            if (pool_kid->second->postRequest(inbuffer_) != BH_SUCCESS)
                                requeueRequest(inbuffer_);
                        }
          
                        return;
                    }
                }
                std::cerr << "we don't have a pool for: " << inbuffer_.dest_ << std::endl;                
            }
            //
            // Could it be for one of our direct children?
            //
            else {
                // search for child
                auto it = childActors_.find(inbuffer_.dest_);
                if (it != childActors_.end()) {
                    //
                    // put it on correct child queue
                    //
                    if (it->second->postRequest(inbuffer_) != BH_SUCCESS)
                        requeueRequest(inbuffer_);
                    // not final destination, so don't wipe message
                }
                else {
                    //
                    // if we can't find mailbox, pass to parent
                    //
                    if (parentActor_ == 0) {
                        std::cerr << inbuffer_.dest_ << " actor - not found for message from " << inbuffer_.from_ << std::endl;
                        // wipe as best we can - NB: may not call derived destructors
                        delete inbuffer_.payload_; 
                        // TODO: Could requeue these 
                    }
                    else {
                        //
                        // Pass to parent
                        //
                        if (parentActor_->postRequest(inbuffer_) != BH_SUCCESS)
                            requeueRequest(inbuffer_);
                    }
                }
            }
        }
        else {
            break; // leave the while loop as no messages to process
        }
    }
            
    // figure out load
    clock_gettime(CLOCK_MONOTONIC, &frame_end);
    tickTimings_.push(timespecDiff(frame_start, frame_end));
}

/* end of file */