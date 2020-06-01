QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    download_thread.cpp \
    http.cpp \
    main.cpp \
    acfundownloader.cpp \
    search_thread.cpp

HEADERS += \
    acfundownloader.h \
    download_thread.h \
    http.h \
    search_thread.h \
    ts_info.h

FORMS += \
    acfundownloader.ui

RC_ICONS = icon.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

win32:CONFIG(release, debug|release): LIBS += -LF:/AcFunDownloader-cpp/libcurl/lib/ -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -LF:/AcFunDownloader-cpp/libcurl/lib/ -llibcurld
else:unix: LIBS += -LF:/AcFunDownloader-cpp/libcurl/lib/ -llibcurl

INCLUDEPATH += F:/AcFunDownloader-cpp/libcurl/include
DEPENDPATH += F:/AcFunDownloader-cpp/libcurl/lib
