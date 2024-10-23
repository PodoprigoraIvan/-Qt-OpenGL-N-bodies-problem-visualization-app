#include "drawwidget.h"
#include <QMouseEvent>
#include <QEvent>
#include <qmainwindow.h>

DrawWidget::DrawWidget(QWidget *parent) : QOpenGLWidget(parent)
    , elapsedTime(0)
    , deltaTime(0)
    , mouseCaptured(false)
    , camera(QVector3D(0.0f, 0.0f, 10.0f), QVector3D(0.0f, 1.0f, 0.0f))
    , sphereInfos(nullptr)
    , isTracking(false) {
    setMinimumHeight(400);
    setMinimumWidth(400);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);
}

void DrawWidget::initializeGL() {
    initializeOpenGLFunctions();
    // Sphere VAO and VBO
    glGenVertexArrays(1, &VAO_sphere);
    glGenBuffers(1, &VBO_sphere);
    glGenVertexArrays(1, &VAO_line);
    glGenBuffers(1, &VBO_line);

    glBindVertexArray(VAO_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    int segmentsSz = 16;
    int vertexCount = (segmentsSz + 1) * (segmentsSz + 1);
    int indexCount = segmentsSz * segmentsSz * 6;

    float * vertices = new float[vertexCount * 3];
    indices_sphere = new unsigned int[indexCount];

    int v = 0;
    int i = 0;

    for (int y = 0; y <= segmentsSz; y++) {
        for (int x = 0; x <= segmentsSz; x++) {
            float xSegment = (float)x / (float)segmentsSz;
            float ySegment = (float)y / (float)segmentsSz;
            float xPos = cos(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
            float yPos = cos(ySegment * M_PI);
            float zPos = sin(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);

            vertices[v++] = xPos;
            vertices[v++] = yPos;
            vertices[v++] = zPos;
        }
    }

    for (int y = 0; y < segmentsSz; ++y) {
        for (int x = 0; x < segmentsSz; ++x) {
            indices_sphere[i++] = y * (segmentsSz + 1) + x;
            indices_sphere[i++] = (y + 1) * (segmentsSz + 1) + x;
            indices_sphere[i++] = y * (segmentsSz + 1) + x + 1;

            indices_sphere[i++] = y * (segmentsSz + 1) + x + 1;
            indices_sphere[i++] = (y + 1) * (segmentsSz + 1) + x;
            indices_sphere[i++] = (y + 1) * (segmentsSz + 1) + x + 1;
        }
    }

    indices_sphere_size = i;
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    delete [] vertices;

    glBindVertexArray(VAO_line);
    vertices = new float[6]{
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f
    };

    indices_line = new unsigned int[2]{0,1};

    glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    delete [] vertices;

    QMatrix4x4 matrix;
    matrix.setToIdentity();

    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/SimpleColor.vert");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/SimpleColor.frag");
    shaderProgram.link();
    shaderProgram.bind();

    GLint matrixLocation = shaderProgram.uniformLocation("model");
    shaderProgram.setUniformValue(matrixLocation, matrix);
    matrixLocation = shaderProgram.uniformLocation("view");
    shaderProgram.setUniformValue(matrixLocation, matrix);
    matrixLocation = shaderProgram.uniformLocation("projection");
    shaderProgram.setUniformValue(matrixLocation, matrix);

    glLineWidth(1.0);
}

void DrawWidget::paintGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram.bind();

    QVector3D colorVec(1.0, sin(elapsedTime/600.0), 0.0);
    if (mouseCaptured) colorVec = QVector3D(0.0, 1.0, 0.0);
    GLint colorVecLocation = shaderProgram.uniformLocation("aColor");
    shaderProgram.setUniformValue(colorVecLocation, colorVec);

    //auto radians = [](int angle){return (angle * M_PI) / 180.0;};
    QMatrix4x4 projection;
    projection.perspective(camera.Fov, (float)width() / (float)height(), 0.1f, 1000.0f);

    if (isTracking){
        for (int i = 0; i < sphereInfos->size(); i++){
            const SphereInfo * sphere = &(*sphereInfos)[i];
            if (sphere->getID() == trackingSphereID){
                camera.Position = sphere->getPos() - (camera.Front * sphere->r * 8.0);
                break;
            }
        }
    }

    QMatrix4x4 view = camera.GetViewMatrix();
    QMatrix4x4 model;
    model.setToIdentity();

    GLint matrixLocation = shaderProgram.uniformLocation("model");
    shaderProgram.setUniformValue(matrixLocation, model);
    matrixLocation = shaderProgram.uniformLocation("view");
    shaderProgram.setUniformValue(matrixLocation, view);
    matrixLocation = shaderProgram.uniformLocation("projection");
    shaderProgram.setUniformValue(matrixLocation, projection);

    // X, Y, Z AXIS
    drawLine(QVector3D(0.0,0.0,0.0), QVector3D(1.0,0.0,0.0), QVector3D(1.0,0.0,0.0), 3.0);
    drawLine(QVector3D(0.0,0.0,0.0), QVector3D(0.0,1.0,0.0), QVector3D(0.0,1.0,0.0), 3.0);
    drawLine(QVector3D(0.0,0.0,0.0), QVector3D(0.0,0.0,1.0), QVector3D(0.0,0.0,1.0), 3.0);

    if (sphereInfos != nullptr){
        for (int i = 0; i < sphereInfos->size(); i++){
            const SphereInfo * sphere = &(*sphereInfos)[i];
            drawSphere(sphere);
        }
    }
}

