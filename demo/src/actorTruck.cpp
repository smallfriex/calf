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

#include "actorTruck.hpp"
#include "actorIni.hpp"

#include "messageReport.hpp"

enum Direction {HORIZ_STREET = 0, VERT_STREET, DIAG_STREET};

void actorTruck::actionReceivePointMessage(void) {

    // get message
    messagePoint* pmr = GetMessagePtr<messagePoint>(inbuffer_);
    
    // we'll add this to our calling queue
    callingStops_.push(new messagePoint(*pmr));
    // NB: this took a copy of the message, so we are free to delete the incoming ptr
    
    // always wipe message holders when finished with data
    WipeMessageHolder<messagePoint>(inbuffer_);
}
    
void actorTruck::actionNewMinute(void) {
    userStatus_=2;
    
    // Get Ini stuff periodically
    interval_ = getIni("Messaging","MOVE_INTERVAL", 1000); // basically, inverse of truck's speed   
}

void actorTruck::actionNewTick(void) {
    userStatus_=9;
    
    if (tickCount_++ > interval_) {
		tickCount_ = 0;
		 
		move();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void actorTruck::move(void) {
	if (destination_) {
		
		// move truck
		Direction direction = static_cast<Direction>(std::rand()/((RAND_MAX)/3));
		messageVector* vector = new messageVector(0,0);
		switch (direction) {
			case HORIZ_STREET :
			{
				if (location_.x_> destination_->x_) vector->x_--;
				else if (location_.x_ < destination_->x_) vector->x_++;
				break;
			}
			case VERT_STREET :
			{
				if (location_.y_> destination_->y_) vector->y_--;
				else if (location_.y_ < destination_->y_) vector->y_++;
				break;
			}
			case DIAG_STREET :
			{
				if (location_.x_> destination_->x_) vector->x_--;
				else if (location_.x_ < destination_->x_) vector->x_++;
				if (location_.y_> destination_->y_) vector->y_--;
				else if (location_.y_ < destination_->y_) vector->y_++;
				break;
			}
			default:
				assert(false);
				break;
		}
		location_ += *vector;

		// send this leg of journey to a tracker
		ID dest = "FLT-POOL-JRN-" + actorName_;
		SetMessageHolder(outbuffer_,dest, vector);
		sendRequest(outbuffer_);
		
		// are we there yet?
		if ( location_ == *destination_) {
			
			// reached destination!
			delete destination_; 
			destination_ = nullptr;
			
			// free up the tracker we used
			eventFlushActor* efa = new eventFlushActor();
            SetMessageHolder(outbuffer_,dest,efa);
            sendRequest(outbuffer_);

			// and report journey completion
			std::stringstream ss;
			ss << "Truck: " << actorName_ << " reached " << location_;
			messageReport* rmsg = new messageReport(ss.str());
			SetMessageHolder(outbuffer_,"REP", rmsg);
			sendRequest(outbuffer_);
		}
		
	} else { // NB: this is an else, since each stop will take at least 1 unit of time for resting
		
		int wait = std::rand()/((RAND_MAX)/2);
		if (wait == 0) {
			// rest is over, start heading for another destination
			if (!callingStops_.empty()) {
				destination_ = callingStops_.front();
				callingStops_.pop();
			}
		}
	}
}

double actorTruck::distanceFromPoint(const messagePoint& point) const {
	double x_diff = (double) std::abs(point.x_ - location_.x_);
	double y_diff = (double) std::abs(point.y_ - location_.y_);
	double distance = sqrt(( x_diff * x_diff) + (y_diff * y_diff));
	return distance;
}

/* end of file */