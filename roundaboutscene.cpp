/*
    Copyright 2011 Arne Jacobs <jarne@jarne.de>

    This file is part of Roundabout.

    Roundabout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Roundabout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Roundabout.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "roundaboutscene.h"
#include "roundabouttestitem.h"
#include <QGraphicsSceneMouseEvent>

RoundaboutTestConnectionItem::RoundaboutTestConnectionItem(QColor color_, qreal width_, QGraphicsItem *parent, QGraphicsScene *scene) :
    QGraphicsPathItem(parent, scene),
    color(color_),
    angle1(0),
    angle2(0),
    width(width_),
    segmentItem1(0),
    segmentItem2(0)
{
    setPen(QPen(QBrush(color), width, Qt::SolidLine, Qt::FlatCap));
    setBrush(QBrush(Qt::NoBrush));
}

void RoundaboutTestConnectionItem::setSegment(Point point, RoundaboutTestSegmentItem *segmentItem, QPointF p, qreal angle)
{
    if (point == P1) {
        if (segmentItem1) {
            segmentItem1->removeConnection();
        }
        segmentItem1 = segmentItem;
        if ((p != p1) || (angle != angle1)) {
            p1 = p;
            angle1 = angle;
            setPath(createConnectionPath(p1, angle1, p2, angle2));
        }
    } else {
        if (segmentItem2) {
            segmentItem2->removeConnection();
        }
        segmentItem2 = segmentItem;
        if ((p != p2) || (angle != angle2)) {
            p2 = p;
            angle2 = angle;
            setPath(createConnectionPath(p1, angle1, p2, angle2));
        }
    }
}

void RoundaboutTestConnectionItem::setPoint(Point point, QPointF p, qreal angle)
{
    if (point == P1) {
        if ((p != p1) || (angle != angle1)) {
            p1 = p;
            angle1 = angle;
            setPath(createConnectionPath(p1, angle1, p2, angle2));
        }
    } else {
        if ((p != p2) || (angle != angle2)) {
            p2 = p;
            angle2 = angle;
            setPath(createConnectionPath(p1, angle1, p2, angle2));
        }
    }
}

void RoundaboutTestConnectionItem::setPoint(Point point, QPointF p)
{
    if (point == P1) {
        if (p != p1) {
            p1 = p;
            setPath(createConnectionPath(p2, angle2, p1));
        }
    } else {
        if (p != p2) {
            p2 = p;
            setPath(createConnectionPath(p1, angle1, p2));
        }
    }
}

void RoundaboutTestConnectionItem::startMove(Point point)
{
    grabMouse();
    movingPoint = point;
}

void RoundaboutTestConnectionItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    Q_ASSERT(scene());
    QList<QGraphicsItem*> items = scene()->items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);
    for (QList<QGraphicsItem*>::iterator i = items.begin(); i != items.end(); i++) {
        if (RoundaboutTestItem *testItem = dynamic_cast<RoundaboutTestItem*>(*i)) {
            RoundaboutTestSegmentItem *segmentItem = testItem->getSegmentAt(testItem->mapFromScene(event->scenePos()));
            RoundaboutTestConnectionItem *connectionItem = segmentItem->getConnectionItem();
            if (connectionItem == this) {
                return;
            } else if (connectionItem) {
                break;
            }
            segmentItem->setConnectionItem(movingPoint, this);
            return;
        }
    }
    if ((movingPoint == P1) && segmentItem1) {
        segmentItem1->removeConnection();
        segmentItem1 = 0;
    } else if ((movingPoint == P2) && segmentItem2) {
        segmentItem2->removeConnection();
        segmentItem2 = 0;
    }
    setPoint(movingPoint, event->pos());
}

void RoundaboutTestConnectionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    ungrabMouse();
    if ((segmentItem1 == 0) || (segmentItem2 == 0)) {
        if (segmentItem1) {
            segmentItem1->removeConnection();
        } else if (segmentItem2) {
            segmentItem2->removeConnection();
        }
        delete this;
    }
}

QPainterPath RoundaboutTestConnectionItem::createConnectionPath(QPointF p1, qreal angle1, QPointF p2, qreal angle2)
{
    qreal length = 0.5 * QLineF(p1, p2).length();
    QPointF dir1 = QLineF::fromPolar(length, -angle1).p2();
    QPointF dir2 = QLineF::fromPolar(length, -angle2).p2();
    QPainterPath path(p1);
    path.cubicTo(p1 + dir1, p2 + dir2, p2);
    return path;
}

QPainterPath RoundaboutTestConnectionItem::createConnectionPath(QPointF p1, qreal angle1, QPointF p2)
{
    qreal length = 0.5 * QLineF(p1, p2).length();
    QPointF dir1 = QLineF::fromPolar(length, -angle1).p2();
    QPainterPath path(p1);
    path.quadTo(p1 + dir1, p2);
    return path;
}

RoundaboutScene::RoundaboutScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

RoundaboutTestConnectionItem * RoundaboutScene::createConnectionItem(QColor color, qreal width)
{
    return new RoundaboutTestConnectionItem(color, width, 0, this);
}
