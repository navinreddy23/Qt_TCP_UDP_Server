QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32:RC_ICONS = Images/icon.ico
macx:ICON = Images/icon.icns

GIT_HASH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)\\\""
GIT_BRANCH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --abbrev-ref HEAD)\\\""
unix:BUILD_TIMESTAMP="\\\"$$system(date -u +\""%Y-%m-%dT%H:%M:%SUTC\"")\\\""
win32:BUILD_TIMESTAMP="\\\"$$system(date /t)\\\""
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
DEFINES += GIT_HASH=$$GIT_HASH GIT_BRANCH=$$GIT_BRANCH BUILD_TIMESTAMP=$$BUILD_TIMESTAMP GIT_VERSION=\\\"$$GIT_VERSION\\\"

unix:TARGET = Qt_TCP_UDP_Server$${GIT_VERSION}
win32:TARGET = Qt_TCP_UDP_Server$${GIT_VERSION}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060300    # disables all the APIs deprecated before Qt 6.3.0

SOURCES += \
    Dialogs/AboutDialog.cpp \
    Programs/IMU/Imu.cpp \
    Programs/Server/TcpHandler.cpp \
    Programs/Server/TcpServer.cpp \
    Programs/Server/UdpServer.cpp \
    Programs/Misc/Logger.cpp \
    Programs/Serial/Serial.cpp \
    Widgets/TcpWidget.cpp \
    Widgets/UdpWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Dialogs/AboutDialog.h \
    Programs/IMU/Imu.h \
    Programs/Server/TcpHandler.h \
    Programs/Server/TcpServer.h \
    Programs/Server/UdpServer.h \
    Programs/Misc/Logger.h \
    Programs/Serial/Serial.h \
    Widgets/TcpWidget.h \
    Widgets/UdpWidget.h \
    mainwindow.h

FORMS += \
    Dialogs/AboutDialog.ui \
    Widgets/TcpWidget.ui \
    Widgets/UdpWidget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
