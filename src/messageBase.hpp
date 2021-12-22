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

#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <cassert>
#include <string>
#include "calf.hpp"

typedef const char* Message_T;

#define MESSAGE_TYPE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)      

class messageBase {

public:

    messageBase() = delete;
    messageBase(Message_T (*type_of)(void)) : type_(type_of) {
#ifndef NDEBUG
        messageId_ = generateMessageId();
#endif
    }
    virtual ~messageBase() = default;
    bool isType(Message_T (*test)(void)) {return (test == type_);}
    Message_T (*type_)(void) = nullptr;
    std::string messageId_;

    static std::string generateMessageId()
    {
        const int MESSAGE_ID_LEN = 37;
        time_t t;
        struct tm *tm;

        std::string ret;
        ret.resize(15);

        time(&t);
        tm = gmtime(&t);

        strftime(const_cast<char *>(ret.c_str()),
                MESSAGE_ID_LEN,
                "%Y%m%d%H%M%S.",
                tm);

        ret.reserve(MESSAGE_ID_LEN);

        static const char alphanum[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

        while (ret.size() < MESSAGE_ID_LEN) {
            ret += alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        return ret;
    }

};

class messageHolder {
    // Sender has responsibility to malloc payload
    // End User has responsibility to delete payload
public:
    messageHolder() {}
    
    messageHolder(const char* destination, const char* from, messageBase* data, const char* task_id = "") {
        init(destination, from, data, task_id);
    }
    
    messageHolder (const messageHolder& obj) {
        dest_ = obj.dest_;
        from_ = obj.from_;
        taskId_   = obj.taskId_;
        destAddr_ = obj.destAddr_;
        payload_  = obj.payload_;
    }

    messageHolder& operator=(const messageHolder& obj) {
        dest_ = obj.dest_;
        from_ = obj.from_;
        taskId_   = obj.taskId_;
        destAddr_ = obj.destAddr_;
        payload_  = obj.payload_;
        return *this;
    }

    void init(const ActorID& destination, const ActorID& from, messageBase* data, const TaskID& task_id) {
        dest_ = destination;
        from_ = from;
        destAddr_.clear();
        payload_  = data;
        taskId_   = task_id;
        requeued_ = 0;
    }

    ~messageHolder() {
        // do not kill payload
    }

    bool isType(Message_T (*test)(void)) {return payload_->isType(test);}

    ActorID dest_  = "";
    ActorID from_  = "";
    TaskID taskId_ = "";
    ActorAddress destAddr_;
    messageBase* payload_ = 0;
    unsigned int requeued_ = 0;
};

// helper to get sent from
#define SetMessageHolder(a,b,c)     SetMessageHolder_(a,b,actorName_,c)
#define SetPoolMessageHolder(a,b,c,d) SetMessageHolder_(a,b,actorName_,c,d) 

template <class payloadT>
int SetMessageHolder_(messageHolder& mh, const ActorID& dest, const ActorID& from, const payloadT* payload, const TaskID& task = "") {
    mh.init(dest,from,(messageBase*) payload, task);
    return 0;
}

template <typename payloadT>
int WipeMessageHolder(messageHolder& mh) {
    assert(mh.payload_ !=0);
    payloadT* wipeObj = dynamic_cast<payloadT*>(mh.payload_);
    assert(wipeObj != 0);
    delete wipeObj;
    return 0;
}

template <class payloadT>
payloadT* GetMessagePtr(const messageHolder& mh) {
    payloadT* retObj = dynamic_cast<payloadT*>(mh.payload_);
    assert(retObj != 0);
    return retObj;
}

/* Note: C++ doesn't natively support reflection, so I've used addresses of static pointers
   to identifity message types. This is fast, adding little overhead for message construction
   or distinguishing and you can still print a message type by dereferencing, which is useful
   in debugging applications.

   The shortfall of this approach is the switch construct can't be supported, which would
   make the actor code that determines what to do with an incoming message a lot cleaner.
   For this to be improved, we would have to resort to use meta-programming of some sort,
   because even if you convert a pointer address into an integer somehow, the compiler will
   want something constanst at compile-time in the case statements.
*/

#endif /* MESSAGE_BASE_H */

/* end of file */
