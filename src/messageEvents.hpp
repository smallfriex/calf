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

#ifndef MESSAGE_EVENTS_H
#define MESSAGE_EVENTS_H

class eventEachTick : public messageBase
{
public:
    static Message_T typeName() {return "eventEachTick";}
    
    eventEachTick() : messageBase(typeName) {}
};

class eventNewMinute : public messageBase
{
public:
    static Message_T typeName() {return "eventNewMinute";}
 
    eventNewMinute() : messageBase(typeName) {}
};

class eventFlushActor : public messageBase
{
public:
    static Message_T typeName() {return "eventFlushActor";}
   
    eventFlushActor() : messageBase(typeName) {}
};


#endif /* MESSAGE_EVENTS_H */

/* end of file */