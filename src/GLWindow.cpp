#include "GLWindow.h"
#include <iostream>
#include <ngl/Vec3.h>
#include <ngl/Light.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Material.h>
#include <ngl/ShaderLib.h>
#include <QColorDialog>

#define INCREMENT 0.01f

//----------------------------------------------------------------------------------------------------------------------
GLWindow::GLWindow(const QGLFormat _format, QWidget *_parent ) : QGLWidget( _format, _parent ), m_clothInfo(), m_cloth()
{

    // set this widget to have the initial keyboard focus
    setFocus();
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    this->resize(_parent->size());
	m_wireframe=false;
	m_rotation=0.0;
	m_scale=1.0;
	m_position=0.0;

    m_time = 0.0f;

    m_drawType=GL_TRIANGLES;

    m_clothInfo.anchoredTopLeft = true;
    m_clothInfo.anchoredTopRight = true;
    m_clothInfo.anchoredBottomLeft = false;
    m_clothInfo.anchoredBottomRight = false;
    m_clothInfo.dampingConstant = 512.0f;
    m_clothInfo.height = 1.636f;
    m_clothInfo.heightNum = 16;
    m_clothInfo.springConstant = 1024.0f;
    m_clothInfo.width = 2.56f;
    m_clothInfo.widthNum = 16;

    m_shouldRotate = false;
    m_shouldTranslateSphere = false;
    m_spinXFace = 0;
    m_spinYFace = 0;
}

// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::initializeGL()
{

  ngl::NGLInit::instance();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  //smoothing, i think?
  glEnable(GL_SMOOTH);
  glShadeModel(GL_SMOOTH);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  /// create our camera
  ngl::Vec3 eye(2,2,2);
  ngl::Vec3 look(0,0,0);
  ngl::Vec3 up(0,1,0);

  m_camera = new ngl::Camera(eye,look,up);
  m_camera->setShape(45,float(1024/720),0.1,300);
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  // we are creating a shader called Phong
  shader->createShaderProgram("Phong");
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader("PhongVert",ngl::VERTEX);
  shader->attachShader("PhongFrag",ngl::FRAGMENT);
  // attach the source
  shader->loadShaderSource("PhongVert","shaders/PhongVertex.glsl");
  shader->loadShaderSource("PhongFrag","shaders/PhongFragment.glsl");
  // compile the shaders
  shader->compileShader("PhongVert");
  shader->compileShader("PhongFrag");
  // add them to the program
  shader->attachShaderToProgram("Phong","PhongVert");
  shader->attachShaderToProgram("Phong","PhongFrag");
  // now bind the shader attributes for most NGL primitives we use the following
  // layout attribute 0 is the vertex data (x,y,z)
  shader->bindAttribute("Phong",0,"inVert");
  // attribute 1 is the UV data u,v (if present)
  shader->bindAttribute("Phong",1,"inUV");
  // attribute 2 are the normals x,y,z
  shader->bindAttribute("Phong",2,"inNormal");

  // now we have associated this data we can link the shader
  shader->linkProgramObject("Phong");
  // and make it active ready to load values
  (*shader)["Phong"]->use();
  // the shader will use the currently active material and light0 so set them
  ngl::Material m(ngl::GOLD);
  // load our material values to the shader into the structure material (see Vertex shader)
  m.loadToShader("material");
  // we need to set a base colour as the material isn't being used for all the params
  shader->setShaderParam4f("Colour",0.23125f,0.23125f,0.23125f,1);

  ngl::Light light(ngl::Vec3(16,16,16),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT);
  // now create our light this is done after the camera so we can pass the
  // transpose of the projection matrix to the light to do correct eye space
  // transformations
  ngl::Mat4 iv=m_camera->getViewMatrix();
  iv.transpose();
  light.setTransform(iv);
  light.setAttenuation(1,0,0);
  light.enable();
  // load these values to the shader as well
  light.loadToShader("light");

  // we are creating a shader called Texture
  shader->createShaderProgram("Texture");
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader("TextureVert",ngl::VERTEX);
  shader->attachShader("TextureFrag",ngl::FRAGMENT);
  // attach the source
  shader->loadShaderSource("TextureVert","shaders/TextureVert.glsl");
  shader->loadShaderSource("TextureFrag","shaders/TextureFrag.glsl");
  // compile the shaders
  shader->compileShader("TextureVert");
  shader->compileShader("TextureFrag");
  // add them to the program
  shader->attachShaderToProgram("Texture","TextureVert");
  shader->attachShaderToProgram("Texture","TextureFrag");
  // now bind the shader attributes for most NGL primitives we use the following
  // layout attribute 0 is the vertex data (x,y,z)
  shader->bindAttribute("Texture",0,"inVert");
  // attribute 1 is the UV data u,v (if present)
  shader->bindAttribute("Texture",1,"inUV");
  // attribute 2 are the normals x,y,z
  //shader->bindAttribute("Texture",2,"vertNormals");

  // now we have associated this data we can link the shader
  shader->linkProgramObject("Texture");
  // and make it active ready to load values
  (*shader)["Texture"]->use();
  // the shader will use the currently active material and light0 so set them
  ngl::Material fabric(ngl::Colour(0.f,0.f,0.f), ngl::Colour(0.8f,0.8f,0.8f), ngl::Colour(0.f,0.f,0.f));
  // load our material values to the shader into the structure material (see Vertex shader)
  fabric.loadToShader("material");
  // we need to set a base colour as the material isn't being used for all the params
  //shader->setShaderParam4f("Colour",0.23125f,0.23125f,0.23125f,1);
  shader->registerUniform("Texture","widthNum");
  shader->registerUniform("Texture","heightNum");
  shader->setUniform("widthNum", (GLint)m_clothInfo.widthNum);
  shader->setUniform("heightNum", (GLint)m_clothInfo.heightNum);

  // load these values to the shader as well
  light.loadToShader("light");

  // we are creating a shader called NormalGeneration
  shader->createShaderProgram("NormalGeneration");
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader("NormalsVert",ngl::VERTEX);
  shader->attachShader("NormalsFrag",ngl::FRAGMENT);
  // attach the source
  shader->loadShaderSource("NormalsVert","shaders/NormalGeneration.vert");
  shader->loadShaderSource("NormalsFrag","shaders/NormalGeneration.frag");
  // compile the shaders
  shader->compileShader("NormalsVert");
  shader->compileShader("NormalsFrag");
  // add them to the program
  shader->attachShaderToProgram("NormalGeneration","NormalsVert");
  shader->attachShaderToProgram("NormalGeneration","NormalsFrag");
  // now bind the shader attributes for most NGL primitives we use the following
  // layout attribute 0 is the vertex data (x,y,z)
  shader->bindAttribute("NormalGeneration",0,"inVert");
  // attribute 1 is the UV data u,v (if present)
  shader->bindAttribute("NormalGeneration",1,"inUV");
  // now we have associated this data we can link the shader
  shader->linkProgramObject("NormalGeneration");
  // and make it active ready to load values
  shader->use("NormalGeneration");
  shader->registerUniform("NormalGeneration","widthNum");
  shader->registerUniform("NormalGeneration","heightNum");
  shader->setUniform("widthNum", (GLint)m_clothInfo.widthNum);
  shader->setUniform("heightNum", (GLint)m_clothInfo.heightNum);

  shader->use("Texture");
  shader->registerUniform("Texture","MVP");
  shader->registerUniform("Phong","MVP");
  createTextures();
  createVAO();

  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createSphere("sphere",m_cloth.m_sphere.m_radius,40);

  CS::Vert viewportVerts[6];

  //bottom left
  viewportVerts[0].p = ngl::Vec3(-1.0f, -1.0f, -0.5f);
  viewportVerts[0].uv = ngl::Vec2(0.0f, 0.0f);
  viewportVerts[0].n = ngl::Vec3(0.0f, 0.0f, 0.0f);

  //bottom right
  viewportVerts[1].p = ngl::Vec3(1.0f, -1.0f, -0.5f);
  viewportVerts[1].uv = ngl::Vec2(1.0f, 0.0f);
  viewportVerts[1].n = ngl::Vec3(0.0f, 0.0f, 0.0f);

  //top right
  viewportVerts[2].p = ngl::Vec3(1.0f, 1.0f, -0.5f);
  viewportVerts[2].uv = ngl::Vec2(1.0f, 1.0f);
  viewportVerts[2].n = ngl::Vec3(0.0f, 0.0f, 0.0f);

  //bottom left
  viewportVerts[3].p = ngl::Vec3(-1.0f, -1.0f, -0.5f);
  viewportVerts[3].uv = ngl::Vec2(0.0f, 0.0f);
  viewportVerts[3].n = ngl::Vec3(0.0f, 0.0f, 0.0f);

  //top right
  viewportVerts[4].p = ngl::Vec3(1.0f, 1.0f, -0.5f);
  viewportVerts[4].uv = ngl::Vec2(1.0f, 1.0f);
  viewportVerts[4].n = ngl::Vec3(0.0f, 0.0f, 0.0f);

  //top left
  viewportVerts[5].p = ngl::Vec3(-1.0f, 1.0f, -0.5f);
  viewportVerts[5].uv = ngl::Vec2(0.0f, 1.0f);
  viewportVerts[5].n = ngl::Vec3(0.0f, 0.0f, 0.0f);

  m_viewportVAO=ngl::VertexArrayObject::createVOA(GL_TRIANGLES);
  m_viewportVAO->bind();

  m_viewportVAO->setData(6*sizeof(CS::Vert),viewportVerts[0].uv.m_x);
  //set vert to be input 0
  m_viewportVAO->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(CS::Vert),5);
  //same for input 1
  m_viewportVAO->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(CS::Vert),0);
  //same for input 2
  m_viewportVAO->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(CS::Vert),2);
  m_viewportVAO->setNumIndices(6);
  m_viewportVAO->unbind();

  startTimer(10);
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void GLWindow::resizeGL( int _w, int _h )
{
  glViewport(0,0,_w,_h);
  //the viewport doesn't seem to size properly for some reason so we might as well set the aspect ratio manually
  m_camera->setShape(45,1.368327402f,0.1,300);

}

