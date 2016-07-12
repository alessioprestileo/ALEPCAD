#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QVector>
#include <QString>

namespace Geometry {

template <class T> class Collector{
protected:
    static int count;
    static QVector<T*>* instances;
public:
    static QVector<T*>* getInstances();

};
class Entity {
protected:
    Entity();
    QString name = QString();
    const char* label;
    QString hierarchy = QString();
    QVector<Entity*>* parents = new QVector<Entity*>();
    QVector<Entity*>* children = new QVector<Entity*>();
    virtual void updateLabel() = 0;
    void updateHierarchy(const char* label);
public:
    ~Entity();
    QString getName();
    void setName(QString newName);
    QString getHierarchy();
    QVector<Entity*>* getParents();
    QVector<Entity*>* getChildren();
};
class EntitiesCollector {
    friend class Entity;
private:
    EntitiesCollector();
    ~EntitiesCollector();
    static int count;
    static QVector<Entity*>* instances;
public:
    static QVector<Entity*>* getInstances();
};
class Point : public Entity, public Collector<Point> {
private:
    float coords[3];
protected:
    virtual void updateLabel();
public:
    Point(float x, float y, float z);
    ~Point();
    float* getCoords();
    void setCoords(float x, float y, float z);
};
class Line : public Entity, public Collector<Line> {
private:
    Point* startPoint;
    Point* endPoint;
protected:
    virtual void updateLabel();
public:
    Line(Point *startP, Point *endP);
    ~Line();
    Point* getStartPoint();
    void setStartPoint(Point* startP);
    Point* getEndPoint();
    void setEndPoint(Point* endP);
};
}

template <class T> int Geometry::Collector<T>::count = 0;
template <class T> QVector<T*>* Geometry::Collector<T>::instances =
        new QVector<T*>();
template <class T> QVector<T*>* Geometry::Collector<T>::getInstances() {
    return instances;
}
#endif // GEOMETRY_H
