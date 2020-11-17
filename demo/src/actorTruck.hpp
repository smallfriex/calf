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
        registerAction(eventNewMinute::typeName, std::bind(&actorTruck::actionNewMinute, this));
        registerAction(eventEachTick::typeName, std::bind(&actorTruck::actionNewTick, this));
        registerAction(messagePoint::typeName, std::bind(&actorTruck::actionReceivePointMessage, this));
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

/* end of file */