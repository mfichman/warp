cmake_minimum_required(VERSION 2.6)
project(Criterium)


set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_definitions("/DWARP_DEBUG")
else()
add_definitions("/DWARP_RELEASE")
endif()

set(EXECUTABLE_OUTPUT_PATH ${Criterium_SOURCE_DIR}\\..)
set(LIBRARY_OUTPUT_PATH ${Criterium_SOURCE_DIR}\\...\\Binary)



include_directories(${Criterium_SOURCE_DIR}\\..\\Include)

include_directories("C:\\Include")
include_directories("C:\\OgreSDK\\include")
include_directories("C:\\OgreSDK\\include\\opt")
include_directories("C:\\OgreSDK\\samples\\include")
include_directories("C:\\Include\\Bullet")

include_directories("D:\\Include")
include_directories("D:\\Include\\OgreSDK\\include")
include_directories("D:\\Include\\OgreSDK\\include\\opt")
include_directories("D:\\Include\\OgreSDK\\samples\\include")
include_directories("D:\\Include\\Bullet")

link_directories("C:\\Libraries")
link_directories("C:\\OgreSDK\\lib")
link_directories("C:\\OgreSDK\\lib\\opt")

link_directories("D:\\Libraries")
link_directories("D:\\Include\\OgreSDK\\lib")
link_directories("D:\\Include\\OgreSDK\\lib\\opt")

add_subdirectory(..\\Source ..\\Build\\Temp)

SET_TARGET_PROPERTIES(Warp PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCMTD")

