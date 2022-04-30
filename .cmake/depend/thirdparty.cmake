
set(CMAKE_CXX_FLAGS_SAVE ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wl,--unresolved-symbols=ignore-in-shared-libs")
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/yaml-cpp EXCLUDE_FROM_ALL)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_SAVE})