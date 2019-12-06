# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)
# 将当前 "./src/main/cpp" 目录下的所有源文件保存到 "NATIVE_SRC" 中，然后在 add_library 方法调用。
aux_source_directory(./src/main/cpp/common   COMMON_SRC )
aux_source_directory(./src/main/cpp/egl      EGL_SRC )
aux_source_directory(./src/main/cpp/objects  OBJECTS_SRC )
aux_source_directory(./src/main/cpp/program  PROGRAM_SRC )
aux_source_directory(./src/main/cpp/utils    UTILS_SRC )
aux_source_directory(./src/main/cpp/nativegl PROJECT_SRC )

# 为log_common.h 指定平台
IF (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    ADD_DEFINITIONS(-DWindows)
ELSE (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DLinux)
ENDIF ()


find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that you want CMake to locate.
        log)

# 引入外部so libzip 供源文件编译
add_library(zip
            SHARED
            IMPORTED )
set_target_properties(zip PROPERTIES IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libzip.so)
set_target_properties(zip PROPERTIES LINKER_LANGUAGE CXX)

add_library(
        native-egl
        SHARED
        ${COMMON_SRC}
        ${EGL_SRC}
        ${OBJECTS_SRC}
        ${PROGRAM_SRC}
        ${UTILS_SRC}
        ${PROJECT_SRC}
        )
target_link_libraries(
        native-egl
        EGL
        GLESv3
        android
        zip
        log)







aux_source_directory(./src/main/cpp/gpufilter GPU_FILTER_SRC )
aux_source_directory(./src/main/cpp/gpufilter/components GPU_FILTER_COM_SRC )
aux_source_directory(./src/main/cpp/gpufilter/render GPU_FILTER_RENDER_SRC )


# 在系统找出预编译的android库，指定在CMake脚本下的别名为android-lib
# android参数 for native windows
find_library( android-lib android )

add_library(
        gpu-filter
        SHARED
        ${COMMON_SRC}
        ${EGL_SRC}
        ${GPU_FILTER_SRC}
        ${GPU_FILTER_COM_SRC}
        ${GPU_FILTER_RENDER_SRC}
        )
target_link_libraries(
        gpu-filter
        EGL
        GLESv2
        GLESv3
        android
        log)

