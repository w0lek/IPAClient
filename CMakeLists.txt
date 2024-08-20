cmake_minimum_required(VERSION 3.5)

set(APP_NAME IPAClient)

project(${APP_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui Network)

add_definitions(-DSERVER_PORT_NUM=60556)
add_definitions(-DAPP_NAME="${APP_NAME}")

set(SRC_FILES
  src/main.cpp
  src/Path.cpp
  src/NCriticalPathReportParser.cpp
  src/NCriticalPathToolsWidget.cpp
  src/NCriticalPathParameters.cpp
  src/NCriticalPathItem.cpp
  src/NCriticalPathModel.cpp
  src/NCriticalPathModelLoader.cpp
  src/NCriticalPathView.cpp
  src/NCriticalPathWidget.cpp
  src/NCriticalPathItemDelegate.cpp
  src/CustomMenu.cpp
  src/VprProcess.cpp
  src/client/ConvertUtils.cpp
  src/client/GateIO.cpp
  src/client/TcpSocket.cpp
  src/client/ByteArrayUtils.cpp
  src/client/TelegramHeader.cpp
  src/client/TelegramBuffer.cpp
  src/client/RequestCreator.cpp
  src/client/TelegramParser.cpp
)

set(HDR_FILES
    src/Path.h
  src/NCriticalPathReportParser.h
  src/NCriticalPathToolsWidget.h
  src/NCriticalPathParameters.h
  src/NCriticalPathItem.h
  src/NCriticalPathModel.h
  src/NCriticalPathModelLoader.h
  src/NCriticalPathView.h
  src/NCriticalPathWidget.h
  src/NCriticalPathItemDelegate.h
  src/CustomMenu.h
  src/VprProcess.h
  src/client/ConvertUtils.h
  src/client/GateIO.h
  src/client/CommConstants.h
  src/client/TcpSocket.h
  src/client/ByteArrayUtils.h
  src/client/TelegramHeader.h
  src/client/TelegramFrame.h
  src/client/TelegramBuffer.h
  src/client/RequestCreator.h
  src/client/TelegramParser.h
  src/client/ZlibUtils.cpp
  src/client/ZlibUtils.h
)

add_executable(${APP_NAME} ${SRC_FILES} ${HDR_FILES}
    IPAClient.qrc)

target_link_libraries(${APP_NAME} PUBLIC 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
    -lz
)


