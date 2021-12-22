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
 
#include <iostream>
#include <sstream>

#include "actorRouteManager.hpp"
#include "actorIni.hpp"

#include "messagePoint.hpp"

void actorRouteManager::actionNewMinute(void) {
    //
    // Each new minute, send a bunch of requests to our fleet for locations to visit 
    //
    int requests = getIni("Routing","NUMBER_OF_REQUESTS_PER_MIN", 5);
    for (int i=0; i<requests; i++) {
	    int x = std::rand()/((RAND_MAX + 1u)/80);
    	int y = std::rand()/((RAND_MAX + 1u)/80);
    	messagePoint* pmsg = new messagePoint(x,y);
    	SetMessageHolder(outbuffer_,"FLT", pmsg);
		sendRequest(outbuffer_);
    }
}

/* end of file */