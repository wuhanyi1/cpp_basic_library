project(common LANGUAGES CXX)

# 非 cache 变量，每个文件都加一句
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB source CONFIGURE_DEPENDS src/*.cpp)    
add_library(${PROJECT_NAME} SHARED ${source})
# 使用 PUBLIC 属性，这样别人链接这个库时可以把头文件暴露出去
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp pthread)