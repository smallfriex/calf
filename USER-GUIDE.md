
# CALF User Guide
version 1.20

To gain an understanding of the framework, I suggest you build and run the demo application included in this repository. Study the code and the program's output. Send a few send SIGTSTP signals (Ctrl-Z) to see the program's status, and then send a SIGKILL (Ctrl-C) to gracefully quit the demo.

When coding your own program using CALF, you'll want to start with a copy of `executive.cpp` from the `demo/src/` directory, which you will modify to suit your needs. Then you'll code up message and actor components to suit your application, using the ones under `demo/src/` as examples. What follows here is really a set of notes and pointers on how to best go about that activity, rather than a comprehensive user guide.

# Provisions
* CALF is a framework. You build the library and include it in your project. You get a low-overhead asynchronous messaging system that you can use to pass messages of any conceivable type between your actors, each actor running on its own thread.
* You have a basic monitoring system for those messages, and you get a simple ini file handler (xml syntax) to use for any application settings you may have, plus you get a logging system to use. Everything else is up to you.
* The monitoring stats can be printed to STDOUT at any time via a SIGTSTP sent to the running program. NB: This is only implemented on 'nix systems.  
* There's no centralised *post office*, rather each actor can send messages to its direct children or parent. When a message is sent, if the framework doesn't find an addressee in the sending actor's children, the message will be passed to its parent, and the parent will first check its own address, and then check its children, and then pass on to its parent.
    * This is done that way so messages aren't bouncing around the family tree indefinitely. If a suitable mailbox isn't found by the time the message reaches the top-level actor, an error is raised (and you'll know you need to recode something).
    * This has pros and cons. One advantage is speed of messaging. If you have a well organised structure, messages only have a few hops and there's no bottleneck imposed by a centralised *post office* thread. The disadvantage is that you need to lay out your parent-child hierarchy carefully.
    * The LOG and INI actors are created for you by the top-level actor, which is also created, in main.
    * You could, in theory, have several top-level actors (ones without parents) and the two family trees wouldn't be able to exchange messages. But the framework only creates one top-level actor for you.
* You can create pools of actors of the same type and these pools can dynamically size as required.
    * Each pool can receive messages addressed as follows: PARENT-POOL-POOLNAME-TASKID. If an actor doesn't exist for that task id its created by the POOL and the message forwarded. 
    * From then on any message directed at that POOL's TASKID will be digested by that same POOL actor.  
    * When the sending actor is finished with a certain task, he can send a flush command (event) to that same address and the TASK actor will clean-up what it needs to then get *flushed* so the task can be reused by the POOL for another task without the overhead of the actor needing to be `delete`d and re`new`ed. 
    * So POOLs are useful for offloading data-processing tasks, or file-handling activity. But some applications will not require them.
* To keep things simple, this framework sends status reports to STDOUT and logging to STDERR. You can redirect these streams to appropriate files/pipes when you launch a program built with this framework. It's easy to turn a compiled CALF program into a well-behaving systemd service, in which case, the streams are handled for you by systemd.

# Software Requirements
* `main()` is provided by the library. Your application needs to provide a file, typically called executive.cpp with the following function bodies:

~~~cpp
void applicationPreStart();   
void applicationPreLaunch();
void applicationAddInitialActors();
bool applicationRunLoop();
void applicationCleanup();
~~~

- use `executive.cpp` in `demo/src/` as a starting point.

# Actor Requirements
* The naming convention for actor software components is to start with the word `actor` and message components to start with the word `message`. That way messages and actors can cohabit the same directory, but when listed in alphabetical order it's easy to discern the messages from the actors. Of course, you're free to chose a different convention or even put the two types of components in their own directories.
* Each actor instance should be assigned a code, usually an abbreviated name that clearly identifies the actor type, but you could have any string. This abbreviation is used to address messages. For actors contained within a pool, they will have the prefix POOL and suffix id assigned automatically.
* Each actor class will need to register actions for each message and event that it wants to digest. This is the glue that holds the framework to the actors. Each action should be a protected method in the actor class. NB: Another convention is that these methods begin with the word `action`.
* Most messages are generated by your application. Events are system-generated. For example, an `eventNewMinute` is provided by the framework and this is a good place to do any housekeeping required by the actor. Digesting this event is a good idea, since the actor won't have to rely on receiving a message directed at it in order to perform housekeeping activities. NB: Events are implemented as messages, but typically don't have any member variables (data).
* Another good convention would be to not do much in the executive.cpp in the way of sending messages. Better to spawn child actors and have them do the application business logic. Similar to a constructor — get out quick — and keep the executive clean.

# Message Requirements
* There needs to be some boiler-plate code in each class you intend to use as a message type. I.e., at the top of the message class, within the public section, for messageLog, we have the line:

~~~cpp
static Message_T typeName() {return MESSAGE_TYPE;}
~~~

This static member function is used to distinguish the *type* of message being constructed, sent, and received. C++ doesn't support reflection, but using the predefinted macro __FILE__, which works with most c++ compilers, we can get at a meaningful string useful for debugging – providing that the naming convention for message header files is adhered to – if not, then just use a unique string here instead. You'll also have to use a string literal if you intend to have more than one message definition per header file.

Furthermore, the typeName must be passed into the baseMessage constructor in each message constructor, e.g.:

~~~cpp
messageLog(const int level, const std::string& message) : messageBase(typeName) {
~~~

For speed advantage, it's actually the address of this function that will be compared when determining message type. Since the function is declared static, C++ compilers ensure there's one version of this no matter how many instances of the class there are, and it will live at the same address throughout the life of the program. If the language definition changes in regard to this directive (unlikely) you would have to stick to a C++ version where this behaviour is guaranteed.

NB: You can call typeName() on messages actors receive for use in logging etc, in which case it would be down to the user code to parse out the message name from the full file name.

* In almost all cases, the Actor sending the message `new`s the message, and the receiving actor `delete`s the message when the message has been digested. Events don't contain data, so don't need deleting. It's as simple as that, there should be little room for memory leeks if those two rules are followed.
* Each Actor has an `inbuffer_` and an `outbuffer_` provided for messages and since each actor has its own thread for message processing, there can be no data conflicts on these buffers.