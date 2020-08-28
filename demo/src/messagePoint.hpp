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

#ifndef MESSAGE_POINT_H
#define MESSAGE_POINT_H 1 

#include "messageBase.hpp"

class messagePoint : public messageBase
{
public:
    
    int reportType() {return MESSAGE_POINT_H;}
        
    messagePoint() {
		x_ = -1;
		y_ = -1;
	}
	
    messagePoint(const int x, const int y)
    : x_(x), y_(y)
    {
    }
    
    messagePoint(const messagePoint& p) {
		x_ = p.x_;
		y_ = p.y_;
	}

	messagePoint& operator+= (const messagePoint& p) {
		this->x_ += p.x_;
		this->y_ += p.y_;
		return *this;
	}
	
	friend bool operator== (const messagePoint& p1, const messagePoint& p2);
	friend std::basic_ostream<char>& operator<<(std::basic_ostream<char>& ss, const messagePoint& pm);
	friend class actorTruck;
	friend class actorJourneyTracker;
    
 private:
    int x_, y_;
};

bool operator== (const messagePoint& p1, const messagePoint& p2);

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& ss, const messagePoint& pm);

// saves creating a whole new class
#define messageVector messagePoint
#define MESSAGE_VECTOR_H MESSAGE_POINT_H 

#endif /* MESSAGE_POINT_H */

/* end of file */