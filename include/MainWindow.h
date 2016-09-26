#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GLWindow.h"

namespace Ui {
    class MainWindow;
}

/// @file MainWindow.h
/// @brief Source file for the window launched by the application.
/// @author Jonathan Macey (modified by Robert Poncelet)
/// @version 1.0
/// @date 21/03/15
/// @class MainWindow
/// @brief The main interface that contains the simulation and the UI and links them together.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// @brief Constructor for MainWindow.
    /// @param[in] parent The parent window to create the MainWindow in - usually null, of course.
    explicit MainWindow(QWidget *parent = 0);

    /// @brief Destructor for MainWindow.
    ~MainWindow();

private:

    /// @brief The UI to display around the simulation.
    Ui::MainWindow *m_ui;

    /// @brief The GLWindow running the simulation.
    GLWindow *m_gl;
};

#endif // MAINWINDOW_H
