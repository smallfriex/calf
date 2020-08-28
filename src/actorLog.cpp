/*
 * CALF C++ Actor Light Framework
 * File:    actorLog.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */


#include <ctime>
#include <iostream>

#include "messageLog.hpp"
#include "actorLog.hpp"
#include "actorIni.hpp"

void actorLog::actionReceiveLogMessage(void)
{
    // get message
    messageLog* lmr = GetMessagePtr<messageLog>(inbuffer_);

    if (lmr->level_ >= logLevel_)
    {
        static char buf[80];
        time_t t = time(0);   // get time now
        struct tm * now = localtime( & t );
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
        std::cerr << buf << " - " << lmr->message_ << std::endl;
    }

    // always wipe message holders when finished with data
    WipeMessageHolder<messageLog>(inbuffer_);
}

void actorLog::actionHousekeeping(void) {
    // regular stuff
    logLevel_ = getIni("Logging", "LOG_LEVEL", LH_INFO);
    
    // flush all messages to stream - may not be necessary, as using end
    std::cerr << std::flush;
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
