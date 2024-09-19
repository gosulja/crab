cmake_minimum_required(VERSION 3.10)
project(crab C)

set(CMAKE_C_STANDARD 11)

# Keep as debug until releases
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include    # Include root directory
  ${CMAKE_SOURCE_DIR}/include/freetype
  ${CMAKE_SOURCE_DIR}/include/glad
  ${CMAKE_SOURCE_DIR}/include/GLFW
  ${CMAKE_SOURCE_DIR}/include/KHR
)

# Link libraries
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# GLFW
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
)

# FreeType
# Manually specify the path to the FreeType library
set(FREETYPE_LIBRARY "${CMAKE_SOURCE_DIR}/lib/freetype.lib")
set(FREETYPE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/freetype")

if (EXISTS "${FREETYPE_LIBRARY}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FREETYPE_LIBRARY})
else()
    message(FATAL_ERROR "Freetype library not found")
endif()

# Add DLLs to build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
    "${CMAKE_SOURCE_DIR}/lib/freetype.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
