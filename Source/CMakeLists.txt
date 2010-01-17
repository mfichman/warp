file(GLOB files "*.cpp")
add_executable(Criterium ${files})
target_link_libraries(Criterium OgreMain_d.lib OIS_d.lib) 
