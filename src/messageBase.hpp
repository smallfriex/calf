/*
 * CALF C++ Actor Light Framework
 * File:    messageBase.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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

/* 
Copyright (c) 2020 Craig Phillips

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
