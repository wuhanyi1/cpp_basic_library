cmake_minimum_required(VERSION 3.12)

include (.cmake/depend/utils.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(why_basic_library LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-builtin-macro-redefined -Wno-sign-compare -Wl,--unresolved-symbols=ignore-in-shared-libs")
message("${CMAKE_CXX_FLAGS}")
option(ENABLE_SUBMODULE_BUILD "option for whether build project by submodule" ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

if (ENABLE_CONAN_BUILD)
  message(STATUS "build project by conan")
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/depend/conan.cmake)
elseif (ENABLE_SUBMODULE_BUILD)
  message(STATUS "build project by submodule")
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/depend/thirdparty.cmake)
endif()

file(WRITE ${PROJECT_BINARY_DIR}/empty.cpp "")
add_library (${PROJECT_NAME} SHARED ${PROJECT_BINARY_DIR}/empty.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
  common
  config
  log
)

force_redefine_file_macro_for_sources(${PROJECT_NAME})

add_subdirectory(src)
# add_subdirectory(sample)
add_subdirectory(tests)