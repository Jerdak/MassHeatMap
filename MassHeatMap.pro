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
    DrawableMesh.cpp \
    CudaWrapper.cpp \
    HeatColor.cpp \
    OptionWindow.cpp \
    DrawWindow.cpp \
    MeshSegmenter.cpp

HEADERS  += MainWindow.h \
    Stable.h \
    ParallelPlane.h \
    Database.h \
    ParallelPlaneManager.h \
    qcustomplot.h \
    PrincipalComponentAnalyzer.h \
    DrawableMesh.h \
    CudaWrapper.h \
    HeatColor.h \
    OptionWindow.h \
    DrawWindow.h \
    MeshSegmenter.h

#OTHER_FILES += CudaKernel.cu
FORMS    += MainWindow.ui \
    OptionWindow.ui \
    DrawWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/build_OpenSceneGraph/lib/ -losg -losgViewer -losgGA -losgDB -losgUtil -losgQt -losgText
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/build_OpenSceneGraph/lib/ -losgd -losgViewerd -losgGAd -losgDBd -losgUtild -losgQtd -losgTextd


INCLUDEPATH += $$PWD/../../Libraries/OpenSceneGraph-3.2.1-rc2/include
DEPENDPATH += $$PWD/../../Libraries/OpenSceneGraph-3.2.1-rc2/include

INCLUDEPATH += $$PWD/../../Libraries/build_OpenSceneGraph/include
DEPENDPATH += $$PWD/../../Libraries/build_OpenSceneGraph/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/build/x86/vc10/lib/ -lopencv_core248
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/build/x86/vc10/lib/ -lopencv_core248d
else:unix:!macx: LIBS += -L$$PWD/../../Libraries/opencv/build/x86/vc10/lib/ -lopencv_core248

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/build/x86/vc10/lib/ -lopencv_highgui248
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/build/x86/vc10/lib/ -lopencv_highgui248d
else:unix:!macx: LIBS += -L$$PWD/../../Libraries/opencv/build/x86/vc10/lib/ -lopencv_highgui248

INCLUDEPATH += $$PWD/../../Libraries/opencv/build/include
DEPENDPATH += $$PWD/../../Libraries/opencv/build/x86/vc10


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependencies/CudaTest/release/ -lCudaTest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependencies/CudaTest/debug/ -lCudaTest

INCLUDEPATH += $$PWD/Dependencies/CudaTest
DEPENDPATH += $$PWD/Dependencies/CudaTest

win32:CONFIG(release, debug|release): LIBS += -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32/" -lcudart
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32/" -lcudart

INCLUDEPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32"
DEPENDPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32"
