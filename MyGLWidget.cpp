#include <GL/glew.h>
#include <QGLShader>
#include <QGLShaderProgram>
#include "MyGLWidget.h"
#include <QKeyEvent>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
}

void MyGLWidget::initializeGL ()
{
  // glew és necessari per cridar funcions de les darreres versions d'OpenGL
  glewExperimental = GL_TRUE;
  glewInit(); 
  glGetError();  // Reinicia la variable d'error d'OpenGL

  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  loadShaders();
  createBuffers();
  transVec = glm::vec3(0.);
  modelTransform(glm::vec3(0.));
}

void MyGLWidget::paintGL ()
{


  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Activem l'Array a pintar 
  glBindVertexArray(VAO[0]);
 
  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
}

void MyGLWidget::modelTransform(const glm::vec3& transChange) {
    float rad = 0.785398;
    glm::mat4 TG = glm::mat4(1.f);
    TG = glm::rotate(TG,rad,glm::vec3(0.,0.,1.));
    transVec = transVec+transChange;
    TG = glm::translate(TG,transVec);


    //Passem al Vertex shader
    GLint posTG = glGetUniformLocation(program->programId(),"TG");
    glUniformMatrix4fv (posTG, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::createBuffers ()
{
    GLfloat triangle_colors[] = {
      1.0, 1.0, 0.0,
      0.0, 0.0, 1.0,
      1.0, 0.0, 0.0,
    };
    glGenBuffers(1, &VBOTC);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTC);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
    GLuint pos = glGetAttribLocation(program->programId(),"color");
    if(pos==-1)
        std::cerr<<"ERROR BINDIN COLOR"<<std::endl;



    glm::vec3 Vertices[4];  // Tres vèrtexs amb X, Y i Z
    Vertices[0] = glm::vec3(-0.5, -0.5, 0.0);
    Vertices[1] = glm::vec3(0.5, -0.5, 0.0);
    Vertices[2] = glm::vec3(0, 0.5, 0.0);

    // Creació del Vertex Array Object (VAO) que usarem per pintar
    glGenVertexArrays(1, VAO);

    glBindVertexArray(VAO[0]);
        // Creació del buffer amb les dades dels vèrtexs
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
        // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        //COLORINES
        glGenBuffers(1, &VBOTC);
        glBindBuffer(GL_ARRAY_BUFFER, VBOTC);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
        glVertexAttribPointer ( pos , 3 , GL_FLOAT , GL_FALSE , 0 , 0);
        glEnableVertexAttribArray ( pos );
    glBindVertexArray(0);
}

void MyGLWidget::loadShaders(){
    QGLShader fs (QGLShader::Fragment,this);
    fs.compileSourceFile(":/shaders/fragshad.frag");
    QGLShader vs (QGLShader::Vertex,this);
    vs.compileSourceFile(":/shaders/vertshad.vert");
    program = new QGLShaderProgram(this);
    program->addShader(&fs);
    program->addShader(&vs);
    program->link();
    program->bind();
}

void MyGLWidget::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
        case Qt::Key_Left:
            modelTransform(glm::vec3(-0.01,0.,0.));
            break;
        case Qt::Key_Right:
            modelTransform(glm::vec3(+0.01,0.,0.));
            break;
        case Qt::Key_Up:
            modelTransform(glm::vec3(0.,+0.01,0.));
            break;
        case Qt::Key_Down:
            modelTransform(glm::vec3(0.,-0.01,0.));
            break;
        default:
            break;
    }
    updateGL();
}

