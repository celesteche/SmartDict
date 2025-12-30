cmake_minimum_required(VERSION 3.16)

project(SmartDict VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 核心步骤：引入 Qt 模块
# 我们需要 Widgets(界面), Network(网络), Sql(数据库), Concurrent(多线程)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network Sql Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Sql Concurrent)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    databasehelper.cpp
    databasehelper.h
    networkmanager.cpp
    networkmanager.h
    filehelper.cpp
    filehelper.h
    logger.cpp
    logger.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SmartDict
        ${PROJECT_SOURCES}
    )
# 如果后续有资源文件(.qrc)，会在这里添加
endif()

# 链接库
target_link_libraries(SmartDict PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Concurrent
)

set_target_properties(SmartDict PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS SmartDict
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
