#include "geometry.h"

Geometry::Entity::Entity() {
    Geometry::EntitiesCollector::count ++;
    Geometry::EntitiesCollector::instances->append(this);
    this->updateHierarchy();
}
Geometry::Entity::~Entity() {
    delete this->parents;
    delete this->children;
    Geometry::EntitiesCollector::count --;
    int index = Geometry::EntitiesCollector::instances->indexOf(this);
    Geometry::EntitiesCollector::instances->removeAt(index);
}
void Geometry::Entity::updateHierarchy() {
    this->hierarchy.append(this->label);
    this->hierarchy.append("/");
}
QString Geometry::Entity::getName() {
    return this->name;
}
void Geometry::Entity::setName(QString newName) {
    this->name = newName;
}
QString Geometry::Entity::getHierarchy() {
    return this->hierarchy;
}
QVector<Geometry::Entity*>* Geometry::Entity::getParents() {
    return this->parents;
}
QVector<Geometry::Entity*>* Geometry::Entity::getChildren() {
    return this->children;
}
int Geometry::EntitiesCollector::count = 0;
QVector<Geometry::Entity*>* Geometry::EntitiesCollector::instances =
        new QVector<Geometry::Entity*>();
QVector<Geometry::Entity*>* Geometry::EntitiesCollector::getInstances() {
        return Geometry::EntitiesCollector::instances;
}
Geometry::Point::Point(float x, float y, float z) :
        Geometry::Entity() {
    count ++;
    instances->append(this);
    this->coords[0] = x;
    this->coords[1] = y;
    this->coords[2] = z;
}
Geometry::Point::~Point() {
    count --;
    int index = instances->indexOf(this);
    instances->removeAt(index);
}
float* Geometry::Point::getCoords() {
    return this->coords;
}
void Geometry::Point::setCoords(float x, float y, float z) {
    this->coords[0] = x;
    this->coords[1] = y;
    this->coords[2] = z;
}
Geometry::Line::Line(Point* startP, Point* endP) {
    count ++;
    instances->append(this);
    this->hierarchy.append("/");
    this->hierarchy.append(this->label);
    this->startPoint = startP;
    this->endPoint = endP;
}
Geometry::Line::~Line() {
    count --;
    int index = instances->indexOf(this);
    instances->removeAt(index);
}
Geometry::Point* Geometry::Line::getStartPoint() {
    return this->startPoint;
}
void Geometry::Line::setStartPoint(Point* startP) {
    this->startPoint = startP;
}
Geometry::Point* Geometry::Line::getEndPoint() {
    return this->endPoint;
}
void Geometry::Line::setEndPoint(Point* endP) {
    this->endPoint = endP;
}
