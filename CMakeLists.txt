cmake_minimum_required(VERSION 3.4)

project(DerpPSX VERSION 0.0.1
            DESCRIPTION "Derp PSX emu"
            LANGUAGES CXX)

cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory( glfw )

# if(MSVC)
#     SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
# endif()

set (CMAKE_CXX_STANDARD 17)

add_executable(DerpPSX 
    src/main.cpp
    src/mem.cpp
    src/app.cpp
    src/gui.cpp
    src/logging.cpp
    src/R3000A.cpp
    src/io_controller.cpp
    src/dma.cpp
    src/gpu.cpp
    src/timer.cpp
    src/core.cpp
    src/renderers/OpenGL/opengl.cpp
    src/cdrom.cpp
    src/runner/runner.cc
    src/runner/debug/debugger.cc
    src/disk/disk.cc
    src/cpu/gte/gte.cc
    src/cpu/gte/commands.cc

    external/imgui/imgui_impl_opengl3.cpp
    external/imgui/imgui_impl_glfw.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)  

target_include_directories(DerpPSX PUBLIC 
    include
    external/imgui/
    external
    )

target_compile_features(DerpPSX PUBLIC cxx_std_11)

target_include_directories(DerpPSX PRIVATE ${PROJECT_SOURCE_DIR}/glew/include)

target_link_libraries(DerpPSX ${OPENGL_LIBRARIES} glfw GLEW)
