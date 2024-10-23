#ifndef SPHEREINFO_H
#define SPHEREINFO_H

#include <QMatrix4x4>
#include <deque>

class SphereInfo
{
public:
    SphereInfo(QVector3D pos,  QVector3D color, float mass, float r, int traceLength = 10, QVector3D velocity = QVector3D(0.0, 0.0, 0.0));
    void changePos(QVector3D newPos, int deltaTimeMs);
    int getID() const;
    QVector3D getPos() const;
    std::deque<QVector3D> const & getTraceVertices() const;
    static void setMsToAddNewVertexInTrace(int ms);
    void setTraceLengh(int newTraceLength);

public:
    float mass;
    float r;
    QVector3D velocity;
    QVector3D color;

private:
    QVector3D pos;
    int msSinceTraceChaged;
    inline static int lastID;
    inline static int msToAddNewVertexInTrace;
    int traceLength;
    int ID;
    std::deque<QVector3D> traceVertices;
};

#endif // SPHEREINFO_H
