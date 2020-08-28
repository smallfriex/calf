/*
 * CALF DEMO
 * File:    actorJourneyTracker.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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
        registerAction(MESSAGE_VECTOR_H, std::bind(&actorJourneyTracker::actionReceiveVectorMessage, this));
        registerAction(FLUSH_ACTOR, std::bind(&actorJourneyTracker::actionFlushJourney, this)); 
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