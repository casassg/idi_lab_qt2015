#include <GL/glew.h>
#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
    setFocusPolicy(Qt::ClickFocus); // per rebre events de teclat
    scale = 1.0f;
}

void MyGLWidget::initializeGL () 
{
    // glew és necessari per cridar funcions de les darreres versions d'OpenGL
    glewExperimental = GL_TRUE;
    glEnable (GL_DEPTH_TEST);
    glewInit();
    glGetError();  // Reinicia la variable d'error d'OpenGL
    homer.load("models/HomerProves.obj");



    glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
    carregaShaders();
    createBuffers();
    modelTransform ();
    projectTransform();
    viewTransform();
}

void MyGLWidget::paintHomer()
{
    float temp = rotate;
    rotate=rotateH;
    modelTransform();
    rotate = temp;
    // Activem el VAO per a pintar la caseta
    glBindVertexArray (VAO_Homer);
    // pintem
    glDrawArrays (GL_TRIANGLES, 0, homer.faces().size () * 3);
}

void MyGLWidget::paintTerra()
{

    modelTransform();

    // Activem el VAO per a pintar el terra
    glBindVertexArray (VAO_Terra);

    // pintem
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void MyGLWidget::paintGL ()
{
    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintHomer();
    paintTerra();



    glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
    // Matriu de transformació de model
    glm::mat4 transform = glm::rotate(glm::mat4(1.0f),rotate,glm::vec3(0.,1.,0.));
    transform = glm::scale(transform, glm::vec3(scale));
    transform = glm::rotate(transform, .58f, glm::vec3(1.,0.,0.));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform ()
{
    glm::mat4 project = glm::perspective(M_PI/2.,1.,1.,3.);
    glUniformMatrix4fv(projLoc,1,GL_FALSE,&project[0][0]);
}

void MyGLWidget::viewTransform ()
{
    glm::mat4 view = glm::lookAt(glm::vec3(0.,0.,2.),glm::vec3(0.,0.,0.),glm::vec3(0.,1.,0.));
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,&view[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{
    glViewport(0, 0, w, h);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
    switch (event->key()) {
    case Qt::Key_Escape:
        exit(0);
    case Qt::Key_S: { // escalar a més gran
        scale += 0.05;
        modelTransform ();
        updateGL();
        break;
    }
    case Qt::Key_D: { // escalar a més petit
        scale -= 0.05;
        modelTransform ();
        updateGL();
        break;
    }
    case Qt::Key_R: {
        rotateH+=M_PI/4;
        modelTransform ();
        updateGL();
        break;
    }
    default: event->ignore(); break;
    }
}

void MyGLWidget::createBuffers () 
{
    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Homer);
    glBindVertexArray(VAO_Homer);

    glGenBuffers(1, &VBO_Homer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * homer.faces().size() * 3 * 3, homer.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glGenBuffers(1, &VBO_HomerCol);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * homer.faces().size() * 3 * 3, homer.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // Dades del terra
    // Dos VBOs, un amb posició i l'altre amb color
    glm::vec3 posterra[4] = {
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, 1.0)
    };
    glm::vec3 colterra[4] = {
        glm::vec3(1,0,1),
        glm::vec3(1,0,1),
        glm::vec3(1,0,1),
        glm::vec3(1,0,1)
    };

    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    glGenBuffers(1, &VBO_TerraPos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glGenBuffers(1, &VBO_TerraCol);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glBindVertexArray (0);
}

void MyGLWidget::carregaShaders() 
{
    // Creem els shaders per al fragment shader i el vertex shader
    QGLShader fs (QGLShader::Fragment, this);
    QGLShader vs (QGLShader::Vertex, this);
    // Carreguem el codi dels fitxers i els compilem
    fs.compileSourceFile(":/shaders/fragshad.frag");
    vs.compileSourceFile(":/shaders/vertshad.vert");
    // Creem el program
    program = new QGLShaderProgram(this);
    // Li afegim els shaders corresponents
    program->addShader(&fs);
    program->addShader(&vs);
    // Linkem el program
    program->link();
    // Indiquem que aquest és el program que volem usar
    program->bind();

    // Obtenim identificador per a l'atribut “vertex” del vertex shader
    vertexLoc = glGetAttribLocation (program->programId(), "vertex");
    // Obtenim identificador per a l'atribut “color” del vertex shader
    colorLoc = glGetAttribLocation (program->programId(), "color");
    // Uniform locations
    transLoc = glGetUniformLocation(program->programId(), "TG");
    projLoc = glGetUniformLocation(program->programId(), "proj");
    viewLoc = glGetUniformLocation(program->programId(), "view");
}

