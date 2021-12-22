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

#ifndef ACTOR_INI_H
#define ACTOR_INI_H

#include <memory>
#include "pugixml.hpp"

#include "actorBase.hpp"
#include "messageIni.hpp"

using namespace pugi;

// helpers
#define getIni(s,k,d) actorIni::getSetting(s,k,d)
#define getIniB(s,k) actorIni::getSetting(s,k)

class actorIni : public actorBase
{
public:

    // extra error definitions
    static const int IM_NOINIFILE = actorBase::BH_LAST;
    static const int IM_NOSETTING = IM_NOINIFILE+1;

    // de/constructor
    actorIni(const ActorID& name, actorBase* parent_actor)
    : actorBase(name,parent_actor) {
        // create any known child actors
        
        // register our actions
        registerAction(messageIni::typeName, std::bind(&actorIni::actionReceiveIniMessage, this));
        registerAction(eventNewMinute::typeName, std::bind(&actorIni::actionHousekeeping, this));
        
        // other initialisation
        if (iniInstance_ != 0) {
            std::cerr << "Shouldn't be more than one iniHandler created\n";
        }
        else {
            iniInstance_ = this;
        }            
    }
    virtual ~actorIni() {};

    // unused default constructors
    actorIni(actorIni const&) = delete;
    void operator=(actorIni const&) = delete;
    
    // user methods
    static std::string getSetting(const char* section, const char* key, const char* default_value);
    static int getSetting(const char* section, const char* key, const int& default_value);
    static bool getSetting(const char* section, const char* key); // NB default always false
    static actorIni* iniInstance_;
    
protected:
    
    void actionReceiveIniMessage();
    void actionHousekeeping();

    // filename - default
    std::string iniFilename_ = "";
    
    std::mutex iniLock_;

    static bool checkIni(const char* section, const char* key);
    void readIniFile();
    
private:
    // vars	
    xml_document doc_a;
    xml_document doc_b;
    xml_document* iniSettings_ = 0;
    
};

#endif // ACTOR_INI_H

/* end of file */
