#include <QInputDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "geometry.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        m_ui(new Ui::MainWindow)
{
    this->m_ui->setupUi(this);
    this->m_glWidget = this->m_ui->openGLWidget;
    connect(this->m_ui->actionCreate_Point, SIGNAL(triggered()), this, SLOT(onCreatePoint()));
}
MainWindow::~MainWindow() {
    delete this->m_ui;
}
bool MainWindow::checkInputCoords(QString input) {
    return true;
}
void MainWindow::onCreatePoint() {
    bool ok;
    QString text = QInputDialog::getText(this, "Create Point",
            "Insert coordinates x,y,z:\n"
            "(separate with commas, don't use spaces)", QLineEdit::Normal,
            "", &ok);
    if (ok) {
        bool inputIsValid = this->checkInputCoords(text);
        if(inputIsValid) {
            QStringList coordsList = text.split(QChar(','));
            float x = coordsList[0].toFloat();
            float y = coordsList[1].toFloat();
            float z = coordsList[2].toFloat();
            Geometry::Point* p = new Geometry::Point(x,y,z);
            qDebug()<<"hierarchy: "<<p->getHierarchy();
        }
    }
    else {
        return;
    }
}
