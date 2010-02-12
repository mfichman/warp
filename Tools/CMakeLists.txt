
add_executable(RoadTool RoadTool.cpp)
target_link_libraries(RoadTool OgreMain_d.lib Plugin_OctreeSceneManager_d.lib) 


add_executable(CurveTool CurveTool.cpp)
target_link_libraries(CurveTool OgreMain_d.lib)
