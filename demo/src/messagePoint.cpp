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

#include "messagePoint.hpp"

bool operator== (const messagePoint& p1, const messagePoint& p2) {
	return (p1.x_ == p2.x_ && p1.y_ == p2.y_);
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& ss, const messagePoint& pm)  
{  
	// format nicely and send to stream
	ss << "Point [X=" << pm.x_ << ", Y=" << pm.y_ << "]";
    return ss;  
}

/* end of file */