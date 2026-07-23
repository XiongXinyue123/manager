cmake_minimum_required(VERSION 3.10)
project(ScheduleManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 OpenSSL（用于 SHA-256 加密）
find_package(OpenSSL REQUIRED)

# 包含头文件目录
include_directories(${OPENSSL_INCLUDE_DIR})

# 源文件
set(SOURCES
    src/main.cpp
)

# 生成可执行文件
add_executable(myschedule ${SOURCES})

# 链接 OpenSSL 和 pthread（多线程）
target_link_libraries(myschedule ${OPENSSL_LIBRARIES} pthread)
