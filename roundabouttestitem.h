#ifndef ROUNDABOUTTESTITEM_H
#define ROUNDABOUTTESTITEM_H

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

#include <QGraphicsEllipseItem>

class RoundaboutTestCenterItem : public QGraphicsEllipseItem
{
public:
    RoundaboutTestCenterItem(QRectF rect, QGraphicsItem *parent = 0);
private:
    QColor normalColor;
};

class RoundaboutTestSegmentItem : public QGraphicsPathItem
{
public:
    RoundaboutTestSegmentItem(QRectF innerRect, QRectF outerRect, qreal angle, QGraphicsItem *parent = 0);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
private:
    QColor normalColor, highlightedColor;
};

class RoundaboutTestArrowItem : public QGraphicsPathItem
{
public:
    RoundaboutTestArrowItem(QRectF innerRect, QRectF outerRect, qreal angle, qreal tipOffset, QGraphicsItem *parent = 0);
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
private:
    QRectF rect;
    QColor normalColor;
};

class RoundaboutTestItem : public QGraphicsEllipseItem
{
public:
    RoundaboutTestItem(QGraphicsItem *parent = 0);
private:
    unsigned int steps;
};

#endif // ROUNDABOUTTESTITEM_H
