file(GLOB files "*.cpp")
add_executable(Warp ${files})
target_link_libraries(Warp libbulletdynamics_d.lib libbulletcollision_d.lib libbulletmath_d.lib lua-gd.lib OgreMain_d.lib OIS_d.lib CEGUIBase_d.lib OgreGUIRenderer_d.lib Plugin_OctreeSceneManager_d.lib oscpack_d.lib ws2_32.lib winmm.lib) 
