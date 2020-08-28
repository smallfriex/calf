/*
 * CALF C++ Actor Light Framework
 * File:    main.cpp
 * Description:
 * License: MIT License (see below)
 * Created: 2020-08-28 Craig Phillips
 * Changed:
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <signal.h>
#include <locale.h>

#include "calf.hpp"
#include "actorBase.hpp"
#include "actorIni.hpp"
#include "actorLog.hpp"

// constants
const char* CALF_VERSION = "1.0";
#define NO_PARENT   0

// user prototypes
void applicationPreStart();   
void applicationPreLaunch();
void applicationAddInitialActors();
bool applicationRunLoop();
void applicationCleanup();

// begin body
static bool running_ = true;
actorBase* top = 0;

std::string pwd = getcwd(NULL,0);

void sigHandler(int s)
{
    std::cout << "caught signal: " << s << std::endl;
    switch (s) {
        case SIGINT:
        case SIGTERM:
        {
			std::cout << "Attempting application shutdown..." << std::endl;
            running_ = false;
            break;
        }
        case SIGPIPE:
        {
            std::cout << "Restarting services we rely on..." << std::endl;
            applicationPreLaunch();
			break;
		}
        case SIGTSTP:
        {
            // Application State Report - ripple down through actors
            std::stringstream ss;
            ss << "ASR | thread-status/userstatus | messages-queued/messages-processed | "
               << "us-on-messages/us-on-housekeeping per min\n";
            top->reportActivity(ss,0);
            std::cout << "\n" << ss.str() << std::endl;
			break;
        }
        default:
            show_stackframe();
            break;
    }
}

int main(int argc, char** argv)
{
    std::cout << "Application started" << std::endl;

    // user initialisation
    applicationPreStart();
    
    // catch signals
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);
    sigaction(SIGPIPE, &sigIntHandler, NULL);
    sigaction(SIGTSTP, &sigIntHandler, NULL);
    sigaction(SIGABRT, &sigIntHandler, NULL);
    sigaction(SIGIOT, &sigIntHandler, NULL);
    sigaction(SIGBUS, &sigIntHandler, NULL);
    sigaction(SIGFPE, &sigIntHandler, NULL);
    sigaction(SIGHUP, &sigIntHandler, NULL);
    sigaction(SIGILL, &sigIntHandler, NULL);
    sigaction(SIGHUP, &sigIntHandler, NULL);
    sigaction(SIGQUIT, &sigIntHandler, NULL);
    sigaction(SIGSYS, &sigIntHandler, NULL);
    
    // create boiler plate - necessary actors
    top = new actorBase("TOP", NO_PARENT);
    top->spawnChildActor<actorIni>("INI");
    
    // start these first
    top->start();
    
    // get ini filepath
    std::string ini;
    // if supplied on command line, take that
    if ( argc == 2 ) {
        ini = argv[1];
    }
    // or create default from program whereabouts
    else {
        std::string progname = argv[0];
        size_t slash = progname.rfind("/");
        if (slash != std::string::npos) {
            progname = progname.substr(slash+1, std::string::npos);
        }
        ini = pwd + "/" + progname + "-settings.xml"; 
    }

    // kick-off request to ini to load ini file
    messageHolder outbuffer;
    messageIni* imr = new messageIni(ini);
    SetMessageHolder_(outbuffer,"INI","HRN", imr);
    top->postRequest(outbuffer);
    
    // wait for ini handler to load data
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // another boiler plate actor 
    top->spawnChildActor<actorLog>("LOG")->start();
    
    // and add user application actors
    applicationAddInitialActors();
    
    // run loop
    while(running_ && applicationRunLoop()) {}
    
    // exited run loop
    std::cout << "Application finishing" << std::endl;

    // close threads
    top->finish();

    //cleanup
    delete top;

    // and user cleanup
    applicationCleanup();
    
    std::cout << "Application exiting" << std::endl;
    exit(0);

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
