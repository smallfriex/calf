project(calf)

cmake_minimum_required(VERSION 3.1)

option(BUILD_DEMO "Build Demo Program" OFF)

set(BUILD_DEFINES "" CACHE STRING "Build defines")

if(MSVC)
	option(STATIC_CRT "Use static CRT libraries" OFF)

	# Rewrite command line flags to use /MT if necessary
	if(STATIC_CRT)
		foreach(flag_var
				CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
				CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif(${flag_var} MATCHES "/MD")
		endforeach(flag_var)
	endif()

	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Wfatal-errors)
endif()

if (APPLE) 
	include_directories(/usr/local/include)
endif()


if(DEFINED BUILD_DEFINES)
	foreach(DEFINE ${BUILD_DEFINES})
		add_definitions("-D" ${DEFINE})
	endforeach()
endif()

find_package(pugixml REQUIRED)

include_directories(../src)

set(SOURCES ../src/actorBase.cpp ../src/actorIni.cpp ../src/actorLog.cpp ../src/main.cpp ../src/calf.cpp)

add_library(calf ${SOURCES})

target_compile_features(calf PUBLIC cxx_lambdas)

if(BUILD_DEMO)
	find_package(Threads REQUIRED)
	set (DEMO_SOURCES ../demo/src/actorRouteManager.cpp ../demo/src/actorFleetManager.cpp ../demo/src/executive.cpp ../demo/src/actorReport.cpp ../demo/src/messagePoint.cpp ../demo/src/actorTruck.cpp ../demo/src/actorJourneyTracker.cpp)
	add_executable(demo-calf ${DEMO_SOURCES})
	target_include_directories(demo-calf PUBLIC ../demo/src)
	target_link_libraries(demo-calf calf pugixml Threads::Threads)
	set(EXECUTABLE_OUTPUT_PATH ../demo/build)
endif()
