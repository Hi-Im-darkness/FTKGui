#-------------------------------------------------
#
# Project created by QtCreator 2018-05-07T16:47:17
#
#-------------------------------------------------


TARGET = FTKGui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

TRANSLATIONS += translation_vi.ts

win32 {
    RC_FILE = FTKGui.rc
}

OTHER_FILES += \
    FTKGui.rc \
    FTKGui.exe.manifest.xml
