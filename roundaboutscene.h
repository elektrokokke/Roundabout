#ifndef ROUNDABOUTSCENE_H
#define ROUNDABOUTSCENE_H

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

#include <QGraphicsScene>
#include <QGraphicsPathItem>

class RoundaboutTestConnectionItem;
class RoundaboutSequencer;

enum RoundaboutTestConnectionPoint {
    P1,
    P2
};

class RoundaboutTestConnectable
{
public:
    RoundaboutTestConnectable(bool canConnectP1, bool canConnectP2);
    virtual ~RoundaboutTestConnectable();
    RoundaboutTestConnectionItem * getConnectionItem();
    RoundaboutTestConnectionPoint getConnectionPoint() const;
    virtual QPointF getConnectionAnchor(RoundaboutTestConnectionPoint point, qreal &angle) const = 0;
    void setConnectionItem(RoundaboutTestConnectionPoint point, RoundaboutTestConnectionItem *connectionItem);
    void removeConnection();
protected:
    virtual void connected(RoundaboutTestConnectionPoint point, RoundaboutTestConnectionItem *connectionItem);
    virtual void disconnected();
private:
    bool canConnectP1, canConnectP2;
    RoundaboutTestConnectionItem *connectionItem;
    RoundaboutTestConnectionPoint connectionPoint;
};

class RoundaboutTestConnectableHost
{
public:
    virtual RoundaboutTestConnectable * getConnectableAt(QPointF scenePos) = 0;
};

class RoundaboutTestConnectionItem : public QGraphicsPathItem
{
public:
    RoundaboutTestConnectionItem(qreal width, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    void setConnectable(RoundaboutTestConnectionPoint point, RoundaboutTestConnectable *connectable);
    void movedConnectable(RoundaboutTestConnectionPoint point);
    void startMove(RoundaboutTestConnectionPoint point);
protected:
    void setPoint(RoundaboutTestConnectionPoint point, QPointF pos, qreal angle);
    void setPoint(RoundaboutTestConnectionPoint point, QPointF pos);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor color;
    QPointF p1, p2;
    qreal angle1, angle2, width;
    RoundaboutTestConnectionPoint movingPoint;
    RoundaboutTestConnectable *connectable1, *connectable2;
    static QPainterPath createConnectionPath(QPointF p1, qreal angle1, QPointF p2, qreal angle2);
    static QPainterPath createConnectionPath(QPointF p1, qreal angle1, QPointF p2);
};

class RoundaboutScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit RoundaboutScene(QObject *parent = 0);
    RoundaboutTestConnectionItem *createConnectionItem();
    void createConductor();
signals:
public slots:
    void onCreatedSequencer(RoundaboutSequencer *sequencer);
private:
    QPointF nextCirclePosition, nextConductorPosition;
};

#endif // ROUNDABOUTSCENE_H