void DrawWidget::setSphereInfos(const std::vector<SphereInfo> * sphereInfos){
    this->sphereInfos = sphereInfos;
}

void DrawWidget::drawSphere(const SphereInfo* sphere){
    QMatrix4x4 model;
    glLineWidth(1.0);
    model.setToIdentity();
    model.translate(sphere->getPos());
    model.scale(sphere->r);

    setModelMatrix(model);

    GLint colorVecLocation = shaderProgram.uniformLocation("aColor");
    shaderProgram.setUniformValue(colorVecLocation, sphere->color);

    glBindVertexArray(VAO_sphere);
    glDrawElements(GL_LINES, indices_sphere_size, GL_UNSIGNED_INT, indices_sphere);

    std::deque<QVector3D> const & trace = sphere->getTraceVertices();
    glBindVertexArray(VAO_line);
    if (trace.size() >= 2){
        for (int i = 0; i < trace.size()-1; i++){
            drawLine(trace[i], trace[i+1], sphere->color, 1.0);
        }
    }
}

void DrawWidget::drawLine(const QVector3D& firstVertex, const QVector3D& secondVertex, const QVector3D& color, const float& lineWidth){
    GLint colorVecLocation = shaderProgram.uniformLocation("aColor");
    shaderProgram.setUniformValue(colorVecLocation, color);
    glLineWidth(lineWidth);
    QMatrix4x4 model;
    QVector3D scaleVec = secondVertex - firstVertex;
    model.setToIdentity();
    model.translate(firstVertex);
    model.scale(scaleVec);
    setModelMatrix(model);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, indices_line);
}

QVector3D DrawWidget::getCameraPos(){
    return camera.Position;
}

QVector3D DrawWidget::getCameraDirection(){
    return camera.Front;
}

void DrawWidget::mousePressEvent(QMouseEvent *event){
    if (mouseCaptured == false){
        mouseCaptured = true;
        setCursor(Qt::BlankCursor);
        QPoint globalPos = mapToGlobal(pos());
        cursor().setPos(globalPos.x()+(width()/2), globalPos.y()+(height()/2));
    } else {
        mouseCaptured = false;
        unsetCursor();
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event){
    if (mouseCaptured == true){
        QPoint globalPos = mapToGlobal(pos());
        QPoint curPos = cursor().pos();
        QPoint centerPos = QPoint(globalPos.x()+(width()/2), globalPos.y()+(height()/2));
        float xOffset = curPos.x() - centerPos.x();
        float yOffset = centerPos.y() - curPos.y();
        camera.ProcessMouseMovement(xOffset, yOffset);
        cursor().setPos(centerPos);
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

void DrawWidget::keyPressEvent(QKeyEvent *event){
    if (mouseCaptured == true){
        int key=event->nativeVirtualKey();
        if (key == Qt::Key_W)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (key == Qt::Key_S)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (key == Qt::Key_A)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (key == Qt::Key_D)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void DrawWidget::setModelMatrix(QMatrix4x4 matrix){
    GLint matrixLocation = shaderProgram.uniformLocation("model");
    shaderProgram.setUniformValue(matrixLocation, matrix);
}

void DrawWidget::setColor(QVector3D color){
    GLint matrixLocation = shaderProgram.uniformLocation("aColor");
    shaderProgram.setUniformValue(matrixLocation, color);
}

void DrawWidget::onTimeElapsed(int elapsedTime, int deltaTime){
    this->elapsedTime = elapsedTime;
    this->deltaTime = deltaTime;
    repaint();
}

void DrawWidget::trackSphere(int ID){
    isTracking = true;
    trackingSphereID = ID;
}

void DrawWidget::stopTracking(){
    isTracking = false;
}

void DrawWidget::onSphereDeleted(int ID){
    if (trackingSphereID == ID)
        isTracking = false;
}
