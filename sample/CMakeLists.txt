project(why_sample)

set(CMAKE_CXX_STANDARD 17)

file(GLOB source CONFIGURE_DEPENDS src/*.cpp)

foreach(SAMPLE_SOURCE ${source})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" SAMPLE_NAME ${SAMPLE_SOURCE})
    add_executable(${SAMPLE_NAME} ${SAMPLE_SOURCE})

    if (${SAMPLE_NAME} STREQUAL "log_sample")
        target_link_libraries(${SAMPLE_NAME} PRIVATE log pthread)
    endif()
    
endforeach()

