/*
 * CALF C++ Actor Light Framework
 * File:    calf.hpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#ifndef MESSAGE_EPOCH_H
#define MESSAGE_EPOCH_H 8001

#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include "messageBase.hpp"

class messageEpoch : public messageBase {
public:
    
    int reportType() {return MESSAGE_EPOCH_H;}
    
    messageEpoch() {
        minutesSinceEpoch_=0;
    }
    
    messageEpoch(time_t tm) {
        minutesSinceEpoch_ = floor(tm / 60);
    }
    
    messageEpoch(const unsigned long long&  minutes)
    {
        minutesSinceEpoch_ = minutes;
    }
    
    messageEpoch(const messageEpoch& obj) {
        minutesSinceEpoch_ = obj.minutesSinceEpoch_;
    }
    
    messageEpoch(std::string asString) {
        struct tm when;
        memset(&when, 0, sizeof(struct tm));
        strptime(asString.c_str(),"%F%TR", &when);
        minutesSinceEpoch_ = floor(timegm(&when) / 60);
    }
    
    messageEpoch& operator=(const messageEpoch& obj) {
        // test for self-assignment
        if (&obj != this) {
             minutesSinceEpoch_ = obj.minutesSinceEpoch_;
        }
        return *this;
    }
    
    messageEpoch operator-=(long long rhs) {
        this->minutesSinceEpoch_ -= rhs;
        return *this;
    }
    
    messageEpoch operator+=(long long rhs) {
        this->minutesSinceEpoch_ += rhs;
        return *this;
    }
    
    messageEpoch operator+(const long long& rhs) {
        messageEpoch temp(*this);
        temp += rhs;
        return temp;
    }
    
    messageEpoch operator-(const long long& rhs) {
        messageEpoch temp(*this);
        temp -= rhs;
        return temp;
    }
    
    bool operator>(const messageEpoch& rhs) {
        return minutesSinceEpoch_ > rhs.minutesSinceEpoch_;
    }
    
    bool operator<(const messageEpoch& rhs) {
        return minutesSinceEpoch_ < rhs.minutesSinceEpoch_;
    }
    
    friend std::basic_ostream<char>& operator<<(std::basic_ostream<char>& ss, 
												const messageEpoch& epoch);
    
    void update()
    {
        time_t t = time(0);   // get time now
        minutesSinceEpoch_ = floor(t / 60.0);
    }
    
    std::string to_string(bool local_time = false) const;
     
private:
    
    unsigned long long minutesSinceEpoch_=0; 
};

long timespecDiff(struct timespec& start, struct timespec& stop);

void show_stackframe();

int runProcess(std::string path, bool wait = false);

inline bool fExists (const std::string& name) {
        struct stat buffer;
        return (stat (name.c_str(), &buffer) == 0);
}

std::string dateTimeNow();
std::string generateMessageId();

#endif /* MESSAGE_EPOCH_H */

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