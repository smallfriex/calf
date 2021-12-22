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

#ifndef ACTOR_REPORT_H
#define ACTOR_REPORT_H

#include <string>
#include <iostream>

#include "actorBase.hpp"

#include "messageReport.hpp"

class actorReport : public actorBase
{
public:
    actorReport(const ActorID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor) {
        userStatus_=1;    
        // create any known child threads
                 
        // register our actions
        registerAction(messageReport::typeName, std::bind(&actorReport::actionReceiveReportMessage, this));   
    }

    actorReport(const actorReport& other) = delete;

    actorReport& operator=(const actorReport& other) {
        userStatus_=2;
        if (this == &other) return *this; // handle self assignment
        //assignment operator
        return *this;
    }  
     
protected:
    
    void actionReceiveReportMessage(void);    
    
private:
    
    const char* seperator_ = "------------------------------------------------";

};


#endif /* ACTOR_REPORT_H */

/* end of file */