/*
 * CALF DEMO
 * File:    actorFleetManager.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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

    actorFleetManager(const ID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor) {
        
        // create any known child threads
        createPool<actorJourneyTracker>("JRN");

        // register our actions
        registerAction(NEW_MIN, std::bind(&actorFleetManager::actionHousekeeping, this));
        registerAction(MESSAGE_POINT_H, std::bind(&actorFleetManager::actionReceivePointMessage, this));
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