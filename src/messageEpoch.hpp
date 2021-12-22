/*
 * CALF C++ Actor Light Framework
 * https://github.com/smallfriex/calf
 * Copyright (2021) Craig Phillips
 * License: MIT License (see file)
 * File Description: 
 * 
 * File History:
 * Created 2021-12-22 Craig Phillips
 * Changed
 */

#ifndef MESSAGE_EPOCH_H
#define MESSAGE_EPOCH_H

class messageEpoch : public messageBase {
public:
    static Message_T typeName() {return MESSAGE_TYPE;}

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
    
    void update()
    {
        time_t t = time(0);   // get time now
        minutesSinceEpoch_ = floor(t / 60.0);
    }
    
    std::string to_string(bool local_time = false) const {
    // may be able to make more efficient by not converting to time_t
    time_t t = 60 * minutesSinceEpoch_;
    struct tm* this_tm = local_time ? localtime(&t): gmtime(&t);
    static char timeText[80];
    strftime(timeText, sizeof(timeText), "%FT%R", this_tm);
    std::string asString = timeText;
    return asString;
}
     
private:
    
    unsigned long long minutesSinceEpoch_=0; 
};

#endif /* MESSAGE_EPOCH_H */

/* end of file */