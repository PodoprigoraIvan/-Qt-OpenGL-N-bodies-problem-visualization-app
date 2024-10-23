#ifndef ADDNEWSPHEREWIDGET_H
#define ADDNEWSPHEREWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMatrix4x4>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QColorDialog>
#include "sphereinfo.h"
#include "drawwidget.h"
#include "physycsmodel.h"

class Vector3DInput : public QWidget {
public:
    Vector3DInput(QString labelText = "Vector3D input");
    QVector3D getVector();
private:
    QLabel nameLabel, xLabel, yLabel, zLabel;
    QDoubleSpinBox xInput, yInput, zInput;
    QHBoxLayout layout;
};

class AddNewSphereWidget : public QWidget
{
    Q_OBJECT
public:
    AddNewSphereWidget(PhysycsModel* physycsModdel, DrawWidget * drawer = nullptr, QWidget *parent = nullptr);
    void setDrawer(DrawWidget * newDrawer);
    void addSphereManually(QColor color, SphereInfo sphere);

private:
    QGridLayout layout;
    QLabel title, velocityLengthLabel, radiusLabel, massLabel, spawnOnLable, velocityDirectionLabel, colorLabel, errorLabel;
    QDoubleSpinBox velocityLengthInput, radiusInput, massInput;

    QButtonGroup spawnSelectGroup;
    QRadioButton cameraPosBtn, specificPosBtn;

    QButtonGroup velocityTypeSelectGroup;
    QRadioButton cameraDirBtn, specificDirBtn;

    QPushButton addButton, pickColorButton;

    Vector3DInput posVecInput, dirVecInput;

    QColor color;

    DrawWidget * drawer;
    PhysycsModel* physycsModel;

signals:
    void addSphere(int ID, QColor color);

public slots:
    void openColorDialog();
    void addSphereButtonClick();
    void onCameraPosToggle(bool active);
    void onSpecificPosToggle(bool active);
    void onCameraDirToggle(bool active);
    void onSpecificDirToggle(bool active);
};

#endif // ADDNEWSPHEREWIDGET_H
