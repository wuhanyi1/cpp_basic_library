project(why_tests)

set(CMAKE_CXX_STANDARD 14)

file(GLOB source CONFIGURE_DEPENDS ./*.cpp)

foreach(TEST_SOURCE ${source})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" TEST_NAME ${TEST_SOURCE})
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    if (${TEST_NAME} STREQUAL "log_tests")
        target_link_libraries(${TEST_NAME} PRIVATE log pthread)
    elseif(${TEST_NAME} STREQUAL "config_tests")     
        target_link_libraries(${TEST_NAME} PRIVATE why_basic_library)
    endif()
    
endforeach()

# 重写 __FILE__ 宏为相对路径
force_redefine_file_macro_for_sources(config_tests)
