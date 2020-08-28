/*
 * CALF DEMO
 * File:    actorReport.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#include <algorithm>
#include <string>

#include "actorReport.hpp"

void actorReport::actionReceiveReportMessage(void) {
    
    // get message
    messageReport* rmr = GetMessagePtr<messageReport>(inbuffer_);
 
	std::cout << seperator_ << "\r\n"
		<< "Report Received: " << dateTimeNow() << "\r\n" 
		<< "From: " << inbuffer_.from_  << "\r\n"
		<< rmr->data_ << "\r\n"
		<< seperator_ << std::endl;
        
    // always wipe message holders when finished with data
    WipeMessageHolder<messageReport>(inbuffer_);
}

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