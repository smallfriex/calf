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
 
#ifndef ACTOR_ROUTE_MANAGER_H
#define ACTOR_ROUTE_MANAGER_H

#include "actorBase.hpp"

#include "messagePoint.hpp"

class actorRouteManager : public actorBase
{
public:

    actorRouteManager(const ID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor) {
        
        // create any known child threads

        // register our actions
        registerAction(eventNewMinute::typeName, std::bind(&actorRouteManager::actionNewMinute, this));
    }
    
     // unused default constructors
    actorRouteManager(actorRouteManager const&) = delete;
    void operator=(actorRouteManager const&) = delete;

    
protected:
    
    void actionNewMinute(void);
    
private:
    
	// local variables
};

#endif // ACTOR_ROUTE_MANAGER_H

/* end of file */