#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T17:24:49
#
#-------------------------------------------------

QT       += core gui

win32:RC_FILE = roundabout.rc
macx:ICON = roundabout.icns
win32:INCLUDEPATH += "C:\\Program Files\\Jack v1.9.6\\includes"
win32:LIBS += $$quote(c:\\Program Files\\Jack v1.9.6\\lib\\libjack.a) $$quote(c:\\Program Files\\Jack v1.9.6\\lib\\libjackserver.a)
unix:LIBS += -ljack

TARGET = Roundabout
TEMPLATE = app


SOURCES += main.cpp\
        roundabout.cpp \
    roundaboutscene.cpp \
    roundabouttestitem.cpp \
    wheelzoominggraphicsview.cpp

HEADERS  += roundabout.h \
    roundaboutscene.h \
    roundabouttestitem.h \
    wheelzoominggraphicsview.h

FORMS    += roundabout.ui

OTHER_FILES += \
    README \
    license.txt \
    roundabout.rc \
    roundabout.ico \
    roundabout.icns \
    images/splash.png \
    images/smiling_alien_32x32.png

RESOURCES += \
    roundabout.qrc
