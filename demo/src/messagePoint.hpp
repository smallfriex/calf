/*
 * CALF DEMO
 * File:    messagePoint.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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