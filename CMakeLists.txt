cmake_minimum_required(VERSION 4.2)
project(parser VERSION 1.1 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(core_lib STATIC ${SRC_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (MSVC)
    target_compile_options(core_lib PRIVATE /W4 /WX /EHsc /Zc:__cplusplus)
else ()
    target_compile_options(core_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

add_executable(parser src/main.cpp)
target_link_libraries(parser PRIVATE core_lib)
if (MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(parser PRIVATE -static)
endif()

set_target_properties(parser PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")