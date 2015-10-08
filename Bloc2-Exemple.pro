TEMPLATE    = app
QT         += opengl

LIBS += -lGLEW
INCLUDEPATH +=  /usr/include/glm
INCLUDEPATH +=  Model

RESOURCES += shaders.qrc

HEADERS += MyGLWidget.h

SOURCES += main.cpp \
        MyGLWidget.cpp
SOURCES += Model/model.cpp
