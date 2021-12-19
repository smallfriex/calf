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

#ifndef MESSAGE_EPOCH_H
#define MESSAGE_EPOCH_H

#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include "messageBase.hpp"

template <class objectT, unsigned int sz>
class runningAverage {

public:
    // constructors
    runningAverage() {
        array = new objectT[sz];
        firstPush_ = true;
        fill_ = 0;
        divisor_ = 1 / sz;
    }
    ~runningAverage() {
        delete [] array;
    }

   void push(const objectT& to_add) {
        if (firstPush_) {
            std::fill(array, array+sz, to_add);
            firstPush_ = false;
            fill_++;
            return;
        }
        array[fill_++] = to_add;
        if (fill_>=sz) {
            fill_ = 0;
        }
    }

    objectT mean() {
        objectT total = array[0];
        for (auto i = 1; i < sz; i++)
            total += array[i];
        return total * divisor_;
    }

    objectT max() {
        objectT total = array[0];
        for (auto i = 1; i < sz; i++)
            total = std::max(total,array[i]);
        return total;
    }

private:
    bool firstPush_ = true;
    unsigned int fill_ = 0;
    objectT* array = NULL;
    double divisor_ = 0;
};

class messageEpoch : public messageBase {
public:
    static Message_T typeName() {return "messageEpoch";}

    messageEpoch() : messageBase(typeName) {
        minutesSinceEpoch_=0;
    }
    
    messageEpoch(time_t tm) : messageBase(typeName) {
        minutesSinceEpoch_ = floor(tm / 60);
    }
    
    messageEpoch(const unsigned long long&  minutes) : messageBase(typeName) {
        minutesSinceEpoch_ = minutes;
    }
    
    messageEpoch(const messageEpoch& obj) : messageBase(typeName) {
        minutesSinceEpoch_ = obj.minutesSinceEpoch_;
    }
    
    messageEpoch(std::string asString) : messageBase(typeName) {
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

unsigned long long timespecDiff(struct timespec& start, struct timespec& stop);

void show_stackframe();

int runProcess(std::string path, bool wait = false);

inline bool fExists (const std::string& name) {
        struct stat buffer;
        return (stat (name.c_str(), &buffer) == 0);
}

std::string dateTimeNow();
std::string generateMessageId();

#endif /* MESSAGE_EPOCH_H */

/* end of file */