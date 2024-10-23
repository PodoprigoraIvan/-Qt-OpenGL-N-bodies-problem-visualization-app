#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include "camera.h"
#include "sphereinfo.h"

class DrawWidget : public QOpenGLWidget, public QOpenGLFunctions_4_5_Core {
    Q_OBJECT
public:
    DrawWidget(QWidget *parent = nullptr);
    void setModelMatrix(QMatrix4x4 matrix);
    void setColor(QVector3D color);
    void setSphereInfos(const std::vector<SphereInfo> * sphereInfos);
    QVector3D getCameraPos();
    QVector3D getCameraDirection();

protected:
    void initializeGL() override;

    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void drawSphere(const SphereInfo* sphere);

    void drawLine(const QVector3D& firstVertex, const QVector3D& secondVertex, const QVector3D& color, const float& lineWidth);

private:
    GLuint VAO_sphere, VBO_sphere, VAO_line, VBO_line;
    QOpenGLShaderProgram shaderProgram;
    int elapsedTime;
    int deltaTime;
    bool mouseCaptured;
    Camera camera;
    unsigned int * indices_sphere;
    int indices_sphere_size;
    unsigned int * indices_line;
    const std::vector<SphereInfo> * sphereInfos;
    int trackingSphereID;
    bool isTracking;

public slots:
    void onTimeElapsed(int elapsedTime, int deltaTime);
    void trackSphere(int ID);
    void stopTracking();
    void onSphereDeleted(int ID);
};
#endif // DRAWWIDGET_H
