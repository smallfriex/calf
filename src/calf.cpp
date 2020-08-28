/*
 * CALF C++ Actor Light Framework
 * File:    calf.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
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

long timespecDiff(struct timespec& start, struct timespec& end) {
    long diff_secs = difftime(end.tv_sec, start.tv_sec);
    if (diff_secs == 0) {
        return end.tv_nsec - start.tv_nsec;
    }
    else {
        return (diff_secs * 1000000000) + (end.tv_nsec - start.tv_nsec);
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