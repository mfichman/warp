file(GLOB files "*.cpp")
add_executable(Criterium ${files})
target_link_libraries(Criterium lua-gd.lib OgreMain_d.lib OIS_d.lib CEGUIBase_d.lib OgreGUIRenderer_d.lib ode-gd.lib Plugin_OctreeSceneManager_d.lib) 
