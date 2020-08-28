/*
 * CALF DEMO
 * File:    actorJourneyTracker.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#include <string>

#include "actorJourneyTracker.hpp"

#include "messageReport.hpp"

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
	SendLogEntry(LH_INFO, ss.str());

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