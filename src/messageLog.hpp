/*
 * CALF C++ Actor Light Framework
 * File:    messageLog.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */
#ifndef MESSAGE_LOG_H
#define MESSAGE_LOG_H 8003

#include "messageBase.hpp"

enum LogLevel {LH_INFO=0, LH_WARN, LH_ERR};

class messageLog : public messageBase
{
public:
    
    int reportType() {return MESSAGE_LOG_H;}
        
    messageLog(const int level, const std::string& message)
    {
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