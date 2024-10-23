#include "mainwindow.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , prevTime(0)
    , addNewSphereWidget(&physycsModel)
    , sphereListWidget(&physycsModel)
{
    setLayout(&layout);

    layout.setContentsMargins(0, 0, 10, 0);

    layout.addWidget(&drawWidget, 0, 0, 2, 1);

    addNewSphereWidget.setDrawer(&drawWidget);
    layout.addWidget(&addNewSphereWidget, 0, 1);

    layout.addWidget(&sphereListWidget, 1, 1);

    timer = std::make_unique<QTimer>();
    timer->setInterval(25);
    timer->start();

    drawWidget.setSphereInfos(physycsModel.getSphereInfos());
    SphereInfo::setMsToAddNewVertexInTrace(150);

    QObject::connect(timer.get(), SIGNAL(timeout()), this, SLOT(updateTime()));
    QObject::connect(this, SIGNAL(timeElapsed(int,int)), &drawWidget, SLOT(onTimeElapsed(int,int)));
    QObject::connect(this, SIGNAL(timeElapsed(int,int)), &physycsModel, SLOT(onTimeElapsed(int,int)));

    QObject::connect(&addNewSphereWidget, SIGNAL(addSphere(int, QColor)), &sphereListWidget, SLOT(onAdd(int, QColor)));

    QObject::connect(&sphereListWidget, &SpheresListWidget::trackSphere, &drawWidget, &DrawWidget::trackSphere);
    QObject::connect(&sphereListWidget, &SpheresListWidget::stopTracking, &drawWidget, &DrawWidget::stopTracking);
    QObject::connect(&sphereListWidget, &SpheresListWidget::removeOnID, &drawWidget, &DrawWidget::onSphereDeleted);

    elapsedTimer = std::make_unique<QElapsedTimer>();
    elapsedTimer->start();
    prevTime = elapsedTimer->elapsed();

    addNewSphereWidget.addSphereManually(QColor(0, 0, 255), SphereInfo(QVector3D(-3.0,3.0,0.0), QVector3D(0.0,0.0,1.0), 100.0, 1.0, 300, QVector3D(0.0, 0.1, 0.0)));
    addNewSphereWidget.addSphereManually(QColor(0, 0, 255), SphereInfo(QVector3D(3.0,3.0,0.0), QVector3D(0.0,0.0,1.0), 1.0, 0.5, 300, QVector3D(0.0, 3.75, 0.0)));
    addNewSphereWidget.addSphereManually(QColor(0, 0, 255), SphereInfo(QVector3D(-3.0,3.0,6.0), QVector3D(0.0,0.0,1.0), 1.0, 0.5, 300, QVector3D(-3.75, 0.0, 0.0)));
}

void MainWidget::updateTime(){
    int elapsedTime = elapsedTimer->elapsed();
    int deltaTime = elapsedTime - prevTime;
    prevTime = elapsedTime;
    emit timeElapsed(elapsedTime, deltaTime);
}
