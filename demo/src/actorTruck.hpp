/*
 * CALF DEMO
 * File:    actorTruck.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#ifndef ACTOR_TRUCK_H
#define ACTOR_TRUCK_H

#include <string>
#include <map>
#include <iostream>

#include "actorBase.hpp"

#include "messagePoint.hpp"

enum TruckStatus {FREE = 0, RESTING, BUSY};

class actorTruck : public actorBase
{
public:
    actorTruck(const ID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor) {
        userStatus_=-1;    
        // create any known child threads
            
        // register our actions
        registerAction(NEW_MIN, std::bind(&actorTruck::actionNewMinute, this));
        registerAction(EA_TICK, std::bind(&actorTruck::actionNewTick, this));
        registerAction(MESSAGE_POINT_H, std::bind(&actorTruck::actionReceivePointMessage, this));
    }

    actorTruck(const actorTruck& other) = delete;

    actorTruck& operator=(const actorTruck& other) {
        userStatus_=-2;
        if (this == &other) return *this; // handle self assignment
        //assignment operator
        return *this;
    }
    
    void init(const messagePoint& location) {
        // initialise my position
        location_ = location;
    }

    double distanceFromPoint(const messagePoint& point) const;
    
    TruckStatus status() const {
        if (destination_ != nullptr) {
            return BUSY;
        } else {
            return callingStops_.empty() ? FREE : RESTING;
        }
    }
    
protected:
    
    void actionReceivePointMessage(void);
    void actionNewMinute(void);
    void actionNewTick(void);
    void move(void);

private:

    messagePoint location_;
    
    std::queue<messagePoint*> callingStops_;
    messagePoint* destination_ = nullptr;
    
    double interval_ = 100;
    long tickCount_ = 0;
};

#endif // ACTOR_TRUCK_H

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