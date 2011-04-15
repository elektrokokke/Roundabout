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
#include <QTimer>

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

class RoundaboutTestSegmentItem : public QGraphicsPathItem
{
public:
    enum Shape {
        NORMAL,
        BENT_AT_BEGIN,
        BENT_AT_END
    };
    RoundaboutTestSegmentItem(QRectF innerRect, QRectF outerRect, qreal angle, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    bool getState() const;
    void setShape(Shape shape);
    void changeShape();
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
private:
    QRectF innerRect, outerRect;
    qreal angle;
    Shape myShape;
    QColor normalColor, highlightedColor, stateColor;
    bool state, hover, highlight;
    QPainterPath normalPath, beginPath, endPath;
};

class RoundaboutTestArrowItem : public QGraphicsPathItem
{
public:
    RoundaboutTestArrowItem(QRectF innerRect, QRectF outerRect, qreal angle, qreal tipOffset, QGraphicsItem *parent = 0);
private:
    QRectF rect;
    QColor normalColor;
};

class RoundaboutTestKeyItem : public QGraphicsPathItem
{
public:
    RoundaboutTestKeyItem(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal angle, QColor color, QColor stateColor, QGraphicsItem *parent = 0);
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
    RoundaboutTestKeyboardItem(QRectF innerRect, QRectF outerRect, Direction dir, qreal angle, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    void setLowkey(bool lowkey);
private:
    QVector<RoundaboutTestKeyItem*> keyItems;
};

class RoundaboutTestSliceItem : public QGraphicsPathItem
{
public:
    RoundaboutTestSliceItem(QRectF innerRect, QRectF outerRect, RoundaboutTestKeyboardItem::Direction dir, qreal angle, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
    RoundaboutTestKeyboardItem *getKeyboardItem();
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
private:
    QColor normalColor;
    QPainterPath normalShape, extendedShape;
    RoundaboutTestKeyboardItem *keyboardItem;
    RoundaboutTestSegmentItem *segmentItem;
//    RoundaboutTestDragItem *dragItem;
};

class RoundaboutTestItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    RoundaboutTestItem(QGraphicsItem *parent = 0);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
private:
    int steps;
    QTimer timer;
    qreal sliceAngle;
    RoundaboutTestArrowItem *arrowItem;
    QVector<RoundaboutTestSliceItem*> sliceItems;
    qreal bpm;

    void enterStep(int step);
    void leaveStep(int step);
private slots:
    void changeState(bool play);
    void onTimer();
};

#endif // ROUNDABOUTTESTITEM_H
