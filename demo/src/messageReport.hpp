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
#define MESSAGE_REPORT_H 2 

#include "messageBase.hpp"

#include <cstring>

class messageReport : public messageBase
{
public:
    
    int reportType() {return MESSAGE_REPORT_H;}
    
    messageReport(const std::string& message)
    {
        data_.reserve(512);
        data_ = message;
    }
    std::string data_;
};

#endif /* MESSAGE_REPORT_H */

/* end of file */