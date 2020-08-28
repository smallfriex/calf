/*
 * CALF C++ Actor Light Framework
 * File:    executive.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#include <sstream>

#include "actorIni.hpp"
#include "actorRouteManager.hpp"
#include "actorFleetManager.hpp"
#include "actorReport.hpp"

#include "messageReport.hpp"

// boiler-plate

extern actorBase* top; 
messageHolder outbuffer_;

// entry-point methods for user code

void applicationPreStart() {
	// init stuff we need
    setlocale(LC_TIME,"UTC");

}
     
void applicationPreLaunch() {
    // launch needed services  here
    std::string prg = "aserver";
    //runProcess(prg, true);
}

void applicationAddInitialActors() {
    top->spawnChildActor<actorRouteManager>("RTE")->start();
    top->spawnChildActor<actorFleetManager>("FLT")->start();
    top->spawnChildActor<actorReport>("REP")->start();
}

bool applicationRunLoop() {
    
	static bool once_only = true;
	if (once_only) {
		once_only = false;
		
		// This where you would send any messages 
		// to kick off things
    
		messageReport* rmsg = new messageReport("Application has began");
		SetMessageHolder_(outbuffer_,"REP", "TOP", rmsg); 	// NB: from exective use SetMessageHolder_ not SetMessageHolder
		top->postRequest(outbuffer_); 						// NB: from exective, use postRequest not sendRequest
	}
    
    // This is where you would send any top-level
    // regular messages
    
    
    // little pause before returning
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // return true to continue running application
    return true;
}

void applicationCleanup() {
    // cleanup stuff here
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