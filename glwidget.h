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
        TRIAD_ORIGO,
        TRIAD_ARROW,
        TRIAD_AXIS,
        POINT,
        LINE
    };
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void setXTranslation(float deltaX);
    void setYTranslation(float deltaY);
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    float getCameraZ(float maxCoord, float zoom);

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
    void updateMatrices();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    int m_winWidth;
    int m_winHeight;
    float m_maxCoord;
    float m_zoom;
    float m_cameraZ;
    float m_xTrans;
    float m_yTrans;
    QVector3D m_rotCenter;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    float m_nearPlane;
    float m_farPlane;
    QMatrix4x4 m_triadModMatrix;
    QMatrix4x4 m_viewportModMatrix;
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
    QMatrix4x4 buildTriadMvpMatrix();
};

#endif
