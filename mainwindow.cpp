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
    connect(this->m_ui->actionCreate_Point,
            SIGNAL(triggered()), this, SLOT(onCreatePoint()));
}
MainWindow::~MainWindow() {
    delete this->m_ui;
}
bool MainWindow::checkInputCoords(QString input) {
    return true;
}
void MainWindow::getCoordsFromUser(QVector<GLfloat> &vertCoordsPoint) {
    bool ok;
    QString text = QInputDialog::getText(this, "Create Point",
            "Insert coordinates x,y,z:\n"
            "(separate with commas, don't use spaces)", QLineEdit::Normal,
            "", &ok);
    if (ok) {
        bool inputIsValid = this->checkInputCoords(text);
        if(inputIsValid) {
            QStringList coordsList = text.split(QChar(','));

            qDebug()<<"coordsList: "<<coordsList;

            vertCoordsPoint.append(coordsList[0].toFloat());
            vertCoordsPoint.append(coordsList[1].toFloat());
            vertCoordsPoint.append(coordsList[2].toFloat());

            qDebug()<<"vertCoordsPoint: "<<vertCoordsPoint;
        }
        else {
            qDebug()<<"Error: Bad input received for point creation";
            vertCoordsPoint = QVector<GLfloat>(3);
        }
    }
    else {
        vertCoordsPoint = QVector<GLfloat>(3);
    }
}
void MainWindow::onCreatePoint() {
    QVector<GLfloat> vertCoordsPoint = QVector<GLfloat>(0);

    this->getCoordsFromUser(vertCoordsPoint);
    if (vertCoordsPoint.size()) {
        Geometry::Point* p = new Geometry::Point(vertCoordsPoint.data()[0], vertCoordsPoint.data()[1], vertCoordsPoint.data()[2]);
        qDebug()<<"hierarchy: "<<p->getHierarchy();
        qDebug()<<"name: "<<p->getName();
    }
}
