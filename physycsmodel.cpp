#include "physycsmodel.h"

PhysycsModel::PhysycsModel(float gravitationalConstant) :
    gravitationalConstant(gravitationalConstant){}

bool PhysycsModel::canAddSphere(QVector3D pos, float r){
    for (int i = 1; i < sphereInfos.size(); i++){
        if ((pos - sphereInfos[i].getPos()).length() <= r + sphereInfos[i].r)
            return false; //intersection
    }
    return true;
}

bool PhysycsModel::addSphere(SphereInfo sphere){
    bool canAdd = canAddSphere(sphere.getPos(), sphere.r);
    if (canAdd){
        sphereInfos.push_back(sphere);
    }
    return canAdd;
}

void PhysycsModel::removeSphereOnID(int ID){
    for (int i = 1; i < sphereInfos.size(); i++){
        if (sphereInfos[i].getID() == ID){
            sphereInfos.erase(sphereInfos.begin()+i);
            return;
        }
    }
}

const std::vector<SphereInfo> * PhysycsModel::getSphereInfos(){
    return &sphereInfos;
}

void PhysycsModel::onTimeElapsed(int elapsedTime, int deltaTime){
    if (sphereInfos.size() == 0) return;
    float dtSec = (float)deltaTime/1000.0;
    if (sphereInfos.size() == 1) {
        sphereInfos[0].changePos(sphereInfos[0].getPos()+dtSec*sphereInfos[0].velocity, deltaTime);
        return;
    }

    for (int i = 1; i < sphereInfos.size(); i++){
        for (int j = 0; j < i; j++){
            SphereInfo& sphere1 = sphereInfos[j];
            SphereInfo& sphere2 = sphereInfos[i];
            if ((sphere1.getPos() - sphere2.getPos()).length() > sphere1.r + sphere2.r)
                continue;

            QVector3D normal = (sphere2.getPos() - sphere1.getPos()).normalized();
            QVector3D relativeVelocity = sphere2.velocity - sphere1.velocity;
            float impulse = 2.0f * QVector3D::dotProduct(relativeVelocity, normal) / (sphere1.mass + sphere2.mass);
            sphere1.velocity += impulse * normal * sphere2.mass;
            sphere2.velocity -= impulse * normal * sphere1.mass;

            sphere1.changePos(sphere1.getPos()+dtSec*sphere1.velocity, deltaTime);
            sphere2.changePos(sphere2.getPos()+dtSec*sphere2.velocity, deltaTime);
        }
    }

    std::vector<QVector3D> newPoses;
    for (int i = 0; i < sphereInfos.size(); i++){
        QVector3D curV = sphereInfos[i].velocity;
        QVector3D curPos = sphereInfos[i].getPos();
        QVector3D k1 = curV;
        QVector3D k2 = eulerMethodSpeed(i, curPos + (dtSec * 0.5 * k1), deltaTime);
        QVector3D k3 = eulerMethodSpeed(i, curPos + (dtSec * 0.5 * k2), deltaTime);
        QVector3D k4 = eulerMethodSpeed(i, curPos + (dtSec * k3), deltaTime);
        sphereInfos[i].velocity = eulerMethodSpeed(i, curPos, deltaTime);
        newPoses.push_back(curPos+0.166667*dtSec*(k1 + 2.0*k2 + 2.0*k3 + k4));
    }

    for (int i = 0; i < sphereInfos.size(); i++){
        sphereInfos[i].changePos(newPoses[i], deltaTime);
    }
}

QVector3D PhysycsModel::eulerMethodSpeed(int sphereIndex, QVector3D pos, int deltaTime){
    float dtSec = (float)deltaTime/1000.0;
    return sphereInfos[sphereIndex].velocity + (dtSec * derivativeOfVelocity(sphereIndex, pos));
}

QVector3D PhysycsModel::derivativeOfVelocity(int sphereIndex, QVector3D pos){
    QVector3D sum = QVector3D(0.0, 0.0, 0.0);
    for (int i = 0; i < sphereInfos.size(); i++){
        if (i == sphereIndex) continue;
        QVector3D direction = sphereInfos[i].getPos() - pos;
        float distance = direction.length();
        sum += direction * (gravitationalConstant * sphereInfos[i].mass * (1.0/(distance*distance*distance)));
    }
    return sum;
}
