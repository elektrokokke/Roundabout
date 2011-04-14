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
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor, highlightedColor;
    bool state;
};

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
    void setHighlight(bool highlight);
    bool getState() const;
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor, highlightedColor;
    bool state, hover, highlight;
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
    RoundaboutTestKeyItem(QRectF innerRect, QRectF outerRect, qreal angle, QColor color, QColor highlightedColor, QColor stateColor, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
private:
    QColor normalColor, highlightedColor, stateColor;
    bool state, hover, highlight;
};

class RoundaboutTestKeyboardItem : public QGraphicsPathItem
{
public:
    enum Direction {
        INNER_TO_OUTER,
        OUTER_TO_INNER
    };
    RoundaboutTestKeyboardItem(QRectF innerRect, QRectF outerRect, Direction dir, qreal angle, QColor color, QGraphicsItem *parent = 0);
    void setHighlight(bool highlight);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
private:
    QGraphicsPathItem *child;
    bool hover;
    QVector<RoundaboutTestKeyItem*> keyItems;
};

class RoundaboutTestItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    RoundaboutTestItem(QGraphicsItem *parent = 0);
private:
    int steps;
    QTimer timer;
    qreal stepAngle;
    RoundaboutTestArrowItem *arrowItem;
    QVector<RoundaboutTestSegmentItem*> segmentItems;
    QVector<RoundaboutTestKeyboardItem*> keyboardItems;

    void enterStep(int step);
    void leaveStep(int step);
private slots:
    void changeState(bool play);
    void onTimer();
};

#endif // ROUNDABOUTTESTITEM_H
