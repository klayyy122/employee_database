cmake_minimum_required(VERSION 3.16)
project(EmployeeDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/usr/lib/qt6")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

message(STATUS "Qt6 version: ${Qt6_VERSION}")
message(STATUS "Qt6 include dir: ${Qt6Core_INCLUDE_DIRS}")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(SOURCES
    src/main.cpp
    gui/window.cpp
    src/core/database.cpp
)

add_executable(Database ${SOURCES})


target_link_libraries(Database Qt6::Core Qt6::Widgets)


target_include_directories(Database PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

