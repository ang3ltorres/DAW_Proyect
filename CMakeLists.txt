cmake_minimum_required(VERSION 3.27)
project(DAW_Project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -pedantic)

function(add_exe TARGET_NAME)
	file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}/*.cpp")
	add_executable(${TARGET_NAME} ${SOURCES})
	target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}")
endfunction()

add_exe(projectFile)
