cmake_minimum_required(VERSION 3.27)

if (UNIX)
    set(CMAKE_CXX_COMPILER g++-13)
    set(CMAKE_C_COMPILER gcc)
endif ()

project(daw_project)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
PRIVATE
audioDecoder
projectFile
portaudio
)

add_subdirectory(unit_tests)