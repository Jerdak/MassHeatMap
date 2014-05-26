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
    Database.cpp \
    ParallelPlaneManager.cpp \
    qcustomplot.cpp \
    PrincipalComponentAnalyzer.cpp \
    OptionWindow.cpp \
    DrawWindow.cpp \
    qParallelPlanePlot.cpp

HEADERS  += MainWindow.h \
    Stable.h \
    ParallelPlane.h \
    Database.h \
    ParallelPlaneManager.h \
    qcustomplot.h \
    PrincipalComponentAnalyzer.h \
    OptionWindow.h \
    DrawWindow.h \
    qParallelPlanePlot.h

#OTHER_FILES += CudaKernel.cu
FORMS    += MainWindow.ui \
    OptionWindow.ui \
    DrawWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependencies/OpenSceneGraph/lib/ -losg -losgViewer -losgGA -losgDB -losgUtil -losgQt -losgText
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependencies/OpenSceneGraph/lib/ -losgd -losgViewerd -losgGAd -losgDBd -losgUtild -losgQtd -losgTextd


INCLUDEPATH += $$PWD/Dependencies/OpenSceneGraph/include
DEPENDPATH += $$PWD/Dependencies/OpenSceneGraph/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependencies/opencv/x86/vc10/lib/ -lopencv_core248
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependencies/opencv/x86/vc10/lib/ -lopencv_core248d
else:unix:!macx: LIBS += -L$$PWD/Dependencies/opencv/x86/vc10/lib/ -lopencv_core248

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependencies/opencv/x86/vc10/lib/ -lopencv_highgui248
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependencies/opencv/x86/vc10/lib/ -lopencv_highgui248d
else:unix:!macx: LIBS += -L$$PWD/Dependencies/opencv/x86/vc10/lib/ -lopencv_highgui248

INCLUDEPATH += $$PWD/Dependencies/opencv/include
DEPENDPATH += $$PWD/Dependencies/opencv/x86/vc10

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependencies/CudaTest/release/ -lCudaTest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependencies/CudaTest/debug/ -lCudaTest

INCLUDEPATH += $$PWD/Dependencies/CudaTest
DEPENDPATH += $$PWD/Dependencies/CudaTest

win32:CONFIG(release, debug|release): LIBS += -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32/" -lcudart
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32/" -lcudart

INCLUDEPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32"
DEPENDPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32"
