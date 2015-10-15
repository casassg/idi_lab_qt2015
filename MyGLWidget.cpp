#include <GL/glew.h>
#include "MyGLWidget.h"
#include <cmath>

#include <iostream>

MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
    setFocusPolicy(Qt::ClickFocus); // per rebre events de teclat
    scale = 1.0f;
}

void calcCapsaContenidora(const Model &model,glm::vec3& min,glm::vec3& max)
{
    min.x = max.x = model.vertices()[0];
    min.y = max.y = model.vertices()[1];
    min.z = max.z = model.vertices()[2];
    for(unsigned int i = 3;i<model.vertices().size();i+=3) {
        double x = model.vertices()[i];
        double y = model.vertices()[i+1];
        double z = model.vertices()[i+2];
        if(x<min.x) min.x = x;
        else if (x>max.x) max.x = x;
        if(y<min.y) min.y = y;
        else if (y>max.y) max.y = y;
        if(z<min.z) min.z = z;
        else if (z>max.z) max.z = z;
    }
}

void MyGLWidget::initializeGL () 
{
    // glew és necessari per cridar funcions de les darreres versions d'OpenGL
    glewExperimental = GL_TRUE;
    glEnable (GL_DEPTH_TEST);
    glewInit();
    glGetError();  // Reinicia la variable d'error d'OpenGL

    patricio.load("models/Patricio.obj");
    calcCapsaContenidora(patricio,patrMin,patrMax);
    ra=1;


    glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
    carregaShaders();
    createBuffers();
    modelTransform ();
    projectTransform();
    viewTransform();
}



void MyGLWidget::paintPatricio()
{
    float temp = rotate;
    rotate=rotateP;
    modelTransform();
    rotate = temp;
    // Activem el VAO per a pintar la caseta
    glBindVertexArray (VAO_Patricio);
    // pintem
    glDrawArrays (GL_TRIANGLES, 0, patricio.faces().size () * 3);
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

    paintPatricio();
    //paintTerra();



    glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
    // Matriu de transformació de model
    glm::mat4 transform = glm::rotate(glm::mat4(1.0f),rotate,glm::vec3(0.,1.,0.));
    transform = glm::scale(transform, glm::vec3(scale));
    glm::vec3 centreCaixa (-(patrMin.x+patrMax.x)/2,-(patrMin.y+patrMax.y)/2,-(patrMin.z+patrMax.z)/2);
    transform = glm::translate(transform,centreCaixa);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

double calcFOV(double ra, double initialFOV)
{
    if(ra>=1)return initialFOV;
    else {
        double alpha = initialFOV/2;
        double newAlpha = atan(tan(alpha)/ra);
        return newAlpha*2;
    }
}

void MyGLWidget::projectTransform ()
{
    double fov = calcFOV(ra,M_PI/2.);
    glm::mat4 project = glm::perspective(fov,ra,1.,3.);
    glUniformMatrix4fv(projLoc,1,GL_FALSE,&project[0][0]);
}

void MyGLWidget::viewTransform ()
{
    glm::mat4 view = glm::lookAt(glm::vec3(0.,0.,2.),glm::vec3(0.,0.,0.),glm::vec3(0.,1.,0.));
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,&view[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{
    ra = (double)w/(double)h;
    projectTransform();
    updateGL();
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
        rotateP+=M_PI/20;
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
    glGenVertexArrays(1, &VAO_Patricio);
    glBindVertexArray(VAO_Patricio);

    glGenBuffers(1, &VBO_Patricio);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patricio.faces().size() * 3 * 3, patricio.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glGenBuffers(1, &VBO_PatricioCol);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatricioCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patricio.faces().size() * 3 * 3, patricio.VBO_matdiff(), GL_STATIC_DRAW);

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
        glm::vec3(48./255.,63./255.,159./255.),
        glm::vec3(48./255.,63./255.,159./255.),
        glm::vec3(48./255.,63./255.,159./255.),
        glm::vec3(48./255.,63./255.,159./255.),
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



