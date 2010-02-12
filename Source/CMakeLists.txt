file(GLOB files "*.cpp")
add_executable(Criterium ${files})
target_link_libraries(Criterium libbulletdynamics_d.lib libbulletcollision_d.lib libbulletmath_d.lib lua-gd.lib OgreMain_d.lib OIS_d.lib CEGUIBase_d.lib OgreGUIRenderer_d.lib Plugin_OctreeSceneManager_d.lib) 
