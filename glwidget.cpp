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
          m_xTrans(0.0f),
          m_yTrans(0.0f),
          m_rotCenter(QVector3D(0.0f, 0.0f, 0.0f)),
          m_xRot(0),
          m_yRot(0),
          m_zRot(0),
          m_nearPlane(0.01f),
          m_farPlane(100.0f)
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

    m_triadModMatrix.setToIdentity();
    m_viewportModMatrix.setToIdentity();
    this->updateMatrices();
    initializeOpenGLFunctions();
    glClearColor(0.2, 0.2, 0.2, 1);
}
void GLWidget::updateMatrices() {
    this->buildTriadMvpMatrix();
    this->buildViewportMvpMatrix();
}
void GLWidget::paintGL()
{
    glClearDepthf(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GEQUAL);
    glEnable(GL_CULL_FACE);

    QVector<GLfloat> vertCoordsPoint1;
    vertCoordsPoint1.append(0.0f);
    vertCoordsPoint1.append(0.0f);
    vertCoordsPoint1.append(0.0f);
    QVector<GLfloat> vertCoordsPoint2;
    vertCoordsPoint2.append(0.5f);
    vertCoordsPoint2.append(0.0f);
    vertCoordsPoint2.append(0.0f);
    QVector<GLfloat> vertCoordsPoint3;
    vertCoordsPoint3.append(0.0f);
    vertCoordsPoint3.append(0.5f);
    vertCoordsPoint3.append(0.0f);
    QVector<GLfloat> vertCoordsPoint4;
    vertCoordsPoint4.append(0.0f);
    vertCoordsPoint4.append(0.0f);
    vertCoordsPoint4.append(0.5f);
    QVector<GLfloat> vertCoordsLine1;
    vertCoordsLine1.append(0.0f);
    vertCoordsLine1.append(0.0f);
    vertCoordsLine1.append(0.0f);
    vertCoordsLine1.append(0.5f);
    vertCoordsLine1.append(0.0f);
    vertCoordsLine1.append(0.0f);
    QVector<GLfloat> vertCoordsLine2;
    vertCoordsLine2.append(0.0f);
    vertCoordsLine2.append(0.0f);
    vertCoordsLine2.append(0.0f);
    vertCoordsLine2.append(0.0f);
    vertCoordsLine2.append(0.5f);
    vertCoordsLine2.append(0.0f);
    QVector<GLfloat> vertCoordsLine3;
    vertCoordsLine3.append(0.0f);
    vertCoordsLine3.append(0.0f);
    vertCoordsLine3.append(0.0f);
    vertCoordsLine3.append(0.0f);
    vertCoordsLine3.append(0.0f);
    vertCoordsLine3.append(0.5f);

    this->drawTriad();
    this->drawPoint(vertCoordsPoint1);
    this->drawPoint(vertCoordsPoint2);
    this->drawPoint(vertCoordsPoint3);
    this->drawPoint(vertCoordsPoint4);
    this->drawLine(vertCoordsLine1);
    this->drawLine(vertCoordsLine2);
    this->drawLine(vertCoordsLine3);
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
QMatrix4x4 GLWidget::buildTriadMvpMatrix()
{
    QMatrix4x4 xRotScreen;
    QMatrix4x4 yRotScreen;
    QMatrix4x4 zRotScreen;
    QMatrix4x4 rotScreen;
    QMatrix4x4 transScreen;
    QMatrix4x4 camMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projMatrix;
    QMatrix4x4 mvpMatrix;
    QVector3D xAxis = QVector3D(1.0f, 0.0f, 0.0f);
    QVector3D yAxis = QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D zAxis = QVector3D(0.0f, 0.0f, 1.0f);

    // BUILD MVP MATRIX
    // Model matrix
    xRotScreen.setToIdentity();
    xRotScreen.rotate((float)m_xRot, xAxis);
    yRotScreen.setToIdentity();
    yRotScreen.rotate((float)m_yRot, yAxis);
    zRotScreen.setToIdentity();
    zRotScreen.rotate((float)m_zRot, zAxis);
    rotScreen = zRotScreen * yRotScreen * xRotScreen * rotScreen;
    m_triadModMatrix = rotScreen * m_triadModMatrix;
    // Camera matrix and View matrix
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
        projMatrix.ortho(-1.0f, 1.0f, -1.0f, 1.0f, m_nearPlane, m_farPlane);
    // Move to viewport corner
    transScreen.setToIdentity();
    transScreen.translate(-0.87f, -0.75f, 0.0f);
    // Multiply p*v*m
    mvpMatrix = transScreen * projMatrix * viewMatrix * m_triadModMatrix;
    return mvpMatrix;
}
QMatrix4x4 GLWidget::buildViewportMvpMatrix()
{
    QMatrix4x4 xRotMat;
    QMatrix4x4 yRotMat;
    QMatrix4x4 zRotMat;
    QMatrix4x4 xTransMat;
    QMatrix4x4 yTransMat;
    QMatrix4x4 transMat;
    QMatrix4x4 rotMat;
    QMatrix4x4 camMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projMatrix;
    QMatrix4x4 mvpMatrix;
    QVector3D xAxis = QVector3D(1.0f, 0.0f, 0.0f);
    QVector3D yAxis = QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D zAxis = QVector3D(0.0f, 0.0f, 1.0f);

    // BUILD MVP MATRIX
    // Model matrix
    xTransMat.setToIdentity();
    xTransMat.translate(m_xTrans * xAxis);
    yTransMat.setToIdentity();
    yTransMat.translate(m_yTrans * yAxis);
    transMat = xTransMat * yTransMat;
    m_viewportModMatrix = transMat * m_viewportModMatrix;
    xRotMat.setToIdentity();
    xRotMat.rotate((float)m_xRot, xAxis);
    yRotMat.setToIdentity();
    yRotMat.rotate((float)m_yRot, yAxis);
    zRotMat.setToIdentity();
    zRotMat.rotate((float)m_zRot, zAxis);
    rotMat = zRotMat * yRotMat * xRotMat;
    m_viewportModMatrix = rotMat * m_viewportModMatrix;
    // Camera matrix and View matrix
    camMatrix.setToIdentity();
    float cameraZ = this->getCameraZ(m_maxCoord, m_zoom);
    QVector3D cameraEye = QVector3D(camMatrix *
            QVector4D(0.0f, 0.0f, cameraZ, 1.0f));
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
    mvpMatrix = projMatrix * viewMatrix * m_viewportModMatrix;
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
    QMatrix4x4 triadMvpMatrix;
    QMatrix4x4 viewportMvpMatrix;
    QMatrix4x4 mvpMatrix;
    int coordsLoc;
    int mvpMatrixLoc;
    int colorInLoc;
    int drawingMode;

    triadMvpMatrix = this->buildTriadMvpMatrix();
    viewportMvpMatrix = this->buildViewportMvpMatrix();
    if((geomType == GLWidget::Drawables::TRIAD_ORIGO) ||
       (geomType == GLWidget::Drawables::TRIAD_ARROW))
    {
        // Parameters
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomVertices.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = triadMvpMatrix;
        drawingMode = GL_POINTS;
    }
    else if(geomType == GLWidget::Drawables::TRIAD_AXIS)
    {
        // Parameters
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomLines.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = triadMvpMatrix;
        drawingMode = GL_LINES;
    }
    else if(geomType == GLWidget::Drawables::POINT)
    {
        // Parameters
        vertPath = ":/shaders/shaders/vert.shader";
        geomPath = ":/shaders/shaders/geomVertices.shader";
        fragPath = ":/shaders/shaders/frag.shader";
        mvpMatrix = viewportMvpMatrix;
        drawingMode = GL_POINTS;
    }
    else if(geomType == GLWidget::Drawables::LINE)
    {
        // Parameters
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
    QVector<GLfloat> origoCoords;
    origoCoords.append(0.0f);
    origoCoords.append(0.0f);
    origoCoords.append(0.0f);
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
    this->draw(GLWidget::Drawables::TRIAD_ORIGO, origoCoords, QColor("White"));
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

    if (QGuiApplication::keyboardModifiers() == ctrl_alt) {
        if (event->buttons() & Qt::LeftButton) {
            setXRotation(dy);
            setYRotation(dx);
        }
        else if (event->buttons() & Qt::RightButton) {
            setXRotation(dy);
            setZRotation(dx);
        }
        else if (event->buttons() & Qt::MiddleButton) {
            setXTranslation((float)dx);
            setYTranslation((float)dy);
        }
        this->updateMatrices();
        setXTranslation(0.0f);
        setYTranslation(0.0f);
        setXRotation(0);
        setYRotation(0);
        setZRotation(0);
    }
    m_lastMousePos = event->pos();
    this->update();
}
void GLWidget::wheelEvent(QWheelEvent *event)
{
    float numPixels = -(float)event->pixelDelta().y();
    float numDegrees = -(float)event->angleDelta().y() / 120.0f;
    float numSteps;
    float attemptCameraZ;

    if (!event->pixelDelta().isNull()) {
        numSteps = numPixels;
        attemptCameraZ = this->getCameraZ(m_maxCoord, m_zoom + numSteps);
        if ((attemptCameraZ >= m_nearPlane) &&
           (attemptCameraZ <= m_farPlane)) {
          m_zoom += numSteps;
        }
        else {
          numPixels = 0.0f;
        }
    }
    else if (!event->angleDelta().isNull()) {
        numSteps = numDegrees;
        attemptCameraZ = this->getCameraZ(m_maxCoord, m_zoom + numSteps);
        if ((attemptCameraZ >= m_nearPlane) &&
           (attemptCameraZ <= m_farPlane)) {
          m_zoom += numSteps;
        }
        else {
        numSteps = 0.0f;
        }
    }
    this->updateMatrices();
    event->accept();
    this->update();
}
float GLWidget::getCameraZ(float maxCoord, float zoom)
{
    float cameraZ = 100.0f * maxCoord / zoom;
    return cameraZ;
}
void GLWidget::setXTranslation(float deltaX)
{
    m_xTrans = deltaX / 50.0f;
}
void GLWidget::setYTranslation(float deltaY)
{
    m_yTrans = -deltaY / 50.0f;
}
void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    m_xRot = angle;
}
void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    m_yRot = angle;
}
void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    m_zRot = angle;
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
