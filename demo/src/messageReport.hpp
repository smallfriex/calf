/*
 * CALF DEMO
 * File:    messagePoint.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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