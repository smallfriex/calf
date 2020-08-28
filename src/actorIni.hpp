/*
 * CALF C++ Actor Light Framework
 * File:    actorIni.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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
    actorIni(const ID& name, actorBase* parent_actor)
    : actorBase(name,parent_actor)
    {
        // create any known child actors
        
        // register our actions
        registerAction(MESSAGE_INI_H, std::bind(&actorIni::actionReceiveIniMessage, this));
        registerAction(NEW_MIN, std::bind(&actorIni::actionHousekeeping, this));
        
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
