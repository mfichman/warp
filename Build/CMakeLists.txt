cmake_minimum_required(VERSION 2.6)
project(Criterium)


set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(EXECUTABLE_OUTPUT_PATH ${Criterium_SOURCE_DIR}\\..)
set(LIBRARY_OUTPUT_PATH ${Criterium_SOURCE_DIR}\\...\\Binary)

include_directories(${Criterium_SOURCE_DIR}\\..\\Include)
include_directories(D:\\OgreSDK\\include)
include_directories(D:\\OgreSDK\\samples\\include)
link_directories(D:\\OgreSDK\\lib)
add_subdirectory(..\\Source ..\\Build\\Temp)