void GLWindow::createVAO()
{
    glPointSize(8);
    m_vao=ngl::VertexArrayObject::createVOA(m_drawType);
    m_vao->bind();

    const unsigned int size = m_cloth.getPointsArraySizeCopy();
    GLfloat *data = new GLfloat[size];
    m_cloth.getPoints(data);
    const unsigned int indexSize = m_cloth.getIndicesArraySize();
    GLuint *indexData = new GLuint[indexSize];
    m_cloth.getIndices(indexData);

    //demoing grabbing the data directly from the cloth
    //(we have to make a copy for the texture anyway so sadly there's no performance increase)
    m_vao->setIndexedData(m_cloth.getPointsArraySizeDirect(), (*m_cloth.getFirstIndex()), m_cloth.getIndicesArraySizeBytes(), indexData, GL_UNSIGNED_INT, GL_STREAM_DRAW);
    m_vao->setNumIndices(m_cloth.getIndicesArraySize());
    //set vert to be input 0
    m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(CS::Particle),4);
    //same for input 1
    m_vao->setVertexAttributePointer(1,1,GL_FLOAT,sizeof(CS::Particle),1);
    m_vao->unbind();

    //might as well create the position texture while we have the data
    glActiveTexture(GL_TEXTURE1);
    glBindBuffer(GL_TEXTURE_BUFFER, m_posBuffer);
    glBufferData(GL_TEXTURE_BUFFER, size, NULL, GL_STREAM_DRAW);  // Alloc
    glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data);              // Fill

    // Generate texture "wrapper" around buffer object
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, m_posBufferTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_posBuffer);

    delete[] data;
    delete[] indexData;
}

