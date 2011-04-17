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

class RoundaboutTestSegmentItem;

class RoundaboutTestConnectionItem : public QGraphicsPathItem
{
public:
    enum Point {
        P1,
        P2
    };
    RoundaboutTestConnectionItem(QColor color, qreal width, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    void setSegment(Point point, RoundaboutTestSegmentItem *segmentItem, QPointF p, qreal angle);
    void setPoint(Point point, QPointF p, qreal angle);
    void setPoint(Point point, QPointF p);
    void startMove(Point point, QPointF pos);
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor color;
    QPointF p1, p2;
    qreal angle1, angle2, width;
    Point movingPoint;
    RoundaboutTestSegmentItem *segmentItem1, *segmentItem2;
    static QPainterPath createConnectionPath(QPointF p1, qreal angle1, QPointF p2, qreal angle2);
    static QPainterPath createConnectionPath(QPointF p1, qreal angle1, QPointF p2);
};

class RoundaboutScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit RoundaboutScene(QObject *parent = 0);

    RoundaboutTestConnectionItem *createConnectionItem(QColor color, qreal width);

signals:

public slots:

};

#endif // ROUNDABOUTSCENE_H
