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

/* end of file */