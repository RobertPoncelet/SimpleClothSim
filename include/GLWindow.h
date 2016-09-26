#ifndef GL_WINDOW_H__
#define GL_WINDOW_H__

#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/VertexArrayObject.h>
#include <QEvent>
#include <QTimer>
#include <QResizeEvent>
#include <QGLWidget>
#include "Cloth.h"

/// @file GLWindow.h
/// @brief a basic Qt GL window class for ngl demos
/// @author Jonathan Macey (modified by Robert Poncelet)
/// @version 1.0
/// @date 21/03/15
/// @class GLWindow
/// @brief The simulation widget that takes information from the Cloth and draws it to the screen,
/// as well as executing the result of any user input.
class GLWindow : public QGLWidget
{
Q_OBJECT        // must include this if you use Qt signals/slots
public :
  /// @brief Constructor for GLWindow
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Constructor for GLWindow
  /// @param[in] _parent the parent window to create the GL context in
  /// @param[in] _format The format of the OpenGL context.
  //----------------------------------------------------------------------------------------------------------------------
  GLWindow(const QGLFormat _format,QWidget *_parent );

		/// @brief dtor
	~GLWindow();
 public slots :
	/// @brief a slot to toggle wireframe mode
	/// @param[in] _mode the mode passed from the toggle
	/// button
	void toggleWireframe( bool _mode	 );
	/// @brief set the X rotation value
    /// @param[in] _x the value to set
	void setXRotation( double _x	);
	/// @brief set the Y rotation value
    /// @param[in] _y the value to set
	void setYRotation( double _y	);
	/// @brief set the Z rotation value
    /// @param[in] _z the value to set
	void setZRotation( double _z	);
	/// @brief set the X scale value
    /// @param[in] _x the value to set
	void setXScale( double _x	);
	/// @brief set the Y scale value
    /// @param[in] _y the value to set
	void setYScale( double _y	);
	/// @brief set the Z scale value
    /// @param[in] _z the value to set
	void setZScale( double _z	);

	/// @brief set the X position value
    /// @param[in] _x the value to set
	void setXPosition( double _x	);
	/// @brief set the Y position value
    /// @param[in] _y the value to set
	void setYPosition(double _y);
	/// @brief set the Z position value
    /// @param[in] _z the value to set
	void setZPosition(double _z	);

    /// @brief Set the method of drawing the object.
    /// @param[in] _i the index of the draw method.
    void setDrawType(int _i);
    /// @brief Pause the cloth simulation.
    void togglePaused();
    /// @brief Apply wind to the cloth.
    void toggleWind();
    /// @brief Set whether collisions between cloth particles should be used.
    /// @param[in] _shouldUse The value to set.
    void setSelfCollisions(bool _shouldUse);
    /// @brief Set whether collisions between cloth particles and the sphere should be used.
    /// @param[in] _shouldUse The value to set.
    void setSphereCollisions(bool _shouldUse);
    /// @brief Set the height of the cloth geometry.
    /// @param[in] _height The value to set.
    void setClothHeight(double _height);
    /// @brief Set the width of the cloth geomety.
    /// @param[in] _width The value to set.
    void setClothWidth(double _width);
    /// @brief Set how many particles the cloth has in the X direction.
    /// @param[in] _num The value to set.
    void setClothWidthRes(int _num);
    /// @brief Set how many particles the cloth has in the Y direction.
    /// @param[in] _num The value to set.
    void setClothHeightRes(int _num);
    /// @brief Set the spring constant for all the springs.
    /// @param[in] _constant The value to set.
    void setSpringConstant(double _constant);
    /// @brief Set the damping constant for all the springs.
    /// @param[in] _constant The value to set.
    void setDampingConstant(double _constant);
    /// @brief Set the X position of the collision sphere.
    /// @param[in] _x The value to set.
    void setSphereX(double _x);
    /// @brief Set the Y position of the collision sphere.
    /// @param[in] _y The value to set.
    void setSphereY(double _y);
    /// @brief Set the Z position of the collision sphere.
    /// @param[in] _z The value to set.
    void setSphereZ(double _z);
    /// @brief Set the radius of the collision sphere.
    /// @param[in] _radius The value to set.
    void setSphereRadius(double _radius);
    /// @brief Set the gravity force on the particles.
    /// @param[in] _gravity The value to set.
    void setGravity(double _gravity);
    /// @brief Set the speed of the cloth simulation.
    /// @param[in] _speed The value to set.
    void setSimSpeed(double _speed);
    /// @brief Call reset() on the cloth.
    void reset();

    /// @brief Set whether the particle in the bottom left corner is anchored.
    /// @param[in] _anchor The value to set.
    void setAnchoredBottomLeft(bool _anchor);
    /// @brief Set whether the particle in the bottom right corner is anchored.
    /// @param[in] _anchor The value to set.
    void setAnchoredBottomRight(bool _anchor);
    /// @brief Set whether the particle in the top left corner is anchored.
    /// @param[in] _anchor The value to set.
    void setAnchoredTopLeft(bool _anchor);
    /// @brief Set whether the particle in the top right corner is anchored.
    /// @param[in] _anchor The value to set.
    void setAnchoredTopRight(bool _anchor);

