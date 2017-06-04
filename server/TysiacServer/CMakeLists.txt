cmake_minimum_required(VERSION 2.6)
project(TysiacServer)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_definitions(-DBOOST_ALL_NO_LIB)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED unit_test_framework system)

include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(engine)
add_subdirectory(network)
add_subdirectory(tests)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} engine network)