void GLWindow::updateVAO()
{
    m_vao->bind();

    const unsigned int size = m_cloth.getPointsArraySizeCopy();
    GLfloat *data = new GLfloat[size];
    m_cloth.getPoints(data);  
    m_vao->updateIndexedData(m_cloth.getPointsArraySizeDirect(), (*m_cloth.getFirstIndex()), GL_STREAM_DRAW);

    m_vao->unbind();

    //might as well update the position texture while we have the data
    glActiveTexture(GL_TEXTURE1);
    glBindBuffer(GL_TEXTURE_BUFFER, m_posBuffer);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data);              // Fill

    delete[] data;
}

void GLWindow::loadMatricesToShader(ngl::Transformation &_transform, std::string _shaderName)
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)[_shaderName.c_str()]->use();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=_transform.getMatrix();
  MV=_transform.getMatrix()*m_camera->getViewMatrix();
  MVP=MV*m_camera->getProjectionMatrix() ;
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void GLWindow::paintGL()
{

    if(m_wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }

    m_transform.setPosition(m_position);
    m_transform.setScale(m_scale);
    m_transform.setRotation(m_rotation);

    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // grab an instance of the shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["Texture"]->use();

    //run the simulation
    if(!m_cloth.isPaused())
    {
        m_time += 0.01f;
        m_cloth.advance(m_time,0.01f);
        updatePositionTexture();
        renderNormals();
        updateVAO();
    }

    loadMatricesToShader(m_transform, "Texture");
    glViewport(0, 0, width(), height());

    shader->use("Texture");
    glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "widthNum"), (GLint)m_clothInfo.widthNum);
    glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "heightNum"), (GLint)m_clothInfo.heightNum);

    m_vao->bind();
    m_vao->draw();
    m_vao->unbind();

    // get the VBO instance and draw the sphere
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
    shader->use("Phong");
    m_sphereTransform.setPosition(m_cloth.m_sphere.m_pos);
    m_sphereTransform.setScale(m_scale);
    m_sphereTransform.setRotation(m_rotation);
    m_sphereTransform = m_sphereTransform * m_transform;
    loadMatricesToShader(m_sphereTransform, "Phong");
    prim->draw("sphere");

    glViewport(0, 0, m_clothInfo.widthNum * 4, m_clothInfo.heightNum * 4);
    (*shader)["NormalGeneration"]->use();
    m_viewportVAO->bind();
    m_viewportVAO->draw();
    m_viewportVAO->unbind();
}


//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseMoveEvent ( QMouseEvent * _event )
{
    // note the method buttons() is the button state when event was called
    // this is different from button() which is used to check which button was
    // pressed when the mousePress/Release event is generated
    if(m_shouldRotate && _event->buttons() == Qt::LeftButton)
    {
      int diffX=_event->x()-m_origX;
      int diffY=_event->y()-m_origY;
      m_spinXFace += (float) 0.5f * diffY;
      m_spinYFace += (float) 0.5f * diffX;
      m_origX = _event->x();
      m_origY = _event->y();

      m_rotation.m_y += diffX;
      m_rotation.m_x += diffY;
    }
          // right mouse translate code
    else if(m_shouldTranslateView && _event->buttons() == Qt::RightButton)
    {
      int diffX = (int)(_event->x() - m_origXPos);
      int diffY = (int)(_event->y() - m_origYPos);
      m_origXPos=_event->x();
      m_origYPos=_event->y();

//      ngl::Vec4 eye4 = m_camera->getEye();
//      ngl::Vec3 eye(eye4.m_x, eye4.m_y, eye4.m_z);
//      ngl::Vec3 xDir = eye.cross(ngl::Vec3(0.f, 1.f, 0.f));
//      ngl::Vec3 yDir = eye.cross(xDir);
//      ngl::Vec3 translation = (xDir * diffX * -INCREMENT) + (yDir * diffY * INCREMENT);

      m_position.m_x += INCREMENT * diffX;
      m_position.m_y -= INCREMENT * diffY;
//      m_position += translation;
    }

    else if(m_shouldTranslateSphere && _event->buttons() == Qt::MiddleButton)
    {
      int diffX = (int)(_event->x() - m_origXPos);
      int diffY = (int)(_event->y() - m_origYPos);
      m_origXPos=_event->x();
      m_origYPos=_event->y();

//      ngl::Vec4 eye4 = m_camera->getEye();
//      ngl::Vec3 eye(eye4.m_x, eye4.m_y, eye4.m_z);
//      ngl::Vec3 xDir = eye.cross(ngl::Vec3(0.f, 1.f, 0.f));
//      ngl::Vec3 yDir = eye.cross(xDir);
//      ngl::Vec3 translation = (xDir * diffX * -INCREMENT) + (yDir * diffY * INCREMENT);

      m_cloth.m_sphere.move(ngl::Vec3(0.f, -diffY * INCREMENT, -diffX * INCREMENT));
      //m_cloth.m_sphere.move(translation);
    }
}


