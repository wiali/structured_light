#-------------------------------------------------
#
# Project created by QtCreator 2011-07-04T23:16:35
#
#-------------------------------------------------

QT  += core gui opengl

TARGET = slapp
TEMPLATE = app

QMAKE_LIBDIR += $$PWD/Libraries/OpenCV/lib/ $$PWD/Libraries/GL_Files/lib/

LIBS += -lopencv_world331 -lopengl32 -lGLU32 -lglut32
INCLUDEPATH += $$PWD/Libraries/OpenCV/include $$PWD/Libraries/GL_Files/include

SOURCES += main.cpp\
        slapp.cpp\
        ../three_step_phase_shift.cpp\
        glpointcloud.cpp \
    capturedialog.cpp

HEADERS  += slapp.h glpointcloud.h \
    capturedialog.h

FORMS    += slapp.ui \
    capturedialog.ui
