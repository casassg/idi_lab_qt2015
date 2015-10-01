TEMPLATE    = app
QT         += opengl

RESOURCES += shaders.qrc

LIBS += -lGLEW
INCLUDEPATH +=  /usr/include/glm

HEADERS += MyGLWidget.h

SOURCES += main.cpp \
        MyGLWidget.cpp
