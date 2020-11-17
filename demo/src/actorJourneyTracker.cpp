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

#include <string>

#include "actorJourneyTracker.hpp"

#include "messageReport.hpp"

typedef messageLog::LogLevel LogLevel;

void actorJourneyTracker::actionReceiveVectorMessage(void) {
    
    // get message
    messageVector* vmr = GetMessagePtr<messageVector>(inbuffer_);

	// add this vector to the truck's journey
	if (vmr->x_>0 && vmr->y_>0) {
		journey_.append("\u2197");
	} else if (vmr->x_<0 && vmr->y_<0) {
		journey_.append("\u2199");
	} else if (vmr->x_>0 && vmr->y_<0) {
		journey_.append("\u2198");
	} else if (vmr->x_<0 && vmr->y_>0) {
		journey_.append("\u2196");
	} else if (vmr->x_==0 && vmr->y_<0) {
		journey_.append("\u2193");
	} else if (vmr->x_==0 && vmr->y_>0) {
		journey_.append("\u2191");
	} else if (vmr->x_>0 && vmr->y_==0) {
		journey_.append("\u2192");
	} else if (vmr->x_<0 && vmr->y_==0) {
		journey_.append("\u2190");
	}
	  
    // always wipe message holders when finished with data
    WipeMessageHolder<messageVector>(inbuffer_);
}

void actorJourneyTracker::actionFlushJourney(void) {
	// Send this journey to the log
	std::stringstream ss;
	ss << "Journey log: " << journey_;
	SendLogEntry(LogLevel::LH_INFO, ss.str());

	// Clear journey ready for reuse
	journey_.clear();

	// tell parent we've flushed, so we can be reused
    // in this case we can bounce the command back

    // strip ID to get POOL destination
    size_t pos = actorName_.find("-ID-");
    assert (pos != std::string::npos);
    ID dest = actorName_.substr(0, pos);
    SetMessageHolder(outbuffer_,dest,inbuffer_.payload_);
    sendRequest(outbuffer_);
    // don't wipe, as bouncing
}

/* end of file */