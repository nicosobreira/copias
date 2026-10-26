cmake_minimum_required(VERSION 3.15)

project(project
  LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(project_flags INTERFACE)

target_compile_features(project_flags INTERFACE c_std_23)

target_compile_definitions(project_flags INTERFACE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_compile_options(project_flags INTERFACE
  $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wformat=2
    -Wconversion
    -Wsign-conversion
    -Wfloat-conversion
    -Wnull-dereference
    -Wimplicit-fallthrough
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:RelWithDebInfo>:-O3;-g;-fno-omit-frame-pointer>
  >

  $<$<C_COMPILER_ID:MSVC>:
    /W4
    /WX
  >
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddProjectExecutable)
include(AddProjectLibrary)

add_subdirectory(project)
