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

#include "calf.hpp"
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <time.h>

unsigned long long timespecDiff(struct timespec& start, struct timespec& end) {
    long secs = difftime(end.tv_sec, start.tv_sec);
    if (secs == 0) {
        return end.tv_nsec - start.tv_nsec;
    }
    else {
        return (1000000000 * secs) + (end.tv_nsec - start.tv_nsec);
    }
}

std::string dateTimeNow() {
    const int RFC5322_TIME_LEN = 32;
    time_t t;
    struct tm *tm;

    std::string ret;
    ret.resize(RFC5322_TIME_LEN);

    time(&t);
    tm = localtime(&t);

    strftime(&ret[0], RFC5322_TIME_LEN, "%a, %d %b %Y %H:%M:%S %z", tm);

    return ret;
}

void show_stackframe() {
  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;

  trace_size = backtrace(trace, 16);
  messages = backtrace_symbols(trace, trace_size);
  printf("[bt] Execution path:\n");
  for (i=0; i<trace_size; ++i)
	printf("[bt] %s\n", messages[i]);
}

int runProcess(std::string path, bool wait) {
    pid_t PID = 0;
    if (wait == false)
        PID = fork();
    if (PID == 0)
    {
        return system(path.c_str());    
    }
    return (int) PID;        
}

basic_ostream<char>& operator<<(basic_ostream<char>& os, const ActorAddress& addr) {
    for (auto obj : addr) {
        os << obj << ".";
    }
    return os;
}

void Directory::add(const ActorID& name, const ActorAddress& address) {
    std::lock_guard<std::mutex> lock_{this->directoryLock_};
    directory_[name] = address;
}

ActorAddress& Directory::add(const ActorID& name, const ActorAddress& parentAddr, AddressElement uniqueNumeral) {
    // TODO: Could check for duplicate names and throw error
                
    // start with a copy of parent
    ActorAddress address = parentAddr;
    // go deeper
    address.push_back(uniqueNumeral);
    // add to the directory and return reference
    std::lock_guard<std::mutex> lock_{this->directoryLock_};
    return directory_[name] = address;
}

void Directory::remove(const ActorID& name) {
    std::lock_guard<std::mutex> lock_{this->directoryLock_};
    directory_.erase(name);
}

ActorAddress Directory::find(const ActorID& name) {
    std::lock_guard<std::mutex> lock_{this->directoryLock_};
    auto pair = directory_.find(name);
    if (pair == directory_.end()) {
        ActorAddress emptyAddress{};
        return emptyAddress;
    }
    else {
        return pair->second;
    }
} 

/* end of file */