project(audioDecoder)

set(PROJECT_SOURCES
        src/decoder.cpp)

set(LIB_NAME audioDecoder)

add_library(${LIB_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# set_target_properties(${LIB_NAME}
#                       PROPERTIES
#                       ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build)