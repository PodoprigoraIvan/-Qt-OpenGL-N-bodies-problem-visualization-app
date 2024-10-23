#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtOpenGLWidgets>
#include <QVBoxLayout>
#include <QPushButton>
#include <QObject>
#include <QElapsedTimer>
#include <memory>
#include "physycsmodel.h"
#include "drawwidget.h"
#include "addnewspherewidget.h"
#include "sphereslistwidget.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);

private:
    std::unique_ptr<QTimer> timer;
    QGridLayout layout;
    std::unique_ptr<QElapsedTimer> elapsedTimer;
    int prevTime;
    DrawWidget drawWidget;
    PhysycsModel physycsModel;
    AddNewSphereWidget addNewSphereWidget;
    SpheresListWidget sphereListWidget;
private slots:
    void updateTime();
signals:
    void timeElapsed(int elapsed, int deltaTime);
};

#endif // MAINWINDOW_H
