#include <QMouseEvent>
#include <QCoreApplication>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "glwidget.h"
#include "shaderhelper.h"

GLWidget::GLWidget(QWidget *parent)
        : QOpenGLWidget(parent),
          m_maxCoord(1.0f),
          m_zoom(100.0f),
          m_xRotWorld(0),
          m_yRotWorld(0),
          m_zRotWorld(0),
          m_nearPlane(0.01f),
          m_farPlane(10.0f)
{
    m_progsMap = new QMap<Drawables, QOpenGLShaderProgram*>();
}

GLWidget::~GLWidget()
{
    cleanup();
}
QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}
static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}
void GLWidget::initializeGL()
{
    m_winWidth = this->width();
    m_winHeight = this->height();
    m_RotWorld = QMatrix4x4();
    m_RotScreen = QMatrix4x4();
    m_triadMvpMatrix = QMatrix4x4();
    m_viewportModMatrix = QMatrix4x4();
    m_viewportMvpMatrix = QMatrix4x4();
    this->buildTriadMvpMatrix();
    this->buildViewportMvpMatrix();
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
}
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    QVector<GLfloat> vertCoordsPoint;
    vertCoordsPoint.append(0.0f);
    vertCoordsPoint.append(0.0f);
    vertCoordsPoint.append(0.0f);
    QVector<GLfloat> vertCoordsLine;
    vertCoordsLine.append(0.0f);
    vertCoordsLine.append(0.0f);
    vertCoordsLine.append(0.0f);
    vertCoordsLine.append(0.5f);
    vertCoordsLine.append(0.0f);
    vertCoordsLine.append(0.0f);

    this->drawTriad();
