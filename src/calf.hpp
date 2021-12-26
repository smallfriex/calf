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

#ifndef CALF_H
#define CALF_H

#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <memory>
#include <mutex>
#include <map>
using namespace std;

//
// Misc Utility stuff
//

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


unsigned long long timespecDiff(struct timespec& start, struct timespec& stop);

void show_stackframe();

int runProcess(std::string path, bool wait = false);

inline bool fExists (const std::string& name) {
        struct stat buffer;
        return (stat (name.c_str(), &buffer) == 0);
}

std::string dateTimeNow();


//
// Address Holder & Directory classes etc
//

typedef unsigned int AddressElement;
// NB: vector could be inherited privately if we were worried about users of 
// ActorAddress having access to all of vector's public functions
class ActorAddress : public std::vector<AddressElement>
{
public:
    // constructors etc
   
    // friends
    friend basic_ostream<char>& operator<<(basic_ostream<char>& os, const ActorAddress& addr);

    // methods
    std::string toString();
    
    bool contains (const ActorAddress& test) {
        // contains means it starts with test and continues
        if (test.size() >= size()) {
            return false;
        }
        for (size_type i=0; i<test.size(); i++) {
            if (at(i) != test.at(i)) {
                return false;
            }
        }
        return true;
    }
};
basic_ostream<char>& operator<<(basic_ostream<char>& os, const ActorAddress& addr);

typedef std::string ActorID;
typedef std::string TaskID;

class Directory {
public:

    Directory() = default;
    ~Directory() = default;
    void add(const ActorID& name, const ActorAddress& address);
    ActorAddress& add(const ActorID& name, const ActorAddress& parentAddr, AddressElement uniqueNumeral);
    ActorAddress  find(const ActorID& name);
    void remove(const ActorID& name);

private:
    std::mutex directoryLock_;
    std::map <ActorID, ActorAddress> directory_;
};

#endif /* CALF_H */

/* end of file */