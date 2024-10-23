#include "sphereinfo.h"

SphereInfo::SphereInfo(QVector3D pos,  QVector3D color, float mass, float r, int traceLength, QVector3D velocity) :
    mass(mass),
    pos(pos),
    color(color),
    r(r),
    msSinceTraceChaged(0),
    traceLength(traceLength),
    velocity(velocity)
    {
    if (this->traceLength < 2) this->traceLength = 10;
    traceVertices = std::deque<QVector3D>();
    traceVertices.push_back(pos);
    ID = lastID++;
}

void SphereInfo::changePos(QVector3D newPos, int deltaTimeMs){
    pos = newPos;
    if(msToAddNewVertexInTrace < 40) msToAddNewVertexInTrace = 40;
    msSinceTraceChaged += deltaTimeMs;
    if (msSinceTraceChaged >= msToAddNewVertexInTrace){
        traceVertices.push_back(newPos);
        if (traceVertices.size() > traceLength)
            traceVertices.pop_front();
        msSinceTraceChaged = 0;
    }
}

int SphereInfo::getID() const{
    return ID;
}

QVector3D SphereInfo::getPos() const{
    return pos;
}

std::deque<QVector3D> const & SphereInfo::getTraceVertices() const{
    return traceVertices;
}

void SphereInfo::setTraceLengh(int newTraceLength){
    if (newTraceLength > traceLength){
        traceLength = newTraceLength;
    }
    else {
        int toPop = traceLength - newTraceLength;
        for (int i = 0; i < toPop; i++){
            traceVertices.pop_front();
        }
    }
}

void SphereInfo::setMsToAddNewVertexInTrace(int ms){
    if (ms >= 40)
        msToAddNewVertexInTrace = ms;
}
