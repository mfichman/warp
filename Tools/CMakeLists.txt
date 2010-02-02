file(GLOB files "*.cpp")
add_executable(RoadTool ${files})

target_link_libraries(RoadTool OgreMain_d.lib Plugin_OctreeSceneManager_d.lib) 
