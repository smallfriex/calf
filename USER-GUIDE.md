
# CALF User Guide

To gain an understanding of the framework, I suggest you build and run the demo application included in this repository. Study the code and the program's output. Send a few send SIGTSTP signals (Ctrl-Z) to see the program's status, and then send a SIGKILL (Ctrl-C) to gracefully quit the demo.

When coding your own program using CALF, you'll want to start with a copy of `executive.cpp` from the `demo/src/` directory, which you will modify to suit your needs. Then you'll code up message and actor components to suit your application, using the ones under `demo/src/` as examples. What follows here is really a set of notes and pointers on how to best go about that activity, rather than a comprehensive user guide.

# Provisions
* CALF is a framework. You build the library and include it in your project. You get an asynchronous messaging system that you can use to pass messages of any conceivable type between your actors, each actor running on its own thread.
* You also have a basic monitoring system for those messages, and you have a simple ini file handler (xml syntax) to use for any application settings you may have, plus you get a logging system to use. Everything else is up to you.
* The monitoring stats can be printed to STDOUT at any time via a SIGTSTP sent to the running program. NB: This is only implemented on 'nix systems.  
* There is no centralised *post office*, rather each actor cand sends messages to its direct children, or failing that, passes the message up to its parent.
    * This is done that way so that messages aren't bouncing around the family tree indefinitively. If a suitable mailbox isn't found by the time the message reaches the top-level actor, an error is raised (and you'll know you need to recode something).
    * This has pros and cons. One advantage is speed of messaging. If you have a well laid-out structure, messages only have a few hops and there's no bottleneck imposed by a centralised *post office* thread. The disadvantage is that you need to lay out your parent-child hierarchy carefully.
    * So each actor can reach any of its children, but not its grandchildren. And each actor can reach its parent, and grandparents, great-grandparents, and so on. It can also reach its auntie and uncles, but not its cousins. You can draw a family tree of your actors to check if actors can reach intended recipients with their messages, and if they can't, you can code an intermediary family member to receive the message type you want, re-address it, and send it on.
    * The LOG and INI actors are created for you by the TOP actor so they can be reached by any actor.
* You can create pools of actors of the same type and these pools can dynamically size as required.
    * Each pool can recieve messages addressed as follows: PARENT-POOL-POOLNAME-TASKID. If an actor doesn't exist for that task id its created by the POOL and the message forwarded. 
    * From then on any message directed at that POOL's TASKID will be digested by that same POOL actor.  
    * When the sending actor is finished with a certain task, he can send a flush command to that same address and the TASK actor will clean-up what it needs to and then can be *flushed* so the task can be reused by the POOL for another TASK without the overhead of the actor needing to be `delete`d and re`new`ed. 
    * So POOLs are useful for offloading data-processing tasks, or file-handling activity.
* To keep things simle, this framework sends status reports to STDOUT and logging to STDERR. You can redirect these streams to appropriate files/pipes when you launch a program built with this framework. It's easy to turn a compiled CALF program into a well-behaving System V service, in which case, the streams are handled by the system.

# Software Requirements
* main() is provided by the library. Your application needs to provide a file, typically called executive.cpp with the following function bodies:

```
void applicationPreStart();   
void applicationPreLaunch();
void applicationAddInitialActors();
bool applicationRunLoop();
void applicationCleanup();
```

- use `executive.cpp` in demo/src/ as a starting point.

# Actor Requirements
* The naming convention for actor software components is to start with the word `actor` and message components to start with the word `message`. That way messages and actors can cohabite the same directory, but when listed in alphabetical order it's easy to discern the messages from the actors. Of course, you're free to chose a different convention or even put the two types of components in their own directories.
* Each actor instance should be assigned a code, usually an abbreviated name. I have used 3 capital letters on actor instances that clearly identifies the actor type, but you could have any name. This abbreviation is used to address messages. For actors contained within a pool, they will have the suffice POOL assigned automatically.
* Each actor class will need to register actions for each message and event that it wants to digest. This is the glue that holds the framework to the actors. Each action should be a protected method in the actor class. NB:Another convention is that these methods begin with the word action.
* Most messages are generated by your application. Events are system-generated. For example, a `NEW_MIN` event is provided by the framework and this is a good place to do any housekeeping required by the actor. Digesting this event is a good idea, since the actor won't have to rely on receiving a message directed at it in order to perform housekeeping activities.
* Another good convention would be to not do much in the executive.cpp in the way of sending messages. Better to spawn child actors and have them do the application business logic. Similar to a constructor -- get out quick -- and keep the executive clean.

# Message Requirements
* Each message type needs a unique Type ID. This is set in the `#define` of the header file and must be reported by the virtual method `reportType()`.
* In almost all cases, the Actor sending the message `new`s the message, and the receiving actor `delete`s the message when the message has been digested. Events don't contain data, so don't need deleting. It's as simple as that, there should be little room for memory leeks if those two rules are followed.
* Each Actor has an `inbuffer_` and an `outbuffer_` provided for messages and since each actor has its own thread for message processing, there can be no data conflicts on these buffers.

