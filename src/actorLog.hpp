/*
 * CALF C++ Actor Light Framework
 * https://github.com/smallfriex/calf
 * Copyright (2020) Craig Phillips
 * License: MIT License (see file)
 * File Description: 
 * 
 * File History:
 * Created 2020-08-28 Craig Phillips
 * Changed
 */

#ifndef ACTOR_LOG_H
#define ACTOR_LOG_H

#include <memory>

#include "messageLog.hpp"
#include "actorBase.hpp"

typedef messageLog::LogLevel LogLevel;

class actorLog : public actorBase {
public:

    // de/constructor
    actorLog(const ActorID& name, actorBase* parent_actor)
        : actorBase(name,parent_actor)
    {
        // create any known child actors
        
        // register our actions
        registerAction(messageLog::typeName, std::bind(&actorLog::actionReceiveLogMessage, this));
        registerAction(eventNewMinute::typeName, std::bind(&actorLog::actionHousekeeping, this));
                    
        // other initialisation
    }

    ~actorLog()
    {}

     // unused default constructors
    actorLog(actorLog const&) = delete;
    void operator=(actorLog const&) = delete;


protected:
    
    void actionReceiveLogMessage(void);
    void actionHousekeeping(void);
    
private:
    
    int logLevel_ = LogLevel::LH_INFO;

};

#endif /* ACTOR_LOG_H */

/* end of file */

