cmake_minimum_required(VERSION 2.6)
project(Criterium)



#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_definitions("/DWARP_DEBUG")
else()
add_definitions("/DWARP_RELEASE")
endif()

set(EXECUTABLE_OUTPUT_PATH ${Criterium_SOURCE_DIR}/..)
set(LIBRARY_OUTPUT_PATH ${Criterium_SOURCE_DIR}/.../Binary)



include_directories(${Criterium_SOURCE_DIR}/../Include)


include_directories("../Deps/Include")
include_directories("../Deps/Include/OgreSDK/include")
include_directories("../Deps/Include/OgreSDK/include/opt")
include_directories("../Deps/Include/OgreSDK/samples/include")
include_directories("../Deps/Include/OgreSDK/include/OGRE")
include_directories("../Deps/Include/OgreSDK/include/OGRE/plugins/OctreeSceneManager")
include_directories("../Deps/Include/Bullet")


link_directories("../../Deps/Libraries/")
link_directories("../../Deps/Include/OgreSDK/lib")
link_directories("../../Deps/Include/OgreSDK/lib/opt")
link_directories("../../Deps/Include/OgreSDK/lib/debug")
link_directories("../../Deps/Include/OgreSDK/lib/release")
link_directories("../../Deps/Include/OgreSDK/lib/debug/opt")
link_directories("../../Deps/Include/OgreSDK/lib/release/opt")

add_subdirectory(../Source ../Build/Temp)

#SET_TARGET_PROPERTIES(Warp PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCMTD")

