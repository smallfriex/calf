/*
 * CALF DEMO
 * https://github.com/smallfriex/calf
 * Copyright (2020) Craig Phillips
 * License: MIT License (see file)
 * File Description: 
 * 
 * File History:
 * Created 2020-08-28 Craig Phillips
 * Changed
 */

#ifndef MESSAGE_REPORT_H
#define MESSAGE_REPORT_H

#include "messageBase.hpp"

#include <cstring>

class messageReport : public messageBase
{
public:
    static Message_T typeName() {return MESSAGE_TYPE;}
    
    messageReport(const std::string& message) : messageBase(typeName) {
        data_.reserve(512);
        data_ = message;
    }
    std::string data_;
};

#endif /* MESSAGE_REPORT_H */

/* end of file */