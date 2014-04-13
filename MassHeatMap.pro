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
    HeatColor.cpp

HEADERS  += MainWindow.h \
    Stable.h \
    ParallelPlane.h \
    Database.h \
    ParallelPlaneManager.h \
    qcustomplot.h \
    PrincipalComponentAnalyzer.h \
    DrawableMesh.h \
    CudaWrapper.h \
    HeatColor.h

#OTHER_FILES += CudaKernel.cu
FORMS    += MainWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/build_OpenSceneGraph/lib/ -losg -losgViewer -losgGA -losgDB -losgUtil -losgQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/build_OpenSceneGraph/lib/ -losgd -losgViewerd -losgGAd -losgDBd -losgUtild -losgQtd


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

### CUDA settings <-- may change depending on your system

#CUDA_DIR = "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v5.5"            # Path to cuda toolkit install


#CUDA_OBJECTS_DIR = $$PWD/CudaObj
#CUDA_LIBS = cuda cudart
#CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')
#NVCC_LIBS = $$join(CUDA_LIBS,' -l','-l', '')
#CUDA_LIB = $$CUDA_DIR\\lib
#CUDA_BIN = $$CUDA_DIR\\bin
#SYSTEM_NAME = Win32         # Depending on your system either 'Win32', 'x64', or 'Win64'
#SYSTEM_TYPE = 32            # '32' or '64', depending on your system
#NVCCFLAGS = --use_fast_math --cl-version 2010
#CUDA_ARCH = sm_30

#QMAKE_LIBDIR += $$CUDA_LIB\\$$SYSTEM_NAME
#DEPENDPATH += $$CUDA_LIB\\$$SYSTEM_NAME

#QMAKE_LFLAGS_RELEASE = /NODEFAULTLIB:msvcrt.lib
#QMAKE_LFLAGS_DEBUG   = /NODEFAULTLIB:msvcrtd.lib

#LIBS += -lcudart \
#        -lcuda


#CONFIG(debug, debug|release) {
#    # Debug mode
#    cuda_d.input = OTHER_FILES
#    cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
#    cuda_d.commands = $$CUDA_DIR/bin/nvcc.exe -c -g -G -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS $$NVCCFLAGS --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#    cuda_d.dependency_type = TYPE_C
#    QMAKE_EXTRA_COMPILERS += cuda_d
#}
#else {
#    # Release mode
#    cuda.input = OTHER_FILES
#    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
#    cuda.commands = $$CUDA_DIR/bin/nvcc.exe $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS $$NVCCFLAGS -m$$SYSTEM_TYPE -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#    cuda.dependency_type = TYPE_C
#    QMAKE_EXTRA_COMPILERS += cuda
#}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Dependencies/CudaTest/release/ -lCudaTest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Dependencies/CudaTest/debug/ -lCudaTest

INCLUDEPATH += $$PWD/Dependencies/CudaTest
DEPENDPATH += $$PWD/Dependencies/CudaTest

win32:CONFIG(release, debug|release): LIBS += -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32/" -lcudart
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32/" -lcudart

INCLUDEPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32"
DEPENDPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v5.5/lib/Win32"
