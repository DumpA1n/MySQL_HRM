cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(MySQL_HRM)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
add_compile_options(-w)

set(MySQL_DIR "C:/Program Files/MySQL/MySQL Connector C++ 9.0")

# 查找头文件目录
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/utils
    ${MySQL_DIR}/include
    ${MySQL_DIR}/include/mysql
    ${MySQL_DIR}/include/mysqlx
)

# 指定库文件路径
link_directories(
    ${MySQL_DIR}/lib64/vs14
)

# 设置所有目标的输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)

# 添加所有源文件
set(SOURCES
    ${CMAKE_SOURCE_DIR}/main.cpp
)

# 添加可执行文件
add_executable(MySQL_HRM ${SOURCES})

# 设置输出路径和文件名
set_target_properties(MySQL_HRM PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "MySQL_HRM"
)

# 链接库文件
target_link_libraries(MySQL_HRM libssl libcrypto mysqlcppconn mysqlcppconnx)