//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mousePressEvent (QMouseEvent * _event  )
{
    // this method is called when the mouse button is pressed in this case we
    // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
    if(_event->button() == Qt::LeftButton)
    {
      m_shouldRotate =true;
    }
    // right mouse translate mode
    else if(_event->button() == Qt::RightButton)
    {
      m_shouldTranslateView=true;
    }

    else if(_event->button() == Qt::MiddleButton)
    {
      m_shouldTranslateSphere=true;
    }

    m_origXPos = _event->x();
    m_origYPos = _event->y();
}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseReleaseEvent (  QMouseEvent * _event  )
{
    // this event is called when the mouse button is released
    // we then set Rotate to false
    if (_event->button() == Qt::LeftButton)
    {
      m_shouldRotate=false;
    }
          // right mouse translate mode
    if (_event->button() == Qt::RightButton)
    {
      m_shouldTranslateView=false;
    }

    if (_event->button() == Qt::MiddleButton)
    {
      m_shouldTranslateSphere=false;
    }
}

void GLWindow::timerEvent(QTimerEvent *)
{
    updateGL();
}

void GLWindow::createTextures()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    //==================== Cloth Texture ====================

    QImage *image = new QImage();

    const size_t bufsize = 256;
    char buf[bufsize] = "";
    readlink("/proc/self/",buf,bufsize);
    std::string ourDir = std::string(buf);
    std::cout<<"this directory: "<<ourDir<<"\n";

    //texture from http://mikeworleyhistory.com/wp-content/uploads/2015/02/33star.png
    bool loaded=image->load((ourDir + std::string("textures/33star.png")).c_str());
    if(loaded == true)
    {
        int width=image->width();
        int height=image->height();

        unsigned char *data = new unsigned char[width*height*4];
        unsigned int index=0;
        QRgb colour;
        for( int y=0; y<height; ++y)
        {
          for( int x=0; x<width; ++x)
          {
            colour=image->pixel(x,y);

            data[index++]=qRed(colour);
            data[index++]=qGreen(colour);
            data[index++]=qBlue(colour);
            data[index++]=qAlpha(colour);
          }
        }

        glGenTextures(1,&m_clothTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_clothTexture);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

        glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "tex"), 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D); //  Allocate the mipmaps

        delete[] data;
    }
    else
    {
        std::cerr<<"Unable to load cloth texture\n";
    }

    //==================== Positions Texture ====================

    const unsigned int size = m_cloth.getPointsArraySizeCopy();
    GLfloat *data2 = new GLfloat[size];
    m_cloth.getPoints(data2);

    glGenBuffers(1, &m_posBuffer);
    //glActiveTexture(GL_TEXTURE1);
    glBindBuffer(GL_TEXTURE_BUFFER, m_posBuffer);
    glBufferData(GL_TEXTURE_BUFFER, size, NULL, GL_STATIC_DRAW);  // Alloc
    glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data2);              // Fill

    // Generate texture "wrapper" around buffer object
    glGenTextures(1, &m_posBufferTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, m_posBufferTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_posBuffer);
    glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "vertPositions"), 1);
    shader->use("NormalGeneration");
    glUniform1i(glGetUniformLocation(shader->getProgramID("NormalGeneration"), "vertPositions"), 1);
    delete[] data2;

    //==================== Normals Texture ====================
    // I'd really appreciate knowing where I'm going wrong here
    // Some of the following code taken from http://gamedev.stackexchange.com/questions/31162/updating-texture-memory-via-shader