//    this->drawPoint(vertCoordsPoint);
//    this->drawLine(vertCoordsLine);
}
QOpenGLShaderProgram* GLWidget::getShaderProgram(
        GLWidget::Drawables geomType,
        const char* vertPath,
        const char* geomPath,
        const char* fragPath)
{
    QOpenGLShaderProgram *program = 0;

    if (m_progsMap->contains(geomType)) {
        program = (*m_progsMap)[geomType];
    }
    else {
        program = new QOpenGLShaderProgram;
        m_progsMap->insert(geomType, program);

        std::vector<char> vertBufffer;
        const char* vertSource = ShaderHelper::readShaderSrc(
                vertPath, vertBufffer);

        std::vector<char> geomBufffer;
        const char* geomSource = ShaderHelper::readShaderSrc(
                geomPath, geomBufffer);

        std::vector<char> fragBufffer;
        const char* fragSource = ShaderHelper::readShaderSrc(
                fragPath, fragBufffer);

        program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertSource);
        program->addShaderFromSourceCode(QOpenGLShader::Geometry, geomSource);
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource);
    }
    return program;
}
void GLWidget::buildTriadMvpMatrix()
{
    QMatrix4x4 xRotScreen;
    QMatrix4x4 yRotScreen;
    QMatrix4x4 zRotScreen;
    QMatrix4x4 rotScreen;
    QMatrix4x4 rotWorld;
    QMatrix4x4 modMatrix;
    QMatrix4x4 camMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projMatrix;
    QVector3D xAxisWorld = QVector3D(1.0f, 0.0f, 0.0f);
    QVector3D yAxisWorld = QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D zAxisWorld = QVector3D(0.0f, 0.0f, 1.0f);
    QVector3D xAxisScreen;
    QVector3D yAxisScreen;
    QVector3D zAxisScreen;

    // BUILD MVP MATRIX
    // Model matrix

    rotScreen = m_RotScreen;
    xRotScreen.setToIdentity();
    xRotScreen.rotate((float)m_xRotScreen, xAxisWorld);
    yRotScreen.setToIdentity();
    yRotScreen.rotate((float)m_yRotScreen, yAxisWorld);
    zRotScreen.setToIdentity();
    zRotScreen.rotate((float)m_zRotScreen, zAxisWorld);
    rotScreen = zRotScreen * yRotScreen * xRotScreen * rotScreen;
    m_RotScreen = rotScreen;

    rotWorld = m_RotWorld;
    rotWorld.rotate((float)m_xRotWorld, xAxisWorld);
    rotWorld.rotate((float)m_yRotWorld, yAxisWorld);
    rotWorld.rotate((float)m_zRotWorld, zAxisWorld);
    m_RotWorld = rotWorld;

    modMatrix = rotScreen * rotWorld;

    // Camera matrix and View matrix
    this->setCameraZ();
    camMatrix.setToIdentity();
    QVector3D cameraEye = QVector3D(camMatrix *
            QVector4D(0.0f, 0.0f, 1.0f, 1.0f));
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(
     cameraEye,                     // eye
     QVector3D(0.0f, 0.0f, 0.0f),   // center
     QVector3D(0.0f, 1.0f, 0.0f) ); // up
    // Projection matrix
    projMatrix.setToIdentity();
    projMatrix.perspective(
     45.0f,                                       // angle
     (float)m_winWidth /(float)m_winHeight,       // aspect
     m_nearPlane,                                 // near plane
     m_farPlane);                                 // far plane

    // Multiply p*v*m
    m_triadMvpMatrix = projMatrix * viewMatrix * modMatrix;
}
QMatrix4x4 GLWidget::buildViewportMvpMatrix()
{
    QMatrix4x4 modMatrix;
    QMatrix4x4 camMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projMatrix;
    QMatrix4x4 mvpMatrix;

    // BUILD MVP MATRIX
    // Model matrix
    modMatrix.setToIdentity();
    QVector3D xAxis = QVector3D(1.0f, 0.0f, 0.0f);
    QVector3D yAxis = QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D zAxis = QVector3D(0.0f, 0.0f, 1.0f);
    modMatrix.rotate((float)m_xRotWorld, xAxis);
    modMatrix.rotate((float)m_yRotWorld, yAxis);
    modMatrix.rotate((float)m_zRotWorld, zAxis);
    // Camera matrix and View matrix
    camMatrix.setToIdentity();
    this->setCameraZ();
    QVector3D cameraEye = QVector3D(camMatrix *
        QVector4D(0.0f, 0.0f, m_cameraZ, 1.0f));
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(
        cameraEye,                     // eye
        QVector3D(0.0f, 0.0f, 0.0f),   // center
        QVector3D(0.0f, 1.0f, 0.0f) ); // up
    // Projection matrix
    projMatrix.setToIdentity();
    projMatrix.perspective(
        45.0f,                                       // angle
        (float)m_winWidth /(float)m_winHeight,       // aspect
        m_nearPlane,                                 // near plane
        m_farPlane);                                 // far plane
    // Multiply p*v*m
    mvpMatrix = projMatrix * viewMatrix * modMatrix;
    return mvpMatrix;
}
void GLWidget::draw(GLWidget::Drawables geomType,
                    QVector<GLfloat> vertCoords,
                    QColor colorIn)
{
    int vertCoordsBytes;
    GLfloat numVertices;
    const char* vertPath;
    const char* geomPath;
    const char* fragPath;
    QOpenGLShaderProgram *program = 0;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QMatrix4x4 viewportMvpMatrix;
    QMatrix4x4 mvpMatrix;
    int coordsLoc;
    int mvpMatrixLoc;
    int colorInLoc;
    int drawingMode;

    viewportMvpMatrix = this->buildViewportMvpMatrix();

    if(geomType == GLWidget::Drawables::TRIAD_ARROW)
    {
        // Paths to shaders
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomVertices.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = m_triadMvpMatrix;
        drawingMode = GL_POINTS;
    }
    else if(geomType == GLWidget::Drawables::TRIAD_AXIS)
    {
        // Paths to shaders
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomLines.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = m_triadMvpMatrix;
        drawingMode = GL_LINES;
    }
    else if(geomType == GLWidget::Drawables::POINT)
    {
        // Paths to shaders
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomVertices.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = viewportMvpMatrix;
        drawingMode = GL_POINTS;
    }
    else if(geomType == GLWidget::Drawables::LINE)
    {
        // Paths to shaders
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomLines.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = viewportMvpMatrix;
        drawingMode = GL_LINES;
    }
    vertCoordsBytes = vertCoords.size() * sizeof(GLfloat);
    numVertices = vertCoords.size() / 3;
    // Create, link and bind shader program
    program = this->getShaderProgram(geomType,
            vertPath,
            geomPath,
            fragPath);
    program->link();
    program->bind();
    coordsLoc = program->attributeLocation("coords");
    mvpMatrixLoc = program->uniformLocation("mvpMatrix");
    colorInLoc = program->uniformLocation("colorIn");
    // Create a vertex array object. Really needed only when using
    // several vbos together
    vao.create();
    vao.bind();
    // Setup our vertex buffer object.
    vbo.create();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.bind();
    vbo.allocate(vertCoordsBytes);
    vbo.write(0, vertCoords.data(), vertCoordsBytes);
    vbo.release();
    vbo.bind();
    // Store the vertex attribute bindings for the program.
    program->setAttributeBuffer(coordsLoc, GL_FLOAT, 0, 3, 0);
    program->enableAttributeArray(coordsLoc);
    vbo.release();
    program->release();
    vao.release();
    program->bind();
    program->setUniformValue(mvpMatrixLoc, mvpMatrix);
    program->setUniformValue(colorInLoc, colorIn);
    vao.bind();
    glDrawArrays(drawingMode, 0, numVertices);
    program->release();
    vao.release();
    program->release();
    vbo.destroy();
}
void GLWidget::drawPoint(QVector<GLfloat> vertCoords, QColor colorIn)
{
    this->draw(GLWidget::Drawables::POINT, vertCoords, colorIn);
}
void GLWidget::drawLine(QVector<GLfloat> vertCoords, QColor colorIn)
{
    this->draw(GLWidget::Drawables::LINE, vertCoords, colorIn);
}
void GLWidget::drawTriad()
{
    QVector<GLfloat> xAxisCoords;
    xAxisCoords.append(0.0f);
    xAxisCoords.append(0.0f);
    xAxisCoords.append(0.0f);
    xAxisCoords.append(0.07f);
    xAxisCoords.append(0.0f);
    xAxisCoords.append(0.0f);
    QVector<GLfloat> yAxisCoords;
    yAxisCoords.append(0.0f);
    yAxisCoords.append(0.0f);
    yAxisCoords.append(0.0f);
    yAxisCoords.append(0.0f);
    yAxisCoords.append(0.07f);
    yAxisCoords.append(0.0f);
    QVector<GLfloat> zAxisCoords;
    zAxisCoords.append(0.0f);
    zAxisCoords.append(0.0f);
    zAxisCoords.append(0.0f);
    zAxisCoords.append(0.0f);
    zAxisCoords.append(0.0f);
    zAxisCoords.append(0.07f);
    this->draw(GLWidget::Drawables::TRIAD_AXIS, xAxisCoords, QColor("Red"));
    this->draw(GLWidget::Drawables::TRIAD_AXIS, yAxisCoords, QColor("Green"));
    this->draw(GLWidget::Drawables::TRIAD_AXIS, zAxisCoords, QColor("Blue"));
    QVector<GLfloat> xArrowCoords;
    xArrowCoords.append(0.07f);
    xArrowCoords.append(0.0f);
    xArrowCoords.append(0.0f);
    QVector<GLfloat> yArrowCoords;
    yArrowCoords.append(0.0f);
    yArrowCoords.append(0.07f);
    yArrowCoords.append(0.0f);
    QVector<GLfloat> zArrowCoords;
    zArrowCoords.append(0.0f);
    zArrowCoords.append(0.0f);
    zArrowCoords.append(0.07f);
    this->draw(GLWidget::Drawables::TRIAD_ARROW, xArrowCoords, QColor("Red"));
    this->draw(GLWidget::Drawables::TRIAD_ARROW, yArrowCoords, QColor("Green"));
    this->draw(GLWidget::Drawables::TRIAD_ARROW, zArrowCoords, QColor("Blue"));
}
void GLWidget::mousePressEvent(QMouseEvent *event) {
    m_lastMousePos = event->pos();
}
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - m_lastMousePos.x();
    int dy = event->y() - m_lastMousePos.y();
    QFlags<Qt::KeyboardModifiers> ctrl_alt = QFlags<Qt::KeyboardModifiers>(
            Qt::ControlModifier|Qt::AltModifier);
    QFlags<Qt::KeyboardModifiers> shift_ctrl_alt =
            QFlags<Qt::KeyboardModifiers>(
            Qt::ShiftModifier|Qt::ControlModifier|Qt::AltModifier);

    if (QGuiApplication::keyboardModifiers() == ctrl_alt) {
        if (event->buttons() & Qt::LeftButton) {
            setXRotationScreen(dy);
            setYRotationScreen(dx);
        } else if (event->buttons() & Qt::RightButton) {
            setXRotationScreen(dy);
            setZRotationScreen(dx);
        }
        this->buildTriadMvpMatrix();
        this->buildViewportMvpMatrix();
        m_xRotScreen = 0;
        m_yRotScreen = 0;
        m_zRotScreen = 0;
    } else if (QGuiApplication::keyboardModifiers() == shift_ctrl_alt) {
        if (event->buttons() & Qt::LeftButton) {
            setXRotationWorld(dy);
            setYRotationWorld(dx);
        } else if (event->buttons() & Qt::RightButton) {
            setXRotationWorld(dy);
            setZRotationWorld(dx);
        }
        this->buildTriadMvpMatrix();
        this->buildViewportMvpMatrix();
        m_xRotWorld = 0;
        m_yRotWorld = 0;
        m_zRotWorld = 0;
    }
    m_lastMousePos = event->pos();
    this->update();
}
void GLWidget::wheelEvent(QWheelEvent *event) {
    float numPixels = (float)event->pixelDelta().y();
    float numDegrees = (float)event->angleDelta().y() / 120.0f;
    float cameraZ;
    float eps = 1E-03f;
    if (!event->pixelDelta().isNull()) {
        cameraZ = m_zoom / 100.0f;
        if (cameraZ > m_nearPlane) {
            m_zoom -= numPixels;
        } else {
            while (!(cameraZ > m_nearPlane)) {
                if (numPixels > eps) {
                    numPixels /= 100.0f;
                } else {
                     numPixels = 0.0f;
                     break;
                }
            }
            m_zoom -= numPixels;
        }
    } else if (!event->angleDelta().isNull()) {
      float numSteps = numDegrees;
      cameraZ = m_zoom / 100.0f;
      if ((cameraZ > m_nearPlane) ||
          ((cameraZ == m_nearPlane) && (numSteps > 0.0f))) {
          m_zoom -= numSteps;
      }
      else {
        numSteps = 0.0f;
      }
    }
    this->buildTriadMvpMatrix();
    event->accept();
    this->update();
}
void GLWidget::setCameraZ()
{
    m_cameraZ = 100.0f * m_maxCoord / m_zoom;
}
void GLWidget::setXRotationScreen(int angle)
{
    qNormalizeAngle(angle);
    m_xRotScreen = angle;
}
void GLWidget::setYRotationScreen(int angle)
{
    qNormalizeAngle(angle);
    m_yRotScreen = angle;
}
void GLWidget::setZRotationScreen(int angle)
{
    qNormalizeAngle(angle);
    m_zRotScreen = angle;
}
void GLWidget::setXRotationWorld(int angle)
{
    qNormalizeAngle(angle);
    m_xRotWorld = angle;
}
void GLWidget::setYRotationWorld(int angle)
{
    qNormalizeAngle(angle);
    m_yRotWorld = angle;
}
void GLWidget::setZRotationWorld(int angle)
{
    qNormalizeAngle(angle);
    m_zRotWorld = angle;
}
void GLWidget::cleanup()
{
    makeCurrent();
    QMap<GLWidget::Drawables, QOpenGLShaderProgram*>::
            key_iterator key = m_progsMap->keyBegin();
    while (key != m_progsMap->keyEnd()) {
        delete m_progsMap->value(*key);
        m_progsMap->remove(*key);
        ++key;
    }
    delete m_progsMap;
    doneCurrent();
}
