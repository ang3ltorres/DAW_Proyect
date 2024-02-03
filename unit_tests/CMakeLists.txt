

add_executable(UTDecoder UTDecoder.cpp)
add_executable(UTProjectFF UTProjectFF.cpp)


function (link RUN_NAME)
    link_directories(${RUN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

    include_directories(${RUN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    include_directories(${RUN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/audioDecoder/include)
    include_directories(${RUN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/projectFile/include)

    target_link_libraries(${RUN_NAME}
                        PRIVATE
                        audioDecoder
                        projectFile
                        portaudio)

    set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endfunction()

link (UTDecoder)
link (UTProjectFF)



