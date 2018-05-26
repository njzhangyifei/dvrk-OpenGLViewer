cmake_minimum_required(VERSION 3.5)
project(dvrk-OpenGLViewer)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/FindTBB)

set(CMAKE_CXX_STANDARD 14)

set(USE_IMGUI TRUE)
#set(WITH_ROS TRUE)
#set(ARCLAB TRUE)

set(VTK_DIR "~/develop/opensource/VTK-build/")
set(TBB_ROOT_DIR "~/develop/opensource/tbb2018_20171205oss")
if (ARCLAB)
    add_definitions(-D__ARCLAB)
    set(VTK_DIR "/home/arclab/opensource/vtk-8.1.0")
    #set(OpenCV_DIR "/home/arclab/opensource/opencv-3.4.1")
    set(TBB_ROOT_DIR "/home/arclab/opensource/tbb2018_20171205oss")
endif()

if (WIN32)
    set(OpenCV_DIR "~/develop/opensource/opencv-build/install")
endif()

if (NOT DEFINED WIN32)
    find_package(catkin REQUIRED COMPONENTS
      cv_bridge
      image_transport
      sensor_msgs
    )
    catkin_package(
    #  INCLUDE_DIRS include
    #  LIBRARIES dvrk-OpenGLViewer
        CATKIN_DEPENDS cv_bridge image_transport message_runtime sensor_msgs
    #  DEPENDS system_lib
    )
    include_directories(
      ${catkin_INCLUDE_DIRS}
    )
    set(EXTRA_LIBS ${EXTRA_LIBS} ${catkin_LIBRARIES})
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
    set(IMGUI_IMPL ${IMGUI_DIR}/imgui_impl_glfw_gl3.h ${IMGUI_DIR}/imgui_impl_glfw_gl3.cpp
            vtkExternalOpenGLRenderWindowFixed.cpp vtkExternalOpenGLRenderWindowFixed.h)
endif()

if (WITH_ROS)
    add_definitions(-D__WITH_ROS)
    set(ROS_INTEROP
            ROSStereoImageProvider.cpp ROSStereoImageProvider.h
            )
endif()


add_executable(dvrk-OpenGLViewer
        ${IMGUI_IMPL}
        main.cpp
        shader.h shader.cpp
        ImageProvider.cpp ImageProvider.h
        StereoWindow.cpp StereoWindow.h
        IRenderProcedure.h
        TextureRenderer.cpp TextureRenderer.h
        GLError.cpp GLError.h
        json.hpp
        get_cwd.h get_cwd.cpp
        ${ROS_INTEROP}
        CameraTextureRenderer.cpp CameraTextureRenderer.h
        VTKRenderProcedure.cpp VTKRenderProcedure.h
        IImageAligned.h
        VTKCameraManager.cpp VTKCameraManager.h
        VideoCaptureImageSource.cpp VideoCaptureImageSource.h)

file(GLOB FRAG_SHADERS "*.frag")
file(GLOB VERT_SHADERS "*.vert")
#file(COPY ${FRAG_SHADERS} DESTINATION ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION})
#file(COPY ${VERT_SHADERS} DESTINATION ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION})

target_link_libraries(
        dvrk-OpenGLViewer
        pthread
        ${TBB_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${VTK_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OpenCV_LIBS}
        ${EXTRA_LIBS}
)
