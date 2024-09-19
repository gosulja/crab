cmake_minimum_required(VERSION 3.10)
project(crab C)

set(CMAKE_C_STANDARD 11)

# keep as debug until releases
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
)

# add dlls to build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
