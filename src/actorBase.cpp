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

class messageReport;

int actorBase::useRequest(messageHolder& message) {
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
    if (input.destAddr_.empty()) {
        input.destAddr_ = directory_.find(input.dest_);
    }
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
    for (auto& pool : ourPools_) {
        for (int t=0; t<level; t++) {output << "\t";}
        output << "- " << pool.first << ":\n";
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

void actorBase::execute() {
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
    for (auto& pool : ourPools_) {
        for (auto& child : pool.second.childActors_) {
        child.second->finish(); 
        }
    }
    if (status_ != ERRORED) {
        status_ = STOPPED;
    }
}

void actorBase::tick() {   
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

    unsigned int requeued = (unsigned long long) requeuedCount_;
    while (requeuedCount_ == (unsigned long long) requeued) {   
        unsigned int messages_onqueue = useRequest(inbuffer_);
        if (messages_onqueue > 0 ) {
            //
            // if there's no destination address, then look one up now
            //
            if (inbuffer_.destAddr_.empty()) {
                inbuffer_.destAddr_ = directory_.find(inbuffer_.dest_);
            }
            //
            // is it for us?
            //
            if (inbuffer_.destAddr_ == actorAddress_) {
                //
                // Check name to confirm we should consume this
                //
                if (inbuffer_.dest_.compare(actorName_) == 0) {
                    //
                    // consume this message
                    //
                    auto c = actions_.find(inbuffer_.payload_->type_);
                    if (c == actions_.end()) {
                        std::stringstream ss;
                        ss << "Actor: " << getActorName() << " cannot process"
                        << " Message Type: " << inbuffer_.payload_->type_()
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
                // Is it a pool command
                //
                else if (inbuffer_.dest_.compare(0, 4, "POOL") == 0) {
                    //
                    // find correct pool from our pools
                    //
                    auto pool_search = ourPools_.find(inbuffer_.dest_);
                    if (pool_search == ourPools_.end()) {
                        std::cerr << actorName_ << " doesn't have a pool for: " << inbuffer_.dest_ << std::endl;
                    }
                    else {
                        //
                        // found correct pool
                        //
                        actorPool& the_pool = const_cast<actorPool&>(pool_search->second);
                        //
                        // first check if it's a command for the pool controller - us
                        //
                        if (inbuffer_.taskId_.empty()) {                           
                            // consume this message
                            if (inbuffer_.isType(eventFlushActor::typeName)) {
                                
                                // flush has returned from POOL - free this actor for reuse
                                size_t index = translateIndex(inbuffer_.from_); 
                                assert (index < the_pool.childActorsLookup_.size()); 
                                the_pool.childActorsLookup_[index] = FREE;

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
                            std::vector<ActorID>::iterator it = std::find(the_pool.childActorsLookup_.begin(),
                                                                        the_pool.childActorsLookup_.end(),
                                                                        inbuffer_.taskId_);    
                            if (it == the_pool.childActorsLookup_.end()) {

                                // not found - try to assign existing free
                                it = std::find(the_pool.childActorsLookup_.begin(),
                                            the_pool.childActorsLookup_.end(),
                                            FREE);  
                                if (it != the_pool.childActorsLookup_.end()) { 
                                    //
                                    // lets reuse this one 
                                    //
                                    *it = inbuffer_.taskId_;
                                }
                                else {
                                    //
                                    // failing that, create a new thread and kick it off
                                    // add to lookup first
                                    the_pool.childActorsLookup_.push_back(inbuffer_.taskId_); 
                                    // then find it - keeps semantics simple
                                    it = std::find(the_pool.childActorsLookup_.begin(),
                                                the_pool.childActorsLookup_.end(), 
                                                inbuffer_.taskId_); 
                                    // then proceed as usual, but create actor & start thread first
                                    ActorID new_id = translateID(pool_search->first, std::distance(the_pool.childActorsLookup_.begin(),it));
                                    actorBase* new_actor = the_pool.childConstructor_();
                                    ActorAddress new_address = directory_.add(new_id, actorAddress_, childrenCreated_);
                                    childrenCreated_++;
                                    new_actor->actorName_   = new_id; // have to et name from TEMP_NAME
                                    new_actor->actorAddress_= new_address; // and now we know adress, set that
                                    the_pool.childActors_.insert(std::make_pair(new_address, new_actor)); // and add to our children
                                    new_actor->start();
                                }     
                            }
                            // set new correct destination address using lookup        
                            auto poolChildId = translateID(pool_search->first, std::distance(the_pool.childActorsLookup_.begin(),it));
                            inbuffer_.dest_ = poolChildId;
                            inbuffer_.destAddr_ = directory_.find(poolChildId);
                            // TODO: Could have some audit trail when messages are rerouted like this

                            // and we need to post it to the correct child - each pool has its own childActors_ map of pointers
                            auto poolkid = the_pool.childActors_.find(inbuffer_.destAddr_);
                            if (poolkid != the_pool.childActors_.end()) {
                                //
                                // put it on correct child queue
                                //
                                poolkid->second->postRequest(inbuffer_, true);
                                // not final destination, so don't wipe message
                            } else {
                                // raise an error - destination could be wrong or destination actor could have been deleted
                                std::cerr << "pool child " <<  inbuffer_.dest_ << " (" << inbuffer_.destAddr_ << ")"
                                          << "not found for message from " << inbuffer_.from_ << " - presumed actor finished" << std::endl;
                                // wipe as best we can - NB: may not call derived destructors
                                delete inbuffer_.payload_; 
                            }
                        }
                    }       
                } else {
                    std::cerr << "message to" << inbuffer_.dest_ << " actor (" << inbuffer_.destAddr_ << ") from "
                              << inbuffer_.from_ << " seems to be for us, but we can't find a way to consume it" << std::endl;
                    std::cerr << "message to : " << inbuffer_.dest_ << std::endl;
                    // wipe as best we can - NB: may not call derived destructors
                    delete inbuffer_.payload_; 
                }
            }  
            //
            // Could it be for one of our progeny?
            //
            else if (inbuffer_.destAddr_.contains(actorAddress_)) {
                // it is, so pass it to correct child
                ActorAddress childAddr = actorAddress_;
                childAddr.push_back(inbuffer_.destAddr_[actorAddress_.size()]);
            
                // search for correct child
                auto it = childActors_.find(childAddr);
                if (it != childActors_.end()) {
                    //
                    // put it on correct child queue
                    //
                    if (it->second->postRequest(inbuffer_) != BH_SUCCESS)
                        requeueRequest(inbuffer_);
                    // not final destination, so don't wipe message
                } else {
                    // raise an error - destination could be wrong or destination actor could have been deleted
                    std::cerr << inbuffer_.dest_ << " actor (" << inbuffer_.destAddr_ << ") not found for message from "
                              << inbuffer_.from_ << " - presumed actor finished" << std::endl;
                    // wipe as best we can - NB: may not call derived destructors
                    delete inbuffer_.payload_; 
                }
            }
            //
            // If destination isn't a descendent of ours, it's a descendent of a parent of ours
            //
            else {
                if (parentActor_ == 0) {
                    // raise an error - destination could be wrong or destination actor could have been deleted
                    std::cerr << inbuffer_.dest_ << " actor (" << inbuffer_.destAddr_ << ") not found for message from "
                            << inbuffer_.from_ << " - end of the line" << std::endl;
                    // wipe as best we can - NB: may not call derived destructors
                    delete inbuffer_.payload_; 
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
        else {
            break; // leave the while loop as no messages to process
        }
    }
            
    // figure out load
    clock_gettime(CLOCK_MONOTONIC, &frame_end);
    tickTimings_.push(timespecDiff(frame_start, frame_end));
}

Directory actorBase::directory_;

/* end of file */