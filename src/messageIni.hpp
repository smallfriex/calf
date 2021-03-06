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

#ifndef MESSAGE_INI_H
#define MESSAGE_INI_H

#include "messageBase.hpp"

class messageIni : public messageBase
{
public:
    static Message_T typeName() {return "messageIni";}
    
    messageIni(const std::string& filename) : messageBase(typeName) {
        inifile_.reserve(112);
        inifile_ = filename;
    }
    std::string inifile_;
};

#endif /* MESSAGE_INI_H */

/* end of file */