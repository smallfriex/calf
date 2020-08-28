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

#ifndef MESSAGE_COMMAND_H
#define MESSAGE_COMMAND_H


// generic action types - shouldn't conflict with message numbers
#define EA_TICK 9001
#define NEW_MIN 9002

// basic command types
#define FLUSH_ACTOR 9003

class messageCommand : public messageBase
{
public:
    
    int reportType() {return command_;}
    messageCommand(int command)
    : command_(command) {}
    messageCommand() = delete;
    int command_;
};


#endif /* MESSAGE_COMMAND_H */

/* end of file */