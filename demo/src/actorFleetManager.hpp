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

#ifndef ACTOR_FLEET_MANAGER_H
#define ACTOR_FLEET_MANAGER_H

#include "actorBase.hpp"
#include "actorTruck.hpp"
#include "actorJourneyTracker.hpp"

#include "messagePoint.hpp"

class actorFleetManager : public actorBase
{
public:

    actorFleetManager(const ActorID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor) {
        
        // create any known child threads
        createPool<actorJourneyTracker>("POOL-JRN");

        // register our actions
        registerAction(eventNewMinute::typeName, std::bind(&actorFleetManager::actionHousekeeping, this));
        registerAction(messagePoint::typeName, std::bind(&actorFleetManager::actionReceivePointMessage, this));
    }
    
     // unused default constructors
    actorFleetManager(actorFleetManager const&) = delete;
    void operator=(actorFleetManager const&) = delete;

    
protected:
    
    void actionHousekeeping(void);
    void actionReceivePointMessage(void);
    
private:
    
	// local variables
};

#endif // ACTOR_FLEET_MANAGER_H

/* end of file */