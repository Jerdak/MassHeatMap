#-------------------------------------------------
#
# Project created by QtCreator 2014-03-30T15:52:43
#
#-------------------------------------------------

QT       += core gui opengl
CONFIG += console

TARGET = MassHeatMap
TEMPLATE = app
PRECOMPILED_HEADER = Stable.h

SOURCES += main.cpp\
        MainWindow.cpp \
    ParallelPlane.cpp \
    Database.cpp

HEADERS  += MainWindow.h \
    Stable.h \
    ParallelPlane.h \
    Database.h

FORMS    += MainWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/build_OpenSceneGraph/lib/ -losg
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/build_OpenSceneGraph/lib/ -losgd -losgViewerd -losgGAd -losgDBd -losgUtild -losgQtd


INCLUDEPATH += $$PWD/../../Libraries/OpenSceneGraph-3.2.1-rc2/include
DEPENDPATH += $$PWD/../../Libraries/OpenSceneGraph-3.2.1-rc2/include

INCLUDEPATH += $$PWD/../../Libraries/build_OpenSceneGraph/include
DEPENDPATH += $$PWD/../../Libraries/build_OpenSceneGraph/include
