# CALF: C++ Actor Light Framework

CALF is cross-compatible lightweight open source implementation of the actor model for C++. Lightweight means there is not much code and, not many dependencies. In contrast to CAF: C++ Actor Framework, CALF aims to provide just the features you need with low overhead. Cross-compatible, it's been designed on linux using standard c++, compiled and tested on OSX, and should, if needed, run on Windows with some minor adjustments.

## Who is CALF for?

* CALF is for you if you have a small or medium project that is suited to the actor model framework, and you don't need advanced features such as synchronous messages or distributed across computers.
* CALF is for you is you need a simple actor model framework to facilitate passing of messages between software components (actors), either for private or commercial use.
* CALF is for you if CAF or other actor and messaging frameworks seem like overkill. CALF needs C++11 minimum, and has no dependency on Boost. CALF uses the std c++ library (including std::thread) and has a dependency of pugixml, itself an open-source lightweight library. 

## What Features does CALF provide?

* CALF has been designed ground-up to be thread-safe. All the usual advice applies when coding a multi-threaded program, suffice to say, the messaging platform inside CALF has been battle-tested in a multi-threaded environment.
* CALF has basic monitoring built-in. Total messages and current messages stats for all actors can be summoned at any time.
* CALF has a logging system and ini handler at the user's disposal.
 
## Who wrote CALF?

* CALF was written by me, Craig Phillips. I can be contacted through my work __website__: https://www.softwareguru.ca
* It was time for me to give something back to the open-source community. It is my hope that someone out there finds this useful, as I have found useful so much open source software for my own projects. CALF is licensed under [MIT license](LICENSE.md) terms.
* I would be delighted if people want to contribute to the project by way of bug fixes or enhancements, but please bear in mind the mantra of this project. I don't intend for this to grow into a larger messaging platform. Those exist already.

## How do I build CALF ?

* By default, CALF uses CMake to build itself, so:

~~~bash
cd build
cmake -DBUILD_DEMO=ON .
make
~~~

will build a static library you can use in your project. Or, since there's not many files, you could reference the source code directly into your own build or `makefile`.

* It doesn't make much sense to build a shared object of CALF — because it includes main() — so it can't be shared by multiple applications. It's a framework, not a utility library.
* The use of std::threads will pull in a dependency on a thread implementation, typically pthreads on 'mix systems.
* The CALF library has a dependency on pugixml, so you would need to build that. Thankfully, pugixml is also open-source, on github, and uses CMake. So:

~~~bash
cd ..
git clone https://github.com/zeux/pugixml.git
cd pugixml
cmake -DBUILD_SHARED_LIBS=ON .
make
sudo make install
sudo ldconfig
~~~

will build and install pugixml for you. Again, you could chose to include the few files in your own build or `Makefile`, but this would be the CMake approach.

## How do I use CALF?

* There is a [USER-GUIDE.md](USER-GUIDE.md) file in the same directory of this `README.md` file that outlines how to use CALF in your project. Also, you'll find a sample program in the test/ directory which is reasonably well-commented. 

