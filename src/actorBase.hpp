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

#define DEFAULT_SLEEP       20 // milliseconds
#define DELAYED_SLEEP       1000
#define MAX_MSGS_PER_TICK   10
// NB: (1000 / DEFAULT_SLEEP) * MAX_MSGS_PER_TICK = mesage quantity an actor can process per second

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
#if defined(__linux__)
#include <sys/prctl.h>
#endif

#include "calf.hpp"
#include "messageBase.hpp"
#include "messageEvents.hpp"

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
   std::vector<ID> childActorsLookup_;
   std::map<ID, actorBase*> childActors_;
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
    actorBase(const ID& name, actorBase* parent_actor) {
        actorName_ = name;
        parentActor_ = parent_actor;
    }

    virtual ~actorBase() {
        // delete children actors
        // NB: since this is declared virtual, derived desctructors
        // should also be called
        for (auto child : childActors_) {
            delete child.second;
        }
        for (auto& pool : childPools_) {
        for (auto& pool_child : pool.second.childActors_) {
            delete pool_child.second;
        }
    }
    };

    // remove unwanted constructors
    actorBase() = delete;

    // methods
    virtual int start();
    virtual int finish();
    
    ID getActorName() {return actorName_;}
    void reportActivity(std::stringstream& output, int level = 0);
    
    // methods to add child thread / pools
    // returns ptr in case it needs to be started - i.e., not spawned in Constructor
    template <class T>
    actorBase* spawnChildActor(const ID& name) {
        if (name.empty()) {
            std::cerr << "Cannot spawn empty actor" << std::endl;
            return 0;
        }
        auto check = childActors_.find(name);
        if (check != childActors_.end()) {
            std::cerr << "Cannot spawn repeat actor: "<< name << std::endl; 
            return 0;
        }
        // create a new child
        actorBase* newActor = new T(name, this);
        childActors_.insert(std::make_pair(name, newActor));
        return newActor;
    }
    
    template <class T>
    int createPool(const ID& name) {    
        if (name.empty()) {
            std::cerr << "Cannot create empty pool" << std::endl;
            return BH_FAILURE;
        }
        // create pool name for us
        std::string pool_name = actorName_ + "-POOL-" + name;
        //check uniqueness
        auto check = childPools_.find(pool_name);
        if (check != childPools_.end()) {
            std::cerr << "Cannot create repeat pool: "<< pool_name << std::endl; 
            return BH_NOTUNIQUE;
        }
        // create new pool & set its child constructor
        childPools_.insert(std::make_pair(pool_name,actorPool([&](){return new T("dummy",this);})));
     
        return BH_SUCCESS;
    }
    
    int postRequest(messageHolder& input);
    
protected:

    int useRequest(messageHolder& message);
    int sendRequest(messageHolder& input);
    int requeueRequest();
    void msleep(int sleep_time = DEFAULT_SLEEP);
    
    void registerAction(Message_T (*message_type)(void), ActionFunction member_function){
        actions_[message_type] = member_function;
    }
    
    // helpers
    ID translateID(std::string pool_name, size_t index) {
        ID id = pool_name + "-ID-" + std::to_string(index);
        return id;
    }
    
    size_t translateIndex(ID actor_name) {
        size_t index = actor_name.rfind("-ID-");
        if (index == std::string::npos) {
            std::cerr << "cannot translate index on: " << actor_name << std::endl;
            return index;
        }
        std::string id = actor_name.substr(index+4, std::string::npos);
        return std::stoi(id);
    }
    
    ID actorName_ = "";
    
    messageHolder inbuffer_;
    messageHolder outbuffer_;

    int sleepTime_ = DEFAULT_SLEEP;
    
    std::queue<messageHolder> input_;
    bool requeued_ = false;
    std::mutex messageLock_;
    
    std::map<Message_T (*)(void), ActionFunction> actions_;
    
    messageEpoch minutesSinceEpoch_;
    messageEpoch mseRolling_;
    
    std::map<ID, actorBase*> childActors_;
    std::map<ID, actorPool> childPools_;
    
    actorBase* parentActor_;
    int userStatus_ = 0;
    
private:
    
    void execute();
    void tick();
    
    std::thread* thisThread_ = 0;
    bool running_ = true;
    unsigned long long processedCount_ = 0;
    long latestHousekeepingDuration_ = 0;
    long accumulatedMessagingDuration_ =0;
    long latestMessagingDuration_ = 0;
    ActorStatus status_ = CREATED;

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