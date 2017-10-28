#include "geometry.h"

Geometry::Entity::Entity() {
    Geometry::EntitiesCollector::count ++;
    Geometry::EntitiesCollector::instances->append(this);
    this->updateLabel();
    this->updateHierarchy(this->label);
}
Geometry::Entity::~Entity() {
    delete this->parents;
    delete this->children;
    Geometry::EntitiesCollector::count --;
    int index = Geometry::EntitiesCollector::instances->indexOf(this);
    Geometry::EntitiesCollector::instances->removeAt(index);
}
void Geometry::Entity::updateLabel() {
    this->label = "Entity";
}
void Geometry::Entity::updateHierarchy(const char *label) {
    this->hierarchy.append(label);
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
void Geometry::EntitiesCollector::drawEntities(GLWidget* glWidget) {
    QVector<Geometry::Entity*>* entitiesCollector = Geometry::EntitiesCollector::getInstances();
    QVector<Geometry::Entity*>::iterator iterator = entitiesCollector->begin();
    while (iterator != entitiesCollector->end()) {
        qDebug()<<"value from collector: "<<*iterator;
        (*iterator)->draw(glWidget);
        iterator++;
    }
}
Geometry::Point::Point(float x, float y, float z) :
        Geometry::Entity() {
    count ++;
    instances->append(this);
    this->updateLabel();
    this->updateHierarchy(this->label);
    this->name = QString(this->label)+QString("_")+QString("%1").arg(count);
    this->coords[0] = x;
    this->coords[1] = y;
    this->coords[2] = z;
}
Geometry::Point::~Point() {
    count --;
    int index = instances->indexOf(this);
    instances->removeAt(index);
}
void Geometry::Point::updateLabel() {
    this->label = "Point";
}
float* Geometry::Point::getCoords() {
    return this->coords;
}
void Geometry::Point::setCoords(float x, float y, float z) {
    this->coords[0] = x;
    this->coords[1] = y;
    this->coords[2] = z;
}
void Geometry::Point::draw(GLWidget* glWidget) {
    qDebug()<<"drawing point: ";
    QVector<GLfloat> vertCoords = QVector<GLfloat>();
    vertCoords.append(this->coords[0]);
    vertCoords.append(this->coords[1]);
    vertCoords.append(this->coords[2]);
    glWidget->drawPoint(vertCoords);
    qDebug()<<"finished drawing point.";
}
Geometry::Line::Line(Point* startP, Point* endP) {
    count ++;
    instances->append(this);
    this->updateLabel();
    this->updateHierarchy(this->label);
    this->startPoint = startP;
    this->endPoint = endP;
}
Geometry::Line::~Line() {
    count --;
    int index = instances->indexOf(this);
    instances->removeAt(index);
}
void Geometry::Line::updateLabel() {
    this->label = "Line";
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
void Geometry::Line::draw(GLWidget* glWidget) {
    qDebug()<<"drawing line: ";
    qDebug()<<"finished drawing line.";
}
