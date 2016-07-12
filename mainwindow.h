#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *m_ui;
    GLWidget *m_glWidget;
    bool checkInputCoords(QString input);
    void getCoordsFromUser(float* &returnArray);

public slots:
    void onCreatePoint();
};

#endif // MAINWINDOW_H
