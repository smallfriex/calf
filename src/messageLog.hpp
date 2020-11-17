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

#ifndef MESSAGE_LOG_H
#define MESSAGE_LOG_H

#include "messageBase.hpp"

class messageLog : public messageBase
{
public:
    static Message_T typeName() {return "messageLog";}
        
    enum LogLevel {LH_INFO=0, LH_WARN, LH_ERR};

    messageLog(const int level, const std::string& message) : messageBase(typeName) {
        level_ = level;
        message_.reserve(128);
        message_ = message;
    }
    int level_;
    std::string message_;
};

//helper macro
#define SendLogEntry(l,m) \
    messageLog* log_msg = new messageLog(l,m); \
    SetMessageHolder(outbuffer_,"LOG",log_msg); \
    sendRequest(outbuffer_);

#endif /* MESSAGE_LOG_H */

/* end of file */