cmake_minimum_required(VERSION 4.2)
project(parser VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(core_lib STATIC ${SRC_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (MSVC)
    target_compile_options(core_lib PRIVATE /W4 /WX /EHsc /Zc:__cplusplus)
else ()
    target_compile_options(core_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

add_executable(start src/main.cpp)
target_link_libraries(start PRIVATE core_lib)

set_target_properties(start PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/res")