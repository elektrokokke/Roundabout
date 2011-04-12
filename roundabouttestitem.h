#ifndef ROUNDABOUTTESTITEM_H
#define ROUNDABOUTTESTITEM_H

#include <QGraphicsEllipseItem>

class RoundaboutTestItem : public QGraphicsEllipseItem
{
public:
    RoundaboutTestItem(QGraphicsItem *parent = 0);
private:
    unsigned int steps;
};

#endif // ROUNDABOUTTESTITEM_H
