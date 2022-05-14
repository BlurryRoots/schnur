# 2021-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
# See license.md for details.

# Version 3.2 and upwards.
CMAKE_MINIMUM_REQUIRED (VERSION 3.2)

# Setup versioning.
set(BUILD_MAJOR 0)
set(BUILD_MINOR 3)
set(BUILD_PATCH 1)
set(BUILD_VERSION "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")

# Expose version information to config.
set(VERSION_NAME_BUFFER_SIZE 16)
set(VERSION_MAJOR ${BUILD_MAJOR})
set(VERISON_MINOR ${BUILD_MINOR})
set(VERSION_PATCH ${BUILD_PATCH})
set(VERSION_FORMAT "v%d.%d.%d")

# Specify version for this project.
project (schnuerchen VERSION ${BUILD_VERSION})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE PATH "" FORCE)
endif()

if (USE_GEDANKEN)
ADD_DEFINITIONS(-DUSE_GEDANKEN=1)
endif()

string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    IS_STANDALON_PROJECT
)

# .
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# Make sure libraries will be places beside executable.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
    # warning level 4 and all warnings as errors
    #add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wunused-result)
endif()

add_definitions(-DSCHNUR_WITH_ASSERT=1)

# Collect source and header files.
file(GLOB_RECURSE SRC
    ${PROJECT_SOURCE_DIR}/src/*.c
)
add_library(schnuerchen ${SRC})
target_include_directories(schnuerchen PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc/>
)
set_property(TARGET schnuerchen PROPERTY C_STANDARD 11)

#.
add_executable(schnuerchen-test EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/test/catch-pch.cpp
    ${PROJECT_SOURCE_DIR}/test/main.cpp
)
target_include_directories(schnuerchen PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Catch/include>
)
set_property(TARGET schnuerchen-test PROPERTY CXX_STANDARD 17)
add_dependencies(schnuerchen-test schnuerchen)
target_link_libraries(schnuerchen-test schnuerchen)

# Build documentation via doxygen.
add_custom_target (schnuerchen-docs
    COMMAND doxygen ${PROJECT_SOURCE_DIR}/docs/Doxyfile
        DEPENDS schnuerchen
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)