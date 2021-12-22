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

#ifndef ACTOR_BASE_H
#define ACTOR_BASE_H

#include <string>
#include <cstring>
#include <queue>
#include <memory>
#include <map>
#include <functional>
#include <mutex>
#include <thread>
#include <assert.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <utility>
#include <condition_variable>
#include <chrono>
#include <mutex>
#if defined(__linux__)
#include <sys/prctl.h>
#endif

#include "calf.hpp"
#include "messageBase.hpp"
#include "messageEvents.hpp"
#include "messageEpoch.hpp"

#define WAIT_TIME 1000 // ms

#define TEMP_NAME "_tempname"

typedef std::function<void(void)> ActionFunction;

class ProcessingException : public std::exception {
public:
    const char* what() const throw()
    {
        return "actor processing exception\n";
    }
};

enum ActorStatus {CREATED=0, STARTED, STOPPING, STOPPED, ERRORED};

class actorBase;

struct actorPool {
   actorPool(std::function<actorBase*(void)> cc) 
   : childConstructor_(cc) {}
   std::vector<ActorID> childActorsLookup_;
   std::map<ActorAddress, actorBase*> childActors_;
   std::function<actorBase*(void)> childConstructor_;
};

class actorIni;

class actorBase
{
public:

    // error definitions
    static const int BH_SUCCESS     = 0;
    static const int BH_FAILURE     = 1;
    static const int BH_MEMFULL     = 2;
    static const int BH_NOTUNIQUE   = 3;
    static const int BH_LAST        = 4;
        
    // de/constructor
    actorBase(const ActorID& name, actorBase* parent_actor) : actorName_(name), parentActor_(parent_actor) {
        // register new actor with directory if not part of a pool (naming is deferred to thread-start)
        if (name.compare(TEMP_NAME) != 0) {
            ActorAddress parentAddress;
            int parentChildrenCount = 0;
            if (parent_actor != nullptr) {
                parentAddress  = parent_actor->getActorAddress();
                parentChildrenCount = parent_actor->childrenCreated();
            }
            actorAddress_ = directory_.add(name, parentAddress, parentChildrenCount);
        }
    }

    virtual ~actorBase() {
        // delete children actors
        // NB: since this is declared virtual, derived desctructors
        // should also be called
        for (auto child : childActors_) {
            delete child.second;
        }
        for (auto& pool : ourPools_) {
            for (auto& pool_child : pool.second.childActors_) {
                delete pool_child.second;
            }
        }
        directory_.remove(actorName_);
    }

    // remove unwanted constructors
    actorBase() = delete;

    // methods
    virtual int start();
    virtual int finish();
    
    ActorID getActorName() {return actorName_;}
    ActorAddress& getActorAddress() {return actorAddress_;}
    unsigned int childrenCreated() {return childrenCreated_;}
    void reportActivity(std::stringstream& output, int level = 0);
    
    // methods to add child thread / pools
    // returns ptr in case it needs to be started - i.e., not spawned in Constructor
    template <class T>
    actorBase* spawnChildActor(const ActorID& name) {
        if (name.empty()) {
            std::cerr << actorName_ << "cannot spawn actor with empty name" << std::endl;
            return 0;
        }
        // create a new child
        actorBase* newActor = new T(name, this);
        childrenCreated_++;
        childActors_.insert(std::make_pair(newActor->getActorAddress(), newActor));
        return newActor;
    }
    
    template <class T>
    int createPool(const ActorID& poolname) {    
        if (poolname.length() <= 5 || poolname.compare(0, 5, "POOL-") != 0) {
            std::cerr << "Pools must follow the naming convention POOL-poolname" << std::endl;
            return BH_FAILURE;
        }

        //check uniqueness
        auto check = ourPools_.find(poolname);
        if (check != ourPools_.end()) {
            std::cerr << "Cannot create repeat pool: "<< poolname << std::endl; 
            return BH_NOTUNIQUE;
        }
        // create new pool & set its child constructor
        ourPools_.insert(std::make_pair(poolname,actorPool([&](){return new T(TEMP_NAME,this);})));
        // register ourself as the pool controller
        directory_.add(poolname, actorAddress_);
     
        return BH_SUCCESS;
    }
    
    int postRequest(messageHolder& input, bool blocking = true); // NB: prefer sendRequest() for posting to own mailbox
    
protected:

    int useRequest(messageHolder& message);
    int sendRequest(messageHolder& input, bool blocking = false);
    int requeueRequest(messageHolder& message);
    
    void registerAction(Message_T (*message_type)(void), ActionFunction member_function){
        actions_[message_type] = member_function;
    }
    
    // helpers
    ActorID translateID(std::string pool_name, size_t index) {
        ActorID id = pool_name + "-ID-" + std::to_string(index);
        return id;
    }
    
    size_t translateIndex(ActorID actor_name) {
        size_t index = actor_name.rfind("-ID-");
        if (index == std::string::npos) {
            std::cerr << "cannot translate index on: " << actor_name << std::endl;
            return index;
        }
        std::string id = actor_name.substr(index+4, std::string::npos);
        return std::stoi(id);
    }
    
    ActorID actorName_ = "";
    ActorAddress actorAddress_;
    
    messageHolder inbuffer_;
    messageHolder outbuffer_;
    
    std::queue<messageHolder> input_;

    std::mutex messageLock_;
    
    std::map<Message_T (*)(void), ActionFunction> actions_;
    
    messageEpoch minutesSinceEpoch_;
    messageEpoch mseRolling_;
    
    std::map<ActorAddress, actorBase*> childActors_;
    std::map<ActorID, actorPool> ourPools_;
    
    actorBase* parentActor_;
    int userStatus_ = 0;
    
private:
    
    void execute();
    void tick();
    
    std::thread* thisThread_ = 0;
    bool running_ = true;
    std::mutex workLock_;
    std::condition_variable messagesArrived_;
    unsigned long long processedCount_ = 0;
    unsigned long long requeuedCount_ = 0;
    runningAverage<unsigned long long, 10> tickTimings_;

    ActorStatus status_ = CREATED;
    AddressElement childrenCreated_ = 0;
    static Directory directory_;
    
    void setThreadName(void) {

#ifdef _WIN32
        // TODO
#elif defined(__linux__)
        //#include <sys/prctl.h>
        prctl(PR_SET_NAME,actorName_.c_str(),0,0,0);
#elif defined(__APPLE__)
        pthread_setname_np(actorName_.c_str());
#else
        auto phandle = thisThread_->native_handle();
        if (phandle) {pthread_setname_np(phandle, actorName_.c_str()); }
#endif
    }

};

#endif /* ACTOR_BASE_H */

/* end of file */