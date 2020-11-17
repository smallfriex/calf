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

typedef const char* Message_T;

class messageBase {
public:
    messageBase() = delete;
    messageBase(Message_T (*type_of)(void)) : type_(type_of) {}
    virtual ~messageBase() = default;
    bool isType(Message_T (*test)(void)) {return (test == type_);}
    Message_T (*type_)(void) = nullptr;
};

typedef std::string ID;

class messageHolder
{
    // Sender has responsibility to malloc payload
    // End User has responsibility to delete payload
public:
    messageHolder() {}
    
    messageHolder(const char* destination, const char* from, messageBase* data) {
        init(destination, from, data);
    }
    
    messageHolder (const messageHolder& obj) {
        dest_ = obj.dest_;
        from_ = obj.from_;
        payload_ = obj.payload_;
    }

    void init(const ID& destination, const ID& from, messageBase* data) {
        dest_ = destination;
        from_ = from;
        payload_ = data;
    }

    ~messageHolder() {
        // do not kill payload
    }

    bool isType(Message_T (*test)(void)) {return payload_->isType(test);}

    ID dest_="";
    ID from_="";
    messageBase* payload_=0;
};

// helper to get sent from
#define SetMessageHolder(a,b,c)    SetMessageHolder_(a,b,actorName_,c) 

template <class payloadT>
int SetMessageHolder_(messageHolder& mh, const ID& dest, const ID& from, const payloadT* payload) {
    mh.init(dest,from,(messageBase*) payload);
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

#endif /* MESSAGE_BASE_H */

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

/* end of file */
