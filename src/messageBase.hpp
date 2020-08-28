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
#define MESSAGE_BASE_H 8000

#include <cassert>
#include <string>

class messageBase {
public:
    virtual ~messageBase() = default;
    virtual int reportType() = 0;
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

/* end of file */
