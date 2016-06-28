#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGuiApplication>
#include <QMap>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class GLWidget : public QOpenGLWidget,
        protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    enum Drawables {
        TRIAD_ARROW,
        TRIAD_AXIS,
        POINT,
        LINE
    };
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void setXRotationScreen(int angle);
    void setYRotationScreen(int angle);
    void setZRotationScreen(int angle);
    void setXRotationWorld(int angle);
    void setYRotationWorld(int angle);
    void setZRotationWorld(int angle);
    void setCameraZ();

    void cleanup();
    void drawPoint(QVector<GLfloat> vertCoords,
            QColor colorIn = QColor("LightSkyBlue"));
    void drawLine(QVector<GLfloat> vertCoords,
            QColor colorIn = QColor("LightSkyBlue"));

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    int m_winWidth;
    int m_winHeight;
    float m_maxCoord;
    float m_zoom;
    float m_cameraZ;
    int m_xRotScreen;
    int m_yRotScreen;
    int m_zRotScreen;
    int m_xRotWorld;
    int m_yRotWorld;
    int m_zRotWorld;
    float m_nearPlane;
    float m_farPlane;
    QMatrix4x4 m_RotWorld;
    QMatrix4x4 m_RotScreen;
    QMatrix4x4 m_triadMvpMatrix;
    QMatrix4x4 m_viewportModMatrix;
    QMatrix4x4 m_viewportMvpMatrix;
    QMap<Drawables, QOpenGLShaderProgram*> *m_progsMap;
    QPoint m_lastMousePos;

    QOpenGLShaderProgram* getShaderProgram(Drawables geomType,
            const char* vertPath,
            const char* geomPath,
            const char* fragPath);
    void draw(Drawables geomType, QVector<GLfloat> vertCoords,
              QColor colorIn = QColor("LightSkyBlue"));
    void drawTriad();
    QMatrix4x4 buildViewportMvpMatrix();
    void buildTriadMvpMatrix();
};

#endif
