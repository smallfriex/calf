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

/* end of file */
