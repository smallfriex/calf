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
    top->spawnChildActor<actorReport>("REP")->start();
    top->spawnChildActor<actorRouteManager>("RTE")->start();
    top->spawnChildActor<actorFleetManager>("FLT")->start();
}

bool applicationRunLoop() {
    
	static bool once_only = true;
	if (once_only) {
		once_only = false;
		
		// This where you would send any messages 
		// to kick off things
    
		messageReport* rmsg = new messageReport("Application has began");
		SetMessageHolder_(outbuffer_,"REP", "TOP", rmsg); 	// NB: from exective use SetMessageHolder_ not SetMessageHolder
		top->postRequest(outbuffer_, true);					// NB: from exective, use postRequest not sendRequest
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

/* end of file */