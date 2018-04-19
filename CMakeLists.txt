cmake_minimum_required(VERSION 3.9)
project(OpenGLViewer)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/FindTBB)

set(CMAKE_CXX_STANDARD 14)
set(USE_IMGUI TRUE)

set(VTK_DIR "~/develop/opensource/VTK-build/")
set(TBB_ROOT_DIR "~/develop/opensource/tbb2018_20171205oss")
if (WIN32)
    set(OpenCV_DIR "~/develop/opensource/opencv-build/install")
endif()

# TBB
find_package(TBB)
message(STATUS "TBB_VERSION: ${TBB_VERSION}")
include_directories(${TBB_INCLUDE_DIRS})
add_definitions(${TBB_DEFINITIONS})



find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(glm REQUIRED)
include_directories(${glm_INCLUDE_DIR})

find_package(VTK REQUIRED)
message(STATUS "VTK_VERSION: ${VTK_VERSION}")
include(${VTK_USE_FILE})

# Find OpenCV
find_package( OpenCV REQUIRED )
message(STATUS "${OpenCV_VERSION}")
message(STATUS "${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})

if (USE_IMGUI)
    add_definitions(-D__WITH_IMGUI)
    # IMGUI
    set(IMGUI_DIR imgui-glew)
    set(IMGUI_SOURCES
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/imgui_demo.cpp
            ${IMGUI_DIR}/imgui_draw.cpp)
    set(IMGUI_HEADERS
            ${IMGUI_DIR}/imconfig.h
            ${IMGUI_DIR}/imgui.h
            ${IMGUI_DIR}/imgui_internal.h
            ${IMGUI_DIR}/stb_rect_pack.h
            ${IMGUI_DIR}/stb_textedit.h
            ${IMGUI_DIR}/stb_truetype.h)

    include_directories(${IMGUI_DIR})
    add_library(ImGui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
    set(EXTRA_LIBS ${EXTRA_LIBS} ImGui)
    set(IMGUI_IMPL ${IMGUI_DIR}/imgui_impl_glfw_gl3.h ${IMGUI_DIR}/imgui_impl_glfw_gl3.cpp ImageProvider.cpp ImageProvider.h StereoWindow.cpp StereoWindow.h)
endif()

add_executable(OpenGLViewer
        ${IMGUI_IMPL}
        main.cpp
        shader.h shader.cpp
        ShadowMapQuad.h ShadowMapQuad.cpp
        json.hpp
        )

target_link_libraries(
        OpenGLViewer
        pthread
        ${TBB_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${VTK_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OpenCV_LIBS}
        ${EXTRA_LIBS}
)

