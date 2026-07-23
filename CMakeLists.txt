cmake_minimum_required(VERSION 3.10)
project(ScheduleManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCES
    src/main.cpp
)

# 生成可执行文件
add_executable(myschedule ${SOURCES})

# 只链接 pthread（多线程）
target_link_libraries(myschedule pthread)
