#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);
  // create an openGL format and pass to the new GLWidget
  QGLFormat format;
  format.setVersion(4,1);
  format.setProfile( QGLFormat::CoreProfile);

  m_gl= new GLWindow(format,this);

  m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);
  connect(m_ui->m_wireframe,SIGNAL(toggled(bool)),m_gl,SLOT(toggleWireframe(bool)));

  /// set the combo box index change signal
  connect(m_ui->m_drawType,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(setDrawType(int)));

  connect(m_ui->m_paused,SIGNAL(clicked(bool)),m_gl,SLOT(togglePaused()));
  connect(m_ui->m_applyWind,SIGNAL(clicked(bool)),m_gl,SLOT(toggleWind()));
  connect(m_ui->m_applySphereCollision,SIGNAL(clicked(bool)),m_gl,SLOT(setSphereCollisions(bool)));
  connect(m_ui->m_applySelfCollision,SIGNAL(clicked(bool)),m_gl,SLOT(setSelfCollisions(bool)));

  connect(m_ui->m_anchorBottomLeft,SIGNAL(clicked(bool)),m_gl,SLOT(setAnchoredBottomLeft(bool)));
  connect(m_ui->m_anchorBottomRight,SIGNAL(clicked(bool)),m_gl,SLOT(setAnchoredBottomRight(bool)));
  connect(m_ui->m_anchorTopLeft,SIGNAL(clicked(bool)),m_gl,SLOT(setAnchoredTopLeft(bool)));
  connect(m_ui->m_anchorTopRight,SIGNAL(clicked(bool)),m_gl,SLOT(setAnchoredTopRight(bool)));

  connect(m_ui->m_clothHeight,SIGNAL(valueChanged(double)),m_gl,SLOT(setClothHeight(double)));
  connect(m_ui->m_clothWidth,SIGNAL(valueChanged(double)),m_gl,SLOT(setClothWidth(double)));
  connect(m_ui->m_clothHeightRes,SIGNAL(valueChanged(int)),m_gl,SLOT(setClothHeightRes(int)));
  connect(m_ui->m_clothWidthRes,SIGNAL(valueChanged(int)),m_gl,SLOT(setClothWidthRes(int)));
  connect(m_ui->m_springConstant,SIGNAL(valueChanged(double)),m_gl,SLOT(setSpringConstant(double)));
  connect(m_ui->m_dampingConstant,SIGNAL(valueChanged(double)),m_gl,SLOT(setDampingConstant(double)));
  connect(m_ui->m_gravity, SIGNAL(valueChanged(double)),m_gl,SLOT(setGravity(double)));
  connect(m_ui->m_simSpeed, SIGNAL(valueChanged(double)),m_gl,SLOT(setSimSpeed(double)));

  connect(m_ui->m_resetButton,SIGNAL(clicked()),m_gl,SLOT(reset()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