//    glGenBuffers(1, &m_normalsBuffer);
//    //glActiveTexture(GL_TEXTURE1);
//    glBindBuffer(GL_TEXTURE_BUFFER, m_normalsBuffer);
//    glBufferData(GL_TEXTURE_BUFFER, size, NULL, GL_STATIC_DRAW);  // Alloc
//    //glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data2);              // Fill

    /*glGenTextures(1, &m_normalsFramebufferTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_normalsFramebufferTexture);
    //glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_normalsBuffer);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)m_clothInfo.widthNum, (GLsizei)m_clothInfo.heightNum, 0, GL_RGB, GL_FLOAT, 0);
    //glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_normalsFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_normalsFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_normalsFramebufferTexture, 0);

    glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "vertNormals"), 1);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<<"Framebuffer created successfully\n";
    }
    else
    {
        std::cerr<<"Something's wrong with the framebuffer :c\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    // create a texture object
    glGenTextures(1, &m_normalsFramebufferTexture);
    // bind it to make it active
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_normalsFramebufferTexture);
    // set params
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glGenerateMipmapEXT(GL_TEXTURE_2D);  // set the data size but just set the buffer to 0 as we will fill it with the FBO
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_clothInfo.widthNum, (GLsizei)m_clothInfo.heightNum, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "vertNormals"), 2);
    // now turn the texture off for now
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    // create a framebuffer object this is deleted in the dtor
    glGenFramebuffers(1, &m_normalsFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_normalsFramebuffer);

    // create a renderbuffer object to store depth info
  //  glGenRenderbuffers(1, &m_rboID);
  //  glBindRenderbuffer(GL_RENDERBUFFER, m_rboID);

  //  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
  //  // bind
  //  glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attatch the texture we created earlier to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_normalsFramebufferTexture, 0);

    // now attach a renderbuffer to depth attachment point
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboID);
    // now got back to the default render context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GLWindow::updatePositionTexture()
{
    const unsigned int size = m_cloth.getPointsArraySizeCopy();
    GLfloat *data = new GLfloat[size];
    m_cloth.getPoints(data);

    // Create buffer
    glActiveTexture(GL_TEXTURE1);
    glBindBuffer(GL_TEXTURE_BUFFER, m_posBuffer);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data);              // Fill

    delete[] data;
}

void GLWindow::renderNormals()
{
   // Some of the following code taken from http://gamedev.stackexchange.com/questions/31162/updating-texture-memory-via-shader

//    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
//    shader->use("NormalGeneration");

//    glBindFramebuffer(GL_FRAMEBUFFER, m_normalsFramebuffer);
//    glPushAttrib(GL_VIEWPORT_BIT);
//    glViewport(0, 0, (GLsizei)m_clothInfo.widthNum, (GLsizei)m_clothInfo.heightNum);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_BUFFER, m_normalsFramebufferTexture);
//    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, m_posBuffer);
//    //glUniform1i(glGetUniformLocation(shader->getProgramID("NormalGeneration"), "vertPositions"), 1);

//    glClear(GL_COLOR_BUFFER_BIT);
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, m_viewportVBO);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDisableVertexAttribArray(0);

//    m_viewportVAO->bind();
//    m_viewportVAO->draw();
//    m_viewportVAO->unbind();

//    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_normalsFramebuffer);
//    glBindTexture(GL_TEXTURE_2D, m_normalsFramebufferTexture);
//    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, (GLsizei)m_clothInfo.widthNum, (GLsizei)m_clothInfo.heightNum);

//    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

//    glPopAttrib();

//    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
//    (*shader)["NormalGeneration"]->use();

//    // we are now going to draw to our FBO
//    // set the rendering destination to FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, m_normalsFramebuffer);
//    // set the background colour (using blue to show it up)
//    glClearColor(0,0.4,0.5,1);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    // set our viewport to the size of the texture
//    // if we want a different camera we wouldset this here
//    glViewport(0, 0, 16, 16);

//    m_viewportVAO->bind();
//    m_viewportVAO->draw();
//    m_viewportVAO->unbind();

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);

//    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
//    (*shader)["NormalGeneration"]->use();

//    // we are now going to draw to our FBO
//    // set the rendering destination to FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, m_normalsFramebuffer);
//    // set the background colour (using blue to show it up)
//    glClearColor(0,0.4,0.5,1);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    // set our viewport to the size of the texture
//    // if we want a different camera we wouldset this here
//    glViewport(0, 0, 16, 16);

//        glActiveTexture(GL_TEXTURE2);
//        glBindBuffer(GL_TEXTURE_BUFFER, m_normalsBuffer);
//        glBufferData(GL_TEXTURE_BUFFER, 256, NULL, GL_STATIC_DRAW);  // Alloc
//        //glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data);              // Fill

//        // Generate texture "wrapper" around buffer object
//        //glGenTextures(1, &m_textureNames[1]);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_BUFFER, m_normalsFramebufferTexture);
//        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_normalsBuffer);

//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, m_viewportVAO);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDisableVertexAttribArray(0);
//    m_viewportVAO->bind();
//    m_viewportVAO->draw();
//    m_viewportVAO->unbind();

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glBindTexture(GL_TEXTURE_2D, m_normalsFramebufferTexture);
//    //glUniform1i(glGetUniformLocation(shader->getProgramID("Texture"), "vertNormals"), 0);


        //----------------------------------------------------------------------------------------------------------------------
        // draw to our FBO first
        //----------------------------------------------------------------------------------------------------------------------
        // grab an instance of the shader manager
        ngl::ShaderLib *shader=ngl::ShaderLib::instance();
        (*shader)["NormalGeneration"]->use();
        glUniform1i(glGetUniformLocation(shader->getProgramID("NormalGeneration"), "widthNum"), (GLint)m_clothInfo.widthNum);
        glUniform1i(glGetUniformLocation(shader->getProgramID("NormalGeneration"), "heightNum"), (GLint)m_clothInfo.heightNum);

        // we are now going to draw to our FBO
        // set the rendering destination to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_normalsFramebuffer);
        // set the background colour (using blue to show it up)
        glClearColor(0,0.4,0.5,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // set our viewport to the size of the texture
        // if we want a different camera we wouldset this here
        glViewport(0, 0, (GLsizei)m_clothInfo.widthNum, (GLsizei)m_clothInfo.heightNum);

        m_viewportVAO->bind();
        m_viewportVAO->draw();
        m_viewportVAO->unbind();

        //----------------------------------------------------------------------------------------------------------------------
        // now we are going to draw to the normal GL buffer and use the texture created
        // in the previous render to draw to our objects
        //----------------------------------------------------------------------------------------------------------------------
        // first bind the normal render buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // now enable the texture we just rendered to
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_normalsFramebufferTexture);
        // get the new shader and set the new viewport size
        shader->use("Texture");
        // this takes into account retina displays etc
        glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
}

GLWindow::~GLWindow()
{
    ngl::NGLInit *Init = ngl::NGLInit::instance();
    std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
    //delete m_light;
    Init->NGLQuit();
    // clear out our buffers
    glDeleteTextures(1,&m_posBufferTexture);
    glDeleteTextures(1,&m_clothTexture);
    glDeleteTextures(1,&m_normalsFramebufferTexture);
    glDeleteFramebuffers(1,&m_normalsFramebuffer);
    glDeleteBuffers(1,&m_posBuffer);
}

void GLWindow::toggleWireframe(bool _mode)
{
	m_wireframe=_mode;
}

void GLWindow::setXRotation( double _x	)
{
	m_rotation.m_x=_x;
}

void GLWindow::setYRotation( double _y	)
{
	m_rotation.m_y=_y;
}
void GLWindow::setZRotation( double _z )
{
	m_rotation.m_z=_z;
}

void GLWindow::setXScale( double _x	)
{
	m_scale.m_x=_x;
}

void GLWindow::setYScale(	 double _y)
{
	m_scale.m_y=_y;
}
void GLWindow::setZScale( double _z )
{
	m_scale.m_z=_z;
}

void GLWindow::setXPosition( double _x	)
{
	m_position.m_x=_x;
}

void GLWindow::setYPosition( double _y	)
{
	m_position.m_y=_y;
}
void GLWindow::setZPosition( double _z	 )
{
	m_position.m_z=_z;
}

void GLWindow::setDrawType(int _i)
{
    switch(_i)
    {
        case 0 : m_drawType = GL_TRIANGLES; break;
        case 1 : m_drawType = GL_LINES;     break;
        case 2 : m_drawType = GL_POINTS;    break;
        default: m_drawType = GL_TRIANGLES; break;
    }
    createVAO();
}

void GLWindow::togglePaused()
{
    m_cloth.togglePaused();
}

void GLWindow::toggleWind()
{
    m_cloth.toggleWind();
}

void GLWindow::setSphereCollisions(bool _shouldUse)
{
    m_cloth.setSphereCollisions(_shouldUse);
}

void GLWindow::setSelfCollisions(bool _shouldUse)
{
    m_cloth.setSelfCollisions(_shouldUse);
}

void GLWindow::resetCloth()
{
    m_cloth.reset(m_clothInfo);
    m_rotation=0.0;
    m_scale=1.0;
    m_position=0.0;
    createVAO();
}

void GLWindow::setClothHeight(double _height)
{
    m_clothInfo.height = (float)_height;
    resetCloth();
}

void GLWindow::setClothWidth(double _width)
{
    m_clothInfo.width = (float)_width;
    resetCloth();
}

void GLWindow::setClothHeightRes(int _num)
{
    m_clothInfo.heightNum = _num;
    resetCloth();
}

void GLWindow::setClothWidthRes(int _num)
{
    m_clothInfo.widthNum = _num;
    resetCloth();
}

void GLWindow::setSpringConstant(double _constant)
{
    m_clothInfo.springConstant = (float)_constant;
    m_cloth.setSpringConstant(_constant);
}

void GLWindow::setDampingConstant(double _constant)
{
    m_clothInfo.dampingConstant = (float)_constant;
    m_cloth.setDampingConstant(_constant);
}

void GLWindow::setSphereRadius(double _radius)
{
    m_cloth.m_sphere.m_radius = _radius;
}

void GLWindow::setSphereX(double _x)
{
    m_cloth.m_sphere.m_pos.m_x = _x;
}

void GLWindow::setSphereY(double _y)
{
    m_cloth.m_sphere.m_pos.m_y = _y;
}

void GLWindow::setSphereZ(double _z)
{
    m_cloth.m_sphere.m_pos.m_z = _z;
}

void GLWindow::setGravity(double _gravity)
{
    m_cloth.setGravity(_gravity);
}

void GLWindow::setSimSpeed(double _speed)
{
    m_cloth.setSimSpeed(_speed);
}

void GLWindow::reset()
{
    m_cloth.reset(m_clothInfo);
}

void GLWindow::setAnchoredBottomLeft(bool _anchor)
{
    m_cloth.setAnchoredCorner(2, _anchor);
    m_clothInfo.anchoredBottomLeft = _anchor;
}

void GLWindow::setAnchoredBottomRight(bool _anchor)
{
    m_cloth.setAnchoredCorner(3, _anchor);
    m_clothInfo.anchoredBottomRight = _anchor;
}

void GLWindow::setAnchoredTopLeft(bool _anchor)
{
    m_cloth.setAnchoredCorner(0, _anchor);
    m_clothInfo.anchoredTopLeft = _anchor;
}

void GLWindow::setAnchoredTopRight(bool _anchor)
{
    m_cloth.setAnchoredCorner(1, _anchor);
    m_clothInfo.anchoredTopRight = _anchor;
}
