cmake_minimum_required (VERSION 2.6)
project (TysiacServer)
SET(CMAKE_CXX_FLAGS "-std=c++0x")

set(Boost_USE_STATIC_LIBS ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS system thread) 

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/lib)
	link_directories(${Boost_LIBRARY_DIRS})
	add_executable(TysiacServer main.cpp engine/GameLogic.cpp engine/GameLogic.hpp network/manager.h network/manager.cpp network/server.h network/server.cpp network/session.h network/session.cpp)
	target_link_libraries(TysiacServer ${Boost_LIBRARIES})
endif()