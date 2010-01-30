file(GLOB files "*.cpp")
add_executable(Criterium ${files})
target_link_libraries(Criterium OgreMain.lib OIS.lib CEGUIBase.lib OgreGUIRenderer.lib ode.lib) 
