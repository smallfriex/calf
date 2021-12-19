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

std::string messageEpoch::to_string(bool local_time) const {
    // may be able to make more efficient by not converting to time_t
    time_t t = 60 * minutesSinceEpoch_;
    struct tm* this_tm = local_time ? localtime(&t): gmtime(&t);
    static char timeText[80];
    strftime(timeText, sizeof(timeText), "%FT%R", this_tm);
    std::string asString = timeText;
    return asString;
}

unsigned long long timespecDiff(struct timespec& start, struct timespec& end) {
    long secs = difftime(end.tv_sec, start.tv_sec);
    if (secs == 0) {
        return end.tv_nsec - start.tv_nsec;
    }
    else {
        return (1000000000 * secs) + (end.tv_nsec - start.tv_nsec);
    }
}

std::string dateTimeNow()
{
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

std::string generateMessageId()
{
    const int MESSAGE_ID_LEN = 37;
    time_t t;
    struct tm *tm;

    std::string ret;
    ret.resize(15);

    time(&t);
    tm = gmtime(&t);

    strftime(const_cast<char *>(ret.c_str()),
             MESSAGE_ID_LEN,
             "%Y%m%d%H%M%S.",
             tm);

    ret.reserve(MESSAGE_ID_LEN);

    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    while (ret.size() < MESSAGE_ID_LEN) {
        ret += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

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

int runProcess(std::string path, bool wait)
{
    pid_t PID = 0;
    if (wait == false)
        PID = fork();
    if (PID == 0)
    {
        return system(path.c_str());    
    }
    return (int) PID;        
}

/* end of file */