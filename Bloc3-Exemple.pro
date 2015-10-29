TEMPLATE    = app
QT         += opengl

LIBS += -lGLEW
INCLUDEPATH +=  /usr/include/glm
INCLUDEPATH += Model

HEADERS += MyGLWidget.h

RESOURCES += shaders.qrc

SOURCES += main.cpp \
        MyGLWidget.cpp Model/model.cpp
