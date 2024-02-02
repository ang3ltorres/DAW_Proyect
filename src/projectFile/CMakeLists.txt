project(projectFiles)

set(PROJECT_SOURCES
        src/project_file.cpp)

set(LIB_NAME projectFile)

add_library(${LIB_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories (${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# set_target_properties(${LIB_NAME}
#                       PROPERTIES
#                       ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build)