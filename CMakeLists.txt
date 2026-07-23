cmake_minimum_required(VERSION 3.10)
project(ScheduleManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(SOURCES
    src/qt_gui.cpp
)

set(HEADERS
    src/qt_gui.h
)

# 让 Qt 的 moc 处理 qt_gui.h
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_executable(myschedule ${SOURCES} ${MOC_SOURCES})

target_link_libraries(myschedule 
    Qt5::Widgets 
    pthread
)
