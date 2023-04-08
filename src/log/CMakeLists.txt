project(log LANGUAGES CXX)

# 非 cache 变量，每个文件都加一句
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB source CONFIGURE_DEPENDS ./*.cpp)    
add_library(${PROJECT_NAME} SHARED ${source})
# 使用 PUBLIC 属性，这样别人链接这个库时可以把头文件暴露出去
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/
        #${why_basic_library_SOURCE_DIR}/src/common/include/
)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp common config)