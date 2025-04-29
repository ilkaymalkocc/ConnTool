cmake_minimum_required(VERSION 3.5)

project(ConnectionTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

qt5_add_resources(GENERATED_RESOURCES ConnectionTool.qrc)

set(APP_ICON_RESOURCE_WINDOWS "appicon.rc")

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    dialog.ui
    openconnectiondialog.h
    openconnectiondialog.cpp
    aboutdialog.cpp
    aboutdialog.h
    aboutdialog.ui
)

add_executable(ConnectionTool WIN32
    ${PROJECT_SOURCES}
    ${GENERATED_RESOURCES}
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_link_libraries(ConnectionTool PRIVATE Qt5::Widgets Qt5::Network)

set_target_properties(ConnectionTool PROPERTIES
    WIN32_EXECUTABLE TRUE
)
