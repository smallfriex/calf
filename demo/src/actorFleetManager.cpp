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

#include "actorFleetManager.hpp"
#include "actorIni.hpp"
#include "actorLog.hpp"
#include "actorTruck.hpp"

#include "messageLog.hpp"
#include "messageReport.hpp"

typedef messageLog::LogLevel LogLevel;

void actorFleetManager::actionHousekeeping(void) {
    //
    // setup the fleet if not already constructed
    //
    if (childActors_.size() == 0) { 
		
		int fleet_size = getIni("Fleet","NUMBER_OF_TRUCKS", 5);
        
		actorTruck* truck;
		for(int i=1; i<=fleet_size; i++)
		{
			// create new truck actor & initialize          
			ActorID actor_name = "TRK-";
			actor_name.append(std::to_string(i));
			
			truck = static_cast<actorTruck*>(spawnChildActor<actorTruck>(actor_name)); 
			
			int x = std::rand()/((RAND_MAX + 1u)/40);
			int y = std::rand()/((RAND_MAX + 1u)/40);
			messagePoint point(x,y);
			
			truck->init(point);

			std::stringstream ss;
			ss << "Added truck: " << truck->getActorName() << " at location" << point;
			SendLogEntry(LogLevel::LH_INFO, ss.str());

			// *we* start the thread (because it's created during runtime)
			truck->start();
		}
    }
    
    // File a report on fleet
    int busy_trucks = 0, free_trucks = 0, resting_trucks = 0;
	for (auto& child : childActors_) {
		actorTruck* at = dynamic_cast<actorTruck*>(child.second);
		if (at) {
			TruckStatus ts = at->status();
			switch (ts) {
				case FREE : 
					free_trucks++;
					break;
				case BUSY :
					busy_trucks++;
					break;
				case RESTING :
					resting_trucks++;
					break;
				default:
					assert(false);
					break;
			}
		}
	}
	std::stringstream ss;
	ss << "Status Update\r\n"
	   << "Busy Trucks:    " << busy_trucks << "\r\n"
	   << "Free Trucks:    " << free_trucks << "\r\n"
	   << "Resting Trucks: " << resting_trucks;
	messageReport* rmsg = new messageReport(ss.str());
	SetMessageHolder(outbuffer_,"REP", rmsg);
	sendRequest(outbuffer_);
}


void actorFleetManager::actionReceivePointMessage(void) {

    // get message            
    messagePoint* pmr = GetMessagePtr<messagePoint>(inbuffer_);

    // figure out which truck is nearest. We could just check where we last sent the truck
    // we could do this via asynchronous messages and then we would have to hold a queue.
    // in this case, since they're our children, we'll ask the objects, synchronously
    double min_distance = 999.0f;
    actorTruck* truck_closest = nullptr;
    for (auto& child : childActors_) {
        actorTruck* at = dynamic_cast<actorTruck*>(child.second);
        if (at) {
            double distance = at->distanceFromPoint(*pmr);
            if (distance < min_distance) {
				min_distance = distance;
				truck_closest = at;
			}
        }
    }
    if (truck_closest) {
		// forward this message on to site actor
		SetMessageHolder(outbuffer_, truck_closest->getActorName(), pmr);
		sendRequest(outbuffer_);
		
		// and report on it
		std::stringstream ss;
		ss << "Sending truck: " << truck_closest->getActorName() << " to " << *pmr;
		messageReport* rmsg = new messageReport(ss.str());
		SetMessageHolder(outbuffer_,"REP", rmsg);
		sendRequest(outbuffer_);
	} else {
		SendLogEntry(LogLevel::LH_ERR, "Can't find a truck");
		
		// always wipe message holders when finished with data
		WipeMessageHolder<messagePoint>(inbuffer_);
		// apart from when you pass them on somewhere else!
	}
   
}

/* end of file */