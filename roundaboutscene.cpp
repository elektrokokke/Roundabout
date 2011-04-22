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

RoundaboutTestConnectable::RoundaboutTestConnectable(bool canConnectP1_, bool canConnectP2_) :
    canConnectP1(canConnectP1_),
    canConnectP2(canConnectP2_),
    connectionItem(0)
{
}

RoundaboutTestConnectionItem * RoundaboutTestConnectable::getConnectionItem()
{
    return connectionItem;
}

RoundaboutTestConnectionPoint RoundaboutTestConnectable::getConnectionPoint() const
{
    return connectionPoint;
}

void RoundaboutTestConnectable::setConnectionItem(RoundaboutTestConnectionPoint point, RoundaboutTestConnectionItem *connectionItem)
{
    Q_ASSERT(this->connectionItem == 0);
    if (((point == P1) && canConnectP1) || ((point == P2) && canConnectP2)) {
        this->connectionItem = connectionItem;
        connectionPoint = point;
        connectionItem->setConnectable(connectionPoint, this);
        connected(point, connectionItem);
    }
}

void RoundaboutTestConnectable::removeConnection()
{
    connectionItem = 0;
    disconnected();
}

void RoundaboutTestConnectable::connected(RoundaboutTestConnectionPoint point, RoundaboutTestConnectionItem *connectionItem)
{
}

void RoundaboutTestConnectable::disconnected()
{
}

RoundaboutTestConnectionItem::RoundaboutTestConnectionItem(qreal width_, QGraphicsItem *parent, QGraphicsScene *scene) :
    QGraphicsPathItem(parent, scene),
    color("steelblue"),
    angle1(0),
    angle2(0),
    width(width_),
    connectable1(0),
    connectable2(0)
{
    setPen(QPen(QBrush(color), width, Qt::SolidLine, Qt::FlatCap));
    setBrush(QBrush(Qt::NoBrush));
    setZValue(-1);
}

void RoundaboutTestConnectionItem::setConnectable(RoundaboutTestConnectionPoint  point, RoundaboutTestConnectable *connectable)
{
    if (point == P1) {
        if (connectable1 && (connectable1 != connectable)) {
            connectable1->removeConnection();
        }
        connectable1 = connectable;
    } else {
        if (connectable2 && (connectable2 != connectable)) {
            connectable2->removeConnection();
        }
        connectable2 = connectable;
    }
    movedConnectable(point);
}

void RoundaboutTestConnectionItem::movedConnectable(RoundaboutTestConnectionPoint  point)
{
    if (point == P1) {
        Q_ASSERT(connectable1);
        p1 = connectable1->getConnectionAnchor(point, angle1);
        if (!connectable2) {
            p2 = p1;
            angle2 = angle1;
        }
        setPath(createConnectionPath(p1, angle1, p2, angle2));
    } else {
        Q_ASSERT(connectable2);
        p2 = connectable2->getConnectionAnchor(point, angle2);
        if (!connectable1) {
            p1 = p2;
            angle1 = angle2;
        }
        setPath(createConnectionPath(p1, angle1, p2, angle2));
    }
}

void RoundaboutTestConnectionItem::startMove(RoundaboutTestConnectionPoint  point)
{
    grabMouse();
    movingPoint = point;
}

void RoundaboutTestConnectionItem::setPoint(RoundaboutTestConnectionPoint  point, QPointF p, qreal angle)
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

void RoundaboutTestConnectionItem::setPoint(RoundaboutTestConnectionPoint  point, QPointF p)
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

void RoundaboutTestConnectionItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    Q_ASSERT(scene());
    QList<QGraphicsItem*> items = scene()->items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);
    for (QList<QGraphicsItem*>::iterator i = items.begin(); i != items.end(); i++) {
        if (RoundaboutTestConnectableHost *host = dynamic_cast<RoundaboutTestConnectableHost*>(*i)) {
            if (RoundaboutTestConnectable *connectable = host->getConnectableAt(event->scenePos())) {
                RoundaboutTestConnectionItem *connectionItem = connectable->getConnectionItem();
                if (connectionItem == this) {
                    return;
                } else if (connectionItem) {
                    break;
                }
                connectable->setConnectionItem(movingPoint, this);
                return;
            }
        }
    }
    if ((movingPoint == P1) && connectable1) {
        connectable1->removeConnection();
        connectable1 = 0;
    } else if ((movingPoint == P2) && connectable2) {
        connectable2->removeConnection();
        connectable2 = 0;
    }
    setPoint(movingPoint, event->pos());
}

void RoundaboutTestConnectionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    ungrabMouse();
    if ((connectable1 == 0) || (connectable2 == 0)) {
        if (connectable1) {
            connectable1->removeConnection();
        } else if (connectable2) {
            connectable2->removeConnection();
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
    QGraphicsScene(parent),
    nextCirclePosition(200, 200),
    nextConductorPosition(-100, 0)
{
}

RoundaboutTestConnectionItem * RoundaboutScene::createConnectionItem()
{
    return new RoundaboutTestConnectionItem(27, 0, this);
}

void RoundaboutScene::createConductor()
{
    RoundaboutTestConductorItem *item = new RoundaboutTestConductorItem(0, this);
    item->setPos(nextConductorPosition);
    nextConductorPosition += QPointF(0, item->rect().height());
}

void RoundaboutScene::onCreatedSequencer(RoundaboutSequencer *sequencer)
{
    RoundaboutSequencerItem *item = new RoundaboutSequencerItem(sequencer, 0, this);
    item->setPos(nextCirclePosition);
    nextCirclePosition += QPointF(item->rect().width() + 50, 0);
}
