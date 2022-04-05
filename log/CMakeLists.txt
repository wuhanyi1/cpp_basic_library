cmake_minimum_required(VERSION 3.15)

# 非 cache 变量，每个文件都加一句
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(log LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)

file(GLOB source CONFIGURE_DEPENDS *.cpp)    
add_library(${PROJECT_NAME} SHARED ${source})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)