    signals:
//    /// @brief Change both the view X rotation and the X rotation spinbox in the UI
//    /// @param[in] _x The value to set.
//    void setRotationX(double _x);
//    /// @brief Change both the view Y rotation and the Y rotation spinbox in the UI
//    /// @param[in] _y The value to set.
//    void setRotationY(double _y);
//    /// @brief Change both the view Z rotation and the Z rotation spinbox in the UI
//    /// @param[in] _z The value to set.
//    void setRotationZ(double _z);
//    /// @brief Change both the sphere's X translation and the X translation spinbox in the UI
//    /// @param[in] _x The value to set.
//    void setTranslateX(double _x);
//    /// @brief Change both the sphere's Y translation and the Y translation spinbox in the UI
//    /// @param[in] _y The value to set.
//    void setTranslateY(double _y);
//    /// @brief Change both the sphere's Z translation and the Z translation spinbox in the UI
//    /// @param[in] _z The value to set.
//    void setTranslateZ(double _z);

private :
	/// @brief m_wireframe mode
	bool m_wireframe;
	/// @brief rotation data
  ngl::Vec3 m_rotation;
	/// @brief scale data
  ngl::Vec3 m_scale;
	/// @brief position data
  ngl::Vec3 m_position;
    /// @brief Which method to draw the cloth sheet.
    GLenum m_drawType;
    /// @brief Cloth construction info.
    CS::ClothInfo m_clothInfo;

    //----------------------------------------------------------------------------------------------------------------------
    // mouse movement stuff
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief whether we should interpret mouse movement as translation of the collision sphere
    bool m_shouldTranslateSphere;
    /// @brief whether we should interpret mouse movement as translation of the camera
    bool m_shouldTranslateView;
    /// @brief whether we should interpret mouse movement as rotation
    bool m_shouldRotate;
    /// @brief used to store the x rotation mouse value
    int m_spinXFace;
    /// @brief used to store the y rotation mouse value
    int m_spinYFace;
    /// @brief the previous x mouse value
    int m_origX;
    /// @brief the previous y mouse value
    int m_origY;
    /// @brief the previous x mouse value for Position changes
    int m_origXPos;
    /// @brief the previous y mouse value for Position changes
    int m_origYPos;
    /// @brief used to store the global mouse transforms
    ngl::Mat4 m_mouseGlobalTX;

    void resetCloth();

protected:

  /// @brief  The following methods must be implimented in the sub class
  /// this is called when the window is created
  void initializeGL();

  /// @brief this is called whenever the window is re-sized
  /// @param[in] _w the width of the resized window
  /// @param[in] _h the height of the resized window
  void resizeGL(const int _w, const int _h );
  /// @brief this is the main gl drawing routine which is called whenever the window needs to be re-drawn
  void paintGL();

	/// @brief our camera
	ngl::Camera *m_camera;
	/// @brief our transform for objects
	ngl::Transformation m_transform;
    /// @brief The separate transform just for the sphere; this is relative to the main transform.
    ngl::Transformation m_sphereTransform;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our cloth object.
    //----------------------------------------------------------------------------------------------------------------------
    Cloth m_cloth;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The time since the simulation began; pretty much just used for the cloth's wind function.
    //----------------------------------------------------------------------------------------------------------------------
    double m_time;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mesh data
    //----------------------------------------------------------------------------------------------------------------------
    ngl::VertexArrayObject *m_vao;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Loads the cloth texture from the file and creates the OpenGL texture buffers for the
    /// position and normal textures.
    //----------------------------------------------------------------------------------------------------------------------
    void createTextures();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Update the position texture with the new values loaded from the Cloth object.
    //----------------------------------------------------------------------------------------------------------------------
    void updatePositionTexture();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief texture name
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_clothTexture;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Handle of the OpenGL buffer storing the particle positions.
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_posBuffer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Handle of the OpenGL texture that stores the data from the position buffer.
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_posBufferTexture;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Handle of the OpenGL framebuffer we (should) write the normals to.
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_normalsFramebuffer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Handle of the OpenGL texture storing the vertex normals.
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_normalsFramebufferTexture;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A very simple VAO that fills the canonical view volume; used for rendering to the
    /// framebuffer.
    ngl::VertexArrayObject *m_viewportVAO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Render the vertex normals to the framebuffer (broken as of the writing of this brief)
    //----------------------------------------------------------------------------------------------------------------------
    void renderNormals();
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief create our mesh
    //----------------------------------------------------------------------------------------------------------------------
    void createVAO();
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Update the mesh with the new vertex positions.
    //----------------------------------------------------------------------------------------------------------------------
    void updateVAO();

protected:
    /// Overloaded function to handle keyboard input
    void timerEvent(QTimerEvent *e);

private :
  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure
  void mouseMoveEvent (QMouseEvent * _event   );

  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  void mousePressEvent ( QMouseEvent *_event  );

  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  void mouseReleaseEvent (QMouseEvent *_event );
  void loadMatricesToShader(ngl::Transformation &_transform, std::string _shaderName);

};

#endif
