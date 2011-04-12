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

#include "roundabouttestitem.h"
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>
#include <QDebug>

QRectF operator*(qreal scale, const QRectF &r1)
{
    return QRectF(r1.x() * scale, r1.y() * scale, r1.width() * scale, r1.height() * scale);
}

QRectF operator+(const QRectF &r1, const QRectF &r2)
{
    return QRectF(r1.x() + r2.x(), r1.y() + r2.y(), r1.width() + r2.width(), r1.height() + r2.height());
}

RoundaboutTestCenterItem::RoundaboutTestCenterItem(QRectF rect, QGraphicsItem *parent) :
    QGraphicsEllipseItem(rect, parent),
    normalColor("steelblue")
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(normalColor));
}

RoundaboutTestSegmentItem::RoundaboutTestSegmentItem(QRectF innerRect, QRectF outerRect, qreal angle, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor("lightsteelblue"),
    highlightedColor("steelblue")
{
    setAcceptHoverEvents(true);
    qreal gapAngle = 1;
    QPainterPath path;
    path.arcMoveTo(innerRect, -(angle - gapAngle * 0.5));
    QPointF innerPos = path.currentPosition();
    path.arcMoveTo(outerRect, -(gapAngle * 0.5));
    path.arcTo(outerRect, -(gapAngle * 0.5), -(angle - gapAngle));
    path.lineTo(innerPos);
    path.arcTo(innerRect, -(angle - gapAngle * 0.5), angle - gapAngle);
    path.closeSubpath();
    setPath(path);
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(normalColor));
}

void RoundaboutTestSegmentItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    setBrush(QBrush(highlightedColor));
}

void RoundaboutTestSegmentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    setBrush(QBrush(normalColor));
}

RoundaboutTestArrowItem::RoundaboutTestArrowItem(QRectF innerRect, QRectF outerRect, qreal angle, qreal tipOffset, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    rect(outerRect),
    normalColor("orangered")
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    QRectF tipRect = 0.5 * (innerRect + outerRect);
    QPainterPath path;
    path.arcMoveTo(tipRect, -(tipOffset - 0.5 * angle));
    QPointF arrowTipBack = path.currentPosition();
    path.arcMoveTo(tipRect, -(tipOffset + 0.5 * angle));
    QPointF arrowTipFront = path.currentPosition();
    path.arcMoveTo(innerRect, -0.5 * angle);
    QPointF arrowInnerFront = path.currentPosition();
    path.arcMoveTo(outerRect, 0.5 * angle);
    path.arcTo(outerRect, 0.5 * angle, -angle);
    path.lineTo(arrowTipFront);
    path.lineTo(arrowInnerFront);
    path.arcTo(innerRect, -0.5 * angle, angle);
    path.lineTo(arrowTipBack);
    path.closeSubpath();
    setPath(path);
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(normalColor));
}

QVariant RoundaboutTestArrowItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
    // just playing around...
    if (change == QGraphicsItem::ItemPositionChange) {
        // rotate the item instead of moving it:
        QPointF newPos = value.toPointF();
        qDebug() << "pos()" << pos();
        qDebug() << "newPos" << newPos;
        // compute the current position on the circle:
        qreal rotationInRadians = rotation() * M_PI / 180.0;
        QPointF posOnCircle(0.5 * rect.width() * cos(rotationInRadians), 0.5 * rect.height() * sin(rotationInRadians));
        qDebug() << "posOnCircle" << posOnCircle;
        QPointF newPosOnCircle = posOnCircle - pos() + newPos;
        qDebug() << "newPosOnCircle" << newPosOnCircle;
        // compute the new angle:
        qreal newAngle = atan2(newPosOnCircle.y(), newPosOnCircle.x()) * 180.0 / M_PI;
        qDebug() << "newAngle" << newAngle;
        setTransformOriginPoint(-newPos);
        setRotation(newAngle);
        setPath(path().translated(pos() - newPos));
        return newPos;
    } else {
        return QGraphicsPathItem::itemChange(change, value);
    }
}

RoundaboutTestItem::RoundaboutTestItem(QGraphicsItem *parent) :
    QGraphicsEllipseItem(-200, -200, 400, 400, parent),
    steps(16)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::NoBrush));
    // create a circle in the center:
    QRectF centerRect = 0.2 * rect();
    new RoundaboutTestCenterItem(centerRect, this);
    // create a circle segment for each step:
    QRectF stepOuterRect = rect();
    QRectF stepInnerRect = 0.85 * rect();
    qreal stepAngle = 360.0 / steps;
    for (unsigned int i = 0; i < steps; i++) {
        (new RoundaboutTestSegmentItem(stepInnerRect, stepOuterRect, stepAngle, this))->rotate(stepAngle * i);
    }
    // create arrows:
    QRectF arrowOuterRect = rect();
    QRectF arrowInnerRect = 0.85 * rect();
    qreal arrowAngle = 12;
    qreal arrowTipOffset = 5;
    for (unsigned int i = 0; i < 1; i++) {
        (new RoundaboutTestArrowItem(arrowInnerRect, arrowOuterRect, arrowAngle, arrowTipOffset, this))->rotate(arrowAngle * i);
    }
}
