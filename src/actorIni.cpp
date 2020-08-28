/*
 * CALF C++ Actor Light Framework
 * File:    actorIni.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#include "assert.h"
#include <typeinfo>
#include <sstream>
#include <iostream>

#include "calf.hpp"
#include "messageIni.hpp"
#include "actorIni.hpp"

void actorIni::actionReceiveIniMessage() {
      
    messageIni* imr = GetMessagePtr<messageIni>(inbuffer_);
    
        // first check if ini file changed
        if (iniFilename_.compare(imr->inifile_) != 0) {
            // now check input is sensible, otherwise we keep previous
            if ((!imr->inifile_.empty()) && fExists(imr->inifile_)) {
                iniFilename_ = imr->inifile_;
                // changed ini setting, send to cerr
                // ini handler doesn't rely on log handler, since it may not be created yet
                std::cerr << "ini file changed: " << iniFilename_ << " - loading" << std::endl;
                // trigger a re/load of ini file
                readIniFile();
            }
        }
    
    // always wipe message holders when finished with data
    WipeMessageHolder<messageIni>(inbuffer_);
}

void actorIni::actionHousekeeping() {
    // Regardless of ini file change, re-read ini file periodly
    // if we have one
    if (!iniFilename_.empty()) {
        readIniFile();
    }
}

void actorIni::readIniFile() {
    xml_parse_result status;
    if (iniSettings_ == 0 || iniSettings_ == &doc_b) {
        // load into doc a
        status = doc_a.load_file(iniFilename_.c_str());        
        if (status) {
            iniLock_.lock();
            iniSettings_ = &doc_a;
            iniLock_.unlock();
        }
        else {
            std::cerr << "bad parsing of ini file: " << iniFilename_ << std::endl;     
        }
    }
    else if (iniSettings_ == &doc_a) {
        status = doc_b.load_file(iniFilename_.c_str());
        if (status) {
            iniLock_.lock();
            iniSettings_ = &doc_b;
            iniLock_.unlock();
        }
        else {
            std::cerr << "bad parsing of ini file: " << iniFilename_ << std::endl;     
        }
    }
    else {
        assert(false);
    }
}

int actorIni::getSetting(const char* section, const char* key, const int& default_value)
{
    int returnInt = default_value;
    iniInstance_->iniLock_.lock();
    if (checkIni(section, key)) {
        // attempt to find key in xml data
        xml_node section_nd = iniInstance_->iniSettings_->child("Settings").child(section);
        if (! section_nd.empty()) {
            xml_node key_nd = section_nd.child(key);
            if (! key_nd.empty()) {  
                returnInt =  key_nd.text().as_int();
            }
        }
    }
    iniInstance_->iniLock_.unlock();
    return returnInt;
}

std::string actorIni::getSetting(const char* section, const char* key, const char* default_value)
{
    std::string returnStr = default_value;
    iniInstance_->iniLock_.lock();
    if (checkIni(section, key)) {
        // attempt to find key in xml data
        xml_node section_nd = iniInstance_->iniSettings_->child("Settings").child(section);
        if (! section_nd.empty()) {
            xml_node key_nd = section_nd.child(key);
            if (! key_nd.empty()) {
                returnStr =  key_nd.child_value();
            }
        }
    }
    iniInstance_->iniLock_.unlock();
    return returnStr;
}

bool actorIni::getSetting(const char* section, const char* key)
{
    std::string field = getSetting(section, key, "false");
    for (auto & c: field) c = toupper(c);
    if (field.compare("TRUE") ==0) {
        return true;
    }
    else {
        return false;
    }
}

bool actorIni::checkIni(const char* section, const char* key) {
     if (!iniInstance_) {
        std::cerr << "no ini instance!\n";
        std::cerr << "\trequested: " << section << "-" << key << std::endl;
        return false;
     }
     else if(!iniInstance_->iniSettings_) {
        std::cerr << "ini file not yet loaded / erroed!\n";
        std::cerr << "\trequested: " << section << "-" << key << std::endl; 
        return false;
     }
     return true;
}

actorIni* actorIni::iniInstance_ = 0;

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