#
# Copyright Alexander Linne 2017 - present.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt
#     or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.8)
project(matrix_multiplication)
set(TARGET matrix_multiplication)

include_directories(${PROJECT_SOURCE_DIR}/../../include)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_CXX_FLAGS "-fconcepts -Wall")

# add pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND LIBRARIES Threads::Threads)

# add OpenCV to the project
if (DEFINED ENV{OPENCV_DIR})
    set(OpenCV_DIR $ENV{OPENCV_DIR})
endif()
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
list(APPEND LIBRARIES ${OpenCV_LIBS})

include_directories(include)
set(SOURCE_FILES
    main.cpp
)

add_executable(
    ${TARGET}
    ${SOURCE_FILES})

target_link_libraries(
    ${TARGET}
    ${LIBRARIES}
)
