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

#include "roundaboutscene.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsProxyWidget>
#include <QTimer>

class RoundaboutTestSwitchEditItem : public QGraphicsPathItem
{
public:
    RoundaboutTestSwitchEditItem(QGraphicsItem *parent = 0);
private:
    QColor normalColor, backgroundColor;
};

class RoundaboutTestPlayItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    RoundaboutTestPlayItem(QRectF rect, QGraphicsItem *parent = 0);
signals:
    void changedState(bool playing);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor, hoverColor;
    bool state, hover;
    QPainterPath playPath, pausePath;
};

class RoundaboutTestCenterItem : public QGraphicsEllipseItem
{
public:
    RoundaboutTestCenterItem(QRectF rect, QGraphicsItem *parent = 0);
private:
    QColor normalColor;
};

class RoundaboutTestDragItem : public QGraphicsPathItem
{
public:
    RoundaboutTestDragItem(QGraphicsItem *parent = 0);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
};

class RoundaboutTestButtonItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    RoundaboutTestButtonItem(const QPainterPath &innerPath, QGraphicsItem *parent = 0);
    RoundaboutTestButtonItem(QRectF rect, QGraphicsItem *parent = 0);
signals:
    void triggered();
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor, hoverColor;
    bool hover;
};

class RoundaboutTestSegmentItem : public QGraphicsPathItem, public RoundaboutTestConnectable
{
public:
    enum Shape {
        NORMAL,
        BENT_AT_BEGIN,
        BENT_AT_END
    };
    RoundaboutTestSegmentItem(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal arcLength, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    bool getState() const;
    void setShape(Shape shape);
    virtual QPointF getConnectionAnchor(RoundaboutTestConnectionPoint point, qreal &angle) const;
protected:
    virtual void connected(RoundaboutTestConnectionPoint point, RoundaboutTestConnectionItem *connectionItem);
    virtual void disconnected();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
private:
    Shape myShape;
    QColor normalColor, highlightedColor, stateColor;
    bool state, hover, highlight;
    QPainterPath normalPath, bentAtBeginPath, bentAtEndPath;
    QPointF bentAtEndAnchor, bentAtBeginAnchor;
    qreal bentAtEndAngle, bentAtBeginAngle;
};

class RoundaboutTestArrowItem : public QGraphicsPathItem
{
public:
    RoundaboutTestArrowItem(QRectF innerRect, QRectF outerRect, qreal angle, qreal tipOffset, QGraphicsItem *parent = 0);
    RoundaboutTestArrowItem(QRectF rect, qreal tipOffset, QGraphicsItem *parent = 0);
private:
    QColor normalColor;
};

class RoundaboutTestKeyItem : public QGraphicsPathItem
{
public:
    enum KeyType {
        BLACK,
        WHITE
    };

    RoundaboutTestKeyItem(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal arcLength, KeyType keyType, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    void setLowkey(bool lowkey);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor, highlightedColor, stateColor, lowkeyColor;
    bool state, hover, highlight, lowkey;
};

class RoundaboutTestKeyboardItem : public QGraphicsPathItem
{
public:
    enum Direction {
        INNER_TO_OUTER,
        OUTER_TO_INNER
    };
    RoundaboutTestKeyboardItem(QRectF innerRect, QRectF outerRect, Direction dir, qreal startAngle, qreal arcLength, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    void setLowkey(bool lowkey);
private:
    QVector<RoundaboutTestKeyItem*> keyItems;
};

class RoundaboutTestSliceItem : public QGraphicsPathItem
{
public:
    RoundaboutTestSliceItem(QRectF innerRect, QRectF outerRect, RoundaboutTestKeyboardItem::Direction dir, qreal startAngle, qreal arcLength, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    RoundaboutTestKeyboardItem *getKeyboardItem();
    RoundaboutTestSegmentItem *getSegmentItem();
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
private:
    QColor normalColor;
    RoundaboutTestKeyboardItem *keyboardItem;
    RoundaboutTestSegmentItem *segmentItem;
};

class RoundaboutTestConductorItem : public QGraphicsRectItem, public RoundaboutTestConnectable, public RoundaboutTestConnectableHost
{
public:
    RoundaboutTestConductorItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual QPointF getConnectionAnchor(RoundaboutTestConnectionPoint point, qreal &angle) const;
    virtual RoundaboutTestConnectable * getConnectableAt(QPointF scenePos);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor;
    QPointF anchor;
    qreal angle;
};

class RoundaboutTestItem : public QGraphicsEllipseItem, public RoundaboutTestConnectableHost
{
public:
    RoundaboutTestItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual RoundaboutTestConnectable * getConnectableAt(QPointF scenePos);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
private:
    int steps;
    qreal sliceAngle;
    RoundaboutTestArrowItem *arrowItem;
    QVector<RoundaboutTestSliceItem*> sliceItems;

    void enterStep(int step);
    void leaveStep(int step);
};

#endif // ROUNDABOUTTESTITEM_H
