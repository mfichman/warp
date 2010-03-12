file(GLOB files "*.cpp")
add_executable(Warp ${files})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(Warp
    libbulletdynamics_d.lib
    libbulletcollision_d.lib
    libbulletmath_d.lib
    lua_d.lib
    OgreMain_d.lib
    OIS_d.lib
    Plugin_OctreeSceneManager_d.lib
    oscpack_d.lib
    ws2_32.lib
    winmm.lib)
else()
target_link_libraries(Warp
    libbulletdynamics.lib
    libbulletcollision.lib
    libbulletmath.lib
    lua.lib
    OgreMain.lib
    OIS.lib
    Plugin_OctreeSceneManager.lib
    oscpack.lib
    ws2_32.lib
    winmm.lib)
endif()
