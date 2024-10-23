#ifndef PHYSYCSMODEL_H
#define PHYSYCSMODEL_H

#include <QObject>
#include <QMatrix4x4>
#include "sphereinfo.h"

class PhysycsModel : public QObject
{
    Q_OBJECT
public:
    PhysycsModel(float gravitationalConstant = 1.0);
    float gravitationalConstant;
    bool canAddSphere(QVector3D pos, float r);
    bool addSphere(SphereInfo sphere);
    const std::vector<SphereInfo> * getSphereInfos();
    void removeSphereOnID(int ID);

private:
    std::vector<SphereInfo> sphereInfos;
    QVector3D derivativeOfVelocity(int sphereIndex, QVector3D pos);
    QVector3D eulerMethodSpeed(int sphereIndex, QVector3D pos, int deltaTime);

public slots:
    void onTimeElapsed(int elapsedTime, int deltaTime);
};



#endif // PHYSYCSMODEL_H
