/*
 * CALF DEMO
 * File:    actorRouteManager.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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
	int x = std::rand()/((RAND_MAX + 1u)/40);
    	int y = std::rand()/((RAND_MAX + 1u)/40);
    	messagePoint* pmsg = new messagePoint(x,y);
    	SetMessageHolder(outbuffer_,"FLT", pmsg);
		sendRequest(outbuffer_);
    }
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