#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T17:24:49
#
#-------------------------------------------------

QT       += core gui

win32:RC_FILE = roundabout.rc
macx:ICON = myapp.icns

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
    roundabout.ico

RESOURCES += \
    roundabout.qrc
