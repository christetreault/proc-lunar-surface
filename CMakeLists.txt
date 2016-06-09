cmake_minimum_required(VERSION 3.0)
project(lunarSurface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#subdirs(src)
set(SOURCE_FILES src/main.cpp src/Window.cpp src/Shader.cpp src/OBJObject.cpp src/SkyBox.cpp
        src/Light.cpp src/Util.cpp src/Texture.cpp src/Scene.cpp src/SceneGraph.cpp
        src/Landscape.cpp src/Doodad.cpp)

add_executable(lunarSurface ${SOURCE_FILES})
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include(findSOIL.cmake)
pkg_search_module(SOIL REQUIRED SOIL)
include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

target_link_libraries(lunarSurface ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${SOIL_LIBRARIES})
