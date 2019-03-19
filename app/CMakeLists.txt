# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)
# 将当前 "./src/main/cpp" 目录下的所有源文件保存到 "NATIVE_SRC" 中，然后在 add_library 方法调用。
aux_source_directory(./src/main/cpp/common   COMMON_SRC )
aux_source_directory(./src/main/cpp/egl      EGL_SRC )
aux_source_directory(./src/main/cpp/objects  OBJECTS_SRC )
aux_source_directory(./src/main/cpp/program  PROGRAM_SRC )
aux_source_directory(./src/main/cpp/nativegl NATIVE_GL_SRC )

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
# AS CMake sample
add_library( # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp)

target_link_libraries( # Specifies the target library.
        native-lib
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})





add_library(
        native-egl
        SHARED
        ${COMMON_SRC}
        ${EGL_SRC}
        ${OBJECTS_SRC}
        ${PROGRAM_SRC}
        ${NATIVE_GL_SRC}
        )
target_link_libraries(
        native-egl
        EGL
        GLESv2
        android
        ${log-lib})

