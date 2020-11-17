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

#ifndef ACTOR_JOURNEY_TRACKER_H
#define ACTOR_JOURNEY_TRACKER_H

#include <string>
#include <iostream>

#include "actorBase.hpp"
#include "actorLog.hpp"

#include "messagePoint.hpp"

class actorJourneyTracker : public actorBase
{
public:
    actorJourneyTracker(const ID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor) {
        userStatus_=-1;    
        // create any known child threads
                 
        // register our actions
        registerAction(messageVector::typeName, std::bind(&actorJourneyTracker::actionReceiveVectorMessage, this));
        registerAction(eventFlushActor::typeName, std::bind(&actorJourneyTracker::actionFlushJourney, this)); 
    }

    actorJourneyTracker(const actorJourneyTracker& other) = delete;

    actorJourneyTracker& operator=(const actorJourneyTracker& other) {
        userStatus_=-2;
        if (this == &other) return *this; // handle self assignment
        //assignment operator
        return *this;
    }  
     
protected:
    
    void actionReceiveVectorMessage(void); 
    void actionFlushJourney(void);   
    
private:

    std::string journey_;

};


#endif /* ACTOR_JOURNEY_TRACKER_H */

/* end of file */