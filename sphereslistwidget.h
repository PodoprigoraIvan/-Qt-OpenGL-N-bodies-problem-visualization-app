#ifndef SPHERESLISTWIDGET_H
#define SPHERESLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "physycsmodel.h"

class SphereListItem : public QWidget
{
    Q_OBJECT
public:
    SphereListItem(int ID, QColor color);
    int getID();
    void useTrackingBackgroundColor(bool use);

private:
    QColor sphereColor, traсkingColor, defaultColor;
    int ID;
    QLabel idLabel, sphereColorLabel;
    QPushButton trackBtn, removeBtn;
    QHBoxLayout layout;

public slots:
    void onTrackBtnPressed();
    void onRemoveBtnPressed();

signals:
    void trackSphere(int ID);
    void removeSphere(int ID);
};


class SpheresListWidget : public QWidget
{
    Q_OBJECT
public:
    SpheresListWidget(PhysycsModel* physycsModel, QWidget *parent = nullptr);

private:
    QVBoxLayout layout, scrollLayout;
    QPushButton stopTrackBtn;
    QWidget container;
    QScrollArea scrollArea;
    std::vector<SphereListItem*> widgets;
    PhysycsModel* physycsModel;


public slots:
    void onAdd(int ID, QColor color);
    void onTrack(int ID);
    void onRemove(int ID);
    void stopTrackBtnPressed();

signals:
    void trackSphere(int ID);
    void stopTracking();
    void removeOnID(int ID);

};

#endif // SPHERESLISTWIDGET_H
