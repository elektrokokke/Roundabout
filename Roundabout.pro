#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T17:24:49
#
#-------------------------------------------------

QT       += core gui

win32:RC_FILE = roundabout.rc
macx:ICON = roundabout.icns
win32:INCLUDEPATH += "$$(JACK_PATH)\\includes"
win32:LIBS += $$quote($$(JACK_PATH)\\lib\\libjack.a) $$quote($$(JACK_PATH)\\lib\\libjackserver.a)
unix:LIBS += -ljack

TARGET = Roundabout
TEMPLATE = app


SOURCES += main.cpp\
        roundabout.cpp \
    roundaboutscene.cpp \
    roundabouttestitem.cpp \
    wheelzoominggraphicsview.cpp \
    roundaboutthread.cpp \
    roundaboutsequencer.cpp \
    roundabouttoken.cpp

HEADERS  += roundabout.h \
    roundaboutscene.h \
    roundabouttestitem.h \
    wheelzoominggraphicsview.h \
    roundaboutthread.h \
    ringbuffer.h \
    roundaboutsequencer.h \
    roundabouttoken.h

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
