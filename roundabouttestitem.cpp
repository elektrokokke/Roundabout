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
#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QCursor>
#include <QApplication>
#include <cmath>

QRectF operator*(qreal scale, const QRectF &r1)
{
    return QRectF(r1.x() * scale, r1.y() * scale, r1.width() * scale, r1.height() * scale);
}

QRectF operator+(const QRectF &r1, const QRectF &r2)
{
    return QRectF(r1.x() + r2.x(), r1.y() + r2.y(), r1.width() + r2.width(), r1.height() + r2.height());
}

QPointF rotate(QPointF p, qreal angle)
{
    QLineF line(QPointF(0, 0), p);
    line.setAngle(line.angle() - angle);
    return line.p2();
}

QColor mixColors(QColor c1, QColor c2, double weight1, double weight2)
{
    QColor mixed;
    qreal r1, g1, b1, r2, g2, b2;
    c1.getRgbF(&r1, &g1, &b1);
    c2.getRgbF(&r2, &g2, &b2);
    mixed.setRgbF((r1 * weight1 + r2 * weight2) / (weight1 + weight2), (g1 * weight1 + g2 * weight2) / (weight1 + weight2), (b1 * weight1 + b2 * weight2) / (weight1 + weight2));
    return mixed;
}

QPainterPath createSegmentPath(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal arcLength)
{
    QPainterPath path;
    path.arcMoveTo(innerRect, -startAngle - arcLength);
    QPointF innerPos = path.currentPosition();
    path.arcMoveTo(outerRect, -startAngle);
    path.arcTo(outerRect, -startAngle, -arcLength);
    path.lineTo(innerPos);
    path.arcTo(innerRect,- startAngle - arcLength, arcLength);
    path.closeSubpath();
    return path;
}

RoundaboutTestPlayItem::RoundaboutTestPlayItem(QRectF rect, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor("steelblue"),
    hoverColor("lightsteelblue"),
    state(false),
    hover(false)
{
    setPen(QPen(QBrush(Qt::white), 3));
    setAcceptHoverEvents(true);

    playPath.moveTo(rect.x() + rect.width() * 0.25, rect.y());
    playPath.lineTo(rect.right(), rect.center().y());
    playPath.lineTo(rect.x() + rect.width() * 0.25, rect.bottom());
    playPath.closeSubpath();

    pausePath.addRect(0.8 * rect);

    setBrush(QBrush(hover ? hoverColor : normalColor));
    setPath(state ? pausePath : playPath);
}

void RoundaboutTestPlayItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hover = true;
    setBrush(QBrush(hover ? hoverColor : normalColor));
}

void RoundaboutTestPlayItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hover = false;
    setBrush(QBrush(hover ? hoverColor : normalColor));
}


void RoundaboutTestPlayItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    state = !state;
    setPath(state ? pausePath : playPath);
    changedState(state);
}

RoundaboutTestCenterItem::RoundaboutTestCenterItem(QRectF rect, QGraphicsItem *parent) :
    QGraphicsEllipseItem(rect, parent),
    normalColor("steelblue")
{
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(normalColor));
}

RoundaboutTestDragItem::RoundaboutTestDragItem(QGraphicsItem *parent) :
    QGraphicsPathItem(parent)
{
    setPen(QPen(QBrush(Qt::white), 2));
    setBrush(QColor("steelblue"));
    setCursor(Qt::OpenHandCursor);
    QPainterPath path;
    path.addEllipse(-8, -8, 16, 16);
    setPath(path);
}

void RoundaboutTestDragItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    setCursor(Qt::ClosedHandCursor);
}

void RoundaboutTestDragItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    setCursor(Qt::OpenHandCursor);
}

void RoundaboutTestDragItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
        return;
    }
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);

    QRect bounds = boundingRect().toRect().adjusted(-1, -1, 1, 1);
    QPixmap pixmap(bounds.width(), bounds.height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.translate(-bounds.x(), -bounds.y());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen());
    painter.setBrush(brush());
    painter.drawPath(path());
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(-bounds.x(), -bounds.y()));

    drag->exec();
    setCursor(Qt::OpenHandCursor);
}

RoundaboutTestButtonItem::RoundaboutTestButtonItem(const QPainterPath &innerPath, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor("steelblue"),
    hoverColor("lightsteelblue"),
    hover(false)
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(hover ? hoverColor : normalColor));
    setAcceptHoverEvents(true);
    QRectF rect = innerPath.boundingRect();
    QPointF center = rect.center();
    rect.translate(-center);
    rect = 2.0 * rect;
    QPainterPath path;
    path.addEllipse(rect);
    path -= innerPath.translated(-center);
    setPath(path);
}


RoundaboutTestButtonItem::RoundaboutTestButtonItem(QRectF rect, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor("steelblue"),
    hoverColor("lightsteelblue"),
    hover(false)
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(hover ? hoverColor : normalColor));
    setAcceptHoverEvents(true);
    QPointF center = rect.center();
    rect.translate(-center);
    rect = 2.0 * rect;
    QPainterPath path;
    path.addEllipse(rect);
    setPath(path);
}

void RoundaboutTestButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hover = true;
    setBrush(QBrush(hover ? hoverColor : normalColor));
}

void RoundaboutTestButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hover = false;
    setBrush(QBrush(hover ? hoverColor : normalColor));
}

void RoundaboutTestButtonItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    triggered();
}

RoundaboutTestSegmentItem::RoundaboutTestSegmentItem(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal arcLength, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    myShape(BENT_AT_BEGIN),
    normalColor("lightsteelblue"),
    highlightedColor(Qt::white),
    stateColor("steelblue"),
    state(true),
    hover(false),
    highlight(false),
    connectionItem(0)
{
    Q_ASSERT(outerRect.width() == outerRect.height());
    Q_ASSERT(innerRect.width() == innerRect.height());
    Q_ASSERT(innerRect.center() == outerRect.center());
    setPen(QPen(QBrush(Qt::white), 3));
    setHighlight(false);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

    qreal innerRadius = 0.5 * innerRect.width();
    qreal outerRadius = 0.5 * outerRect.width();
    qreal radius = 0.5 * (innerRadius + outerRadius);
    qreal bentArcLength = 90 - arcLength;
    qreal bentRadius = radius * arcLength / bentArcLength;
    qreal bentInnerRadius = bentRadius + innerRadius - radius;
    qreal bentOuterRadius = bentRadius + outerRadius - radius;
    QPointF center = innerRect.center();
    normalPath = createSegmentPath(innerRect, outerRect, startAngle, arcLength);
    {
        QPointF bentCenter = center + ::rotate(QPointF(radius + bentRadius, 0), startAngle);
        QRectF bentInnerRect(bentCenter.x() - bentInnerRadius, bentCenter.y() - bentInnerRadius, bentInnerRadius * 2, bentInnerRadius * 2);
        QRectF bentOuterRect(bentCenter.x() - bentOuterRadius, bentCenter.y() - bentOuterRadius, bentOuterRadius * 2, bentOuterRadius * 2);
        bentAtEndPath = normalPath | createSegmentPath(bentInnerRect, bentOuterRect, startAngle + 180, -bentArcLength);
        QPainterPath anchorPath;
        anchorPath.arcMoveTo(0.5 * (bentInnerRect + bentOuterRect), -(startAngle + 180 - bentArcLength * 0.95));
        bentAtEndAnchor = anchorPath.currentPosition();
        bentAtEndAngle = startAngle - bentArcLength * 0.95 + 90;
    }
    {
        QPointF bentCenter = center + ::rotate(QPointF(radius + bentRadius, 0), startAngle + arcLength);
        QRectF bentInnerRect(bentCenter.x() - bentInnerRadius, bentCenter.y() - bentInnerRadius, bentInnerRadius * 2, bentInnerRadius * 2);
        QRectF bentOuterRect(bentCenter.x() - bentOuterRadius, bentCenter.y() - bentOuterRadius, bentOuterRadius * 2, bentOuterRadius * 2);
        bentAtBeginPath = normalPath | createSegmentPath(bentInnerRect, bentOuterRect, startAngle + 180 + arcLength, bentArcLength);
        QPainterPath anchorPath;
        anchorPath.arcMoveTo(0.5 * (bentInnerRect + bentOuterRect), -(startAngle + 180 + arcLength + bentArcLength * 0.95));
        bentAtBeginAnchor = anchorPath.currentPosition();
        bentAtBeginAngle = startAngle + arcLength + bentArcLength * 0.95 - 90;
    }

    setShape(NORMAL);
}

void RoundaboutTestSegmentItem::setHighlight(bool highlight)
{
    this->highlight = highlight;
    if ((hover && !state) || (highlight && state)) {
        setBrush(QBrush(highlightedColor));
    } else if (hover && state) {
        setBrush(QBrush(mixColors(stateColor, highlightedColor, 1, 3)));
    } else if (state) {
        setBrush(QBrush(stateColor));
    } else {
        setBrush(QBrush(normalColor));
    }
}

bool RoundaboutTestSegmentItem::getState() const
{
    return state;
}

void RoundaboutTestSegmentItem::setShape(Shape shape)
{
    if (myShape != shape) {
        myShape = shape;
        if (shape == NORMAL) {
            setPath(normalPath);
        } else if (shape == BENT_AT_END) {
            setPath(bentAtEndPath);
        } else {
            // shape == BENT_AT_BEGIN
            setPath(bentAtBeginPath);
        }
    }
}

RoundaboutTestConnectionItem * RoundaboutTestSegmentItem::getConnectionItem()
{
    return connectionItem;
}

void RoundaboutTestSegmentItem::setConnectionItem(RoundaboutTestConnectionItem::Point point, RoundaboutTestConnectionItem *connectionItem)
{
    Q_ASSERT(connectionItem == 0);
    this->connectionItem = connectionItem;
    connectionPoint = point;
    if (connectionPoint == RoundaboutTestConnectionItem::P1) {
        connectionItem->setSegment(connectionPoint, this, mapToScene(bentAtEndAnchor), bentAtEndAngle);
        setShape(BENT_AT_END);
    } else {
        connectionItem->setSegment(connectionPoint, this, mapToScene(bentAtBeginAnchor), bentAtBeginAngle);
        setShape(BENT_AT_BEGIN);
    }
}

void RoundaboutTestSegmentItem::removeConnection()
{
    connectionItem = 0;
    setShape(NORMAL);
}

void RoundaboutTestSegmentItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hover = true;
    setHighlight(highlight);
}

void RoundaboutTestSegmentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hover = false;
    setHighlight(highlight);
}

void RoundaboutTestSegmentItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    state = !state;
    setHighlight(highlight);
}

void RoundaboutTestSegmentItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
        return;
    }
    state = true;
    setHighlight(highlight);
    if (!connectionItem) {
        connectionItem = ((RoundaboutScene*)scene())->createConnectionItem(stateColor, 27);
        setConnectionItem(RoundaboutTestConnectionItem::P1, connectionItem);
        connectionItem->startMove(RoundaboutTestConnectionItem::P2);
    } else {
        connectionItem->startMove(connectionPoint);
    }
}

QVariant RoundaboutTestSegmentItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
    if (connectionItem && (change == QGraphicsItem::ItemScenePositionHasChanged)) {
        if (connectionPoint == RoundaboutTestConnectionItem::P1) {
            connectionItem->setPoint(connectionPoint, mapToScene(bentAtEndAnchor), bentAtEndAngle);
        } else {
            connectionItem->setPoint(connectionPoint, mapToScene(bentAtBeginAnchor), bentAtBeginAngle);
        }
    }
    return QGraphicsPathItem::itemChange(change, value);
}

RoundaboutTestArrowItem::RoundaboutTestArrowItem(QRectF innerRect, QRectF outerRect, qreal angle, qreal tipOffset, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    rect(outerRect),
    normalColor("orangered")
{
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(normalColor));
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
}

RoundaboutTestKeyItem::RoundaboutTestKeyItem(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal arcLength, QColor color, QColor stateColor_, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor(color),
    highlightedColor(Qt::white),
    stateColor(stateColor_),
    lowkeyColor(mixColors(QColor("lightsteelblue"), QColor(Qt::white), 1, 1)),
    state(false),
    hover(false),
    highlight(false),
    lowkey(false)
{
    setPen(QPen(QBrush(Qt::white), 2));
    setBrush(QBrush(normalColor));
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, state);
    setAcceptHoverEvents(true);
    setPath(createSegmentPath(innerRect, outerRect, startAngle, arcLength));
}

void RoundaboutTestKeyItem::setHighlight(bool highlight)
{
    this->highlight = highlight;
    if ((hover && !state) || (highlight && state)) {
        setBrush(QBrush(highlightedColor));
    } else if (hover && state) {
        setBrush(QBrush(mixColors(stateColor, highlightedColor, 1, 3)));
    } else if (state) {
        setBrush(QBrush(stateColor));
    } else {
        setBrush(QBrush(normalColor));
    }
}

void RoundaboutTestKeyItem::setLowkey(bool lowkey)
{
    if (this->lowkey != lowkey) {
        QColor swap = lowkeyColor;
        lowkeyColor = normalColor;
        normalColor = swap;
        setHighlight(highlight);
        this->lowkey = lowkey;
    }
}

void RoundaboutTestKeyItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hover = true;
    setHighlight(highlight);
}

void RoundaboutTestKeyItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hover = false;
    setHighlight(highlight);
}

void RoundaboutTestKeyItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    state = !state;
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, state);
    setHighlight(highlight);
}

RoundaboutTestKeyboardItem::RoundaboutTestKeyboardItem(QRectF innerRect, QRectF outerRect, Direction dir, qreal startAngle, qreal arcLength, QGraphicsItem *parent) :
    QGraphicsPathItem(parent)
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::NoBrush));
    // white keys:
    int keys = 8;
    qreal keyWidth = 1.0 / (qreal)keys;
    for (int i = 0; i < keys; i++) {
        qreal from = (qreal)i / (qreal)keys;
        qreal to = from + keyWidth;
        if (dir == OUTER_TO_INNER) {
            from = 1.0 - from;
            to = 1.0 - to;
        }
        QRectF outerKeyRect = (1.0 - from) * innerRect + from * outerRect;
        QRectF innerKeyRect = (1.0 - to) * innerRect + to * outerRect;
        RoundaboutTestKeyItem *keyItem = new RoundaboutTestKeyItem(innerKeyRect, outerKeyRect, startAngle, arcLength, QColor("lightsteelblue"),QColor("steelblue"), this);
        keyItems.append(keyItem);
    }
    // black keys:
    qreal blackKeyArcLength = 0.6 * arcLength;
    qreal blackKeyWidth = 0.75 * keyWidth;
    QVector<qreal> blackKeys;
    blackKeys.append((3.0 * keyWidth - 2.0 * blackKeyWidth) / 3.0);
    blackKeys.append((3.0 * keyWidth - 2.0 * blackKeyWidth) / 3.0 * 2.0 + blackKeyWidth);
    blackKeys.append(keyWidth * 3.0 + (4.0 * keyWidth - 3.0 * blackKeyWidth) / 4.0);
    blackKeys.append(keyWidth * 3.0 + (4.0 * keyWidth - 3.0 * blackKeyWidth) / 4.0 * 2.0 + blackKeyWidth);
    blackKeys.append(keyWidth * 3.0 + (4.0 * keyWidth - 3.0 * blackKeyWidth) / 4.0 * 3.0 + blackKeyWidth * 2.0);
    for (int i = 0; i < blackKeys.size(); i++) {
        qreal from = blackKeys[i];
        qreal to = from + blackKeyWidth;
        if (dir == OUTER_TO_INNER) {
            from = 1.0 - from;
            to = 1.0 - to;
        }
        QRectF outerKeyRect = (1.0 - from) * innerRect + from * outerRect;
        QRectF innerKeyRect = (1.0 - to) * innerRect + to * outerRect;
        RoundaboutTestKeyItem *keyItem;
        if (dir == INNER_TO_OUTER) {
            keyItem = new RoundaboutTestKeyItem(innerKeyRect, outerKeyRect, startAngle, blackKeyArcLength, QColor("steelblue"), QColor("black"), this);
        } else {
            keyItem = new RoundaboutTestKeyItem(innerKeyRect, outerKeyRect, startAngle + arcLength - blackKeyArcLength, blackKeyArcLength, QColor("steelblue"), QColor("black"), this);
        }
        for (int j = 0; j < keys; j++) {
            keyItems[j]->setPath(keyItems[j]->path() - keyItem->path());
        }
        keyItems.append(keyItem);
    }
}

void RoundaboutTestKeyboardItem::setHighlight(bool highlight)
{
    for (int i = 0; i < keyItems.size(); i++) {
        keyItems[i]->setHighlight(highlight);
    }
}

void RoundaboutTestKeyboardItem::setLowkey(bool lowkey)
{
    for (int i = 0; i < keyItems.size(); i++) {
        keyItems[i]->setLowkey(lowkey);
    }
}

RoundaboutTestSliceItem::RoundaboutTestSliceItem(QRectF innerRect, QRectF outerRect, RoundaboutTestKeyboardItem::Direction dir, qreal startAngle, qreal arcLength, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor(mixColors(QColor("lightsteelblue"), QColor(Qt::white), 1, 1))
{
    Q_ASSERT(outerRect.width() == outerRect.height());
    Q_ASSERT(innerRect.width() == innerRect.height());
    Q_ASSERT(innerRect.center() == outerRect.center());
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(normalColor));
    setAcceptHoverEvents(true);
    setPath(createSegmentPath(innerRect, outerRect, startAngle, arcLength));
    keyboardItem = new RoundaboutTestKeyboardItem(innerRect, 0.85 * outerRect, dir, startAngle, arcLength, this);
    keyboardItem->setLowkey(true);
    segmentItem = new RoundaboutTestSegmentItem(0.85 * outerRect, outerRect, startAngle, arcLength, this);
}

void RoundaboutTestSliceItem::setHighlight(bool highlight)
{
    segmentItem->setHighlight(highlight);
    if (!highlight || segmentItem->getState()) {
        keyboardItem->setHighlight(highlight);
    }
}

RoundaboutTestKeyboardItem * RoundaboutTestSliceItem::getKeyboardItem()
{
    return keyboardItem;
}

RoundaboutTestSegmentItem * RoundaboutTestSliceItem::getSegmentItem()
{
    return segmentItem;
}

void RoundaboutTestSliceItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    keyboardItem->setLowkey(false);
}

void RoundaboutTestSliceItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    keyboardItem->setLowkey(true);
}


RoundaboutTestItem::RoundaboutTestItem(QGraphicsItem *parent) :
    QGraphicsEllipseItem(-200, -200, 400, 400, parent),
    steps(16),
    sliceAngle(360.0 / steps),
    bpm(120)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::NoBrush));
    setAcceptHoverEvents(true);
    QRectF innerRect(-200, -200, 400, 400);
    // create a circle in the center:
    new RoundaboutTestCenterItem(0.25 * innerRect, this);
    // create a play "button":
    RoundaboutTestPlayItem *playItem = new RoundaboutTestPlayItem(0.075 * innerRect, this);
    // create a slice for each step:
    for (int i = 0; i < steps; i++) {
        RoundaboutTestSliceItem *sliceItem = new RoundaboutTestSliceItem(0.25 * innerRect, innerRect, i < steps / 2 ? RoundaboutTestKeyboardItem::INNER_TO_OUTER : RoundaboutTestKeyboardItem::OUTER_TO_INNER, sliceAngle * i - 90, sliceAngle, this);
        sliceItem->getKeyboardItem()->setOpacity(0);
        sliceItems.append(sliceItem);
    }
    // create arrow:
    qreal arrowAngle = 12;
    qreal arrowTipOffset = 5;
    arrowItem = new RoundaboutTestArrowItem(0.85 * innerRect, innerRect, arrowAngle, arrowTipOffset, this);
    arrowItem->setRotation(-90 /*- 0.5 * sliceAngle*/);
    timer.setInterval(50);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    QObject::connect(playItem, SIGNAL(changedState(bool)), this, SLOT(changeState(bool)));
}

RoundaboutTestSegmentItem * RoundaboutTestItem::getSegmentAt(QPointF pos)
{
    qreal angle = -QLineF(QPointF(0, 0), pos).angle() + 90;
    for (; angle < 0; angle += 360);
    int step = (int)(angle / sliceAngle) % steps;
    return sliceItems[step]->getSegmentItem();
}

void RoundaboutTestItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    for (int i = 0; i < sliceItems.size(); i++) {
        sliceItems[i]->getKeyboardItem()->setOpacity(1);
    }
}

void RoundaboutTestItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    for (int i = 0; i < sliceItems.size(); i++) {
        sliceItems[i]->getKeyboardItem()->setOpacity(0);
    }
}

void RoundaboutTestItem::enterStep(int step)
{
    sliceItems[step]->setHighlight(true);
}

void RoundaboutTestItem::leaveStep(int step)
{
    sliceItems[step]->setHighlight(false);
}

void RoundaboutTestItem::changeState(bool play)
{
    qreal angle = arrowItem->rotation() + 90 /*+ 0.5 * sliceAngle*/;
    if (angle < 0) {
        angle += 360;
    }
    int step = (int)(angle / sliceAngle) % steps;
    if (play) {
        if (!timer.isActive()) {
            enterStep(step);
        }
        timer.start();
    } else {
        if (timer.isActive()) {
            leaveStep(step);
        }
        timer.stop();
    }
}

void RoundaboutTestItem::onTimer()
{
    qreal angle = arrowItem->rotation() + 90 /*+ 0.5 * sliceAngle*/;
    if (angle < 0) {
        angle += 360;
    }
    int previousStep = (int)(angle / sliceAngle) % steps;
    angle += (qreal)timer.interval() / 60000.0 * 360.0 / 4.0 * bpm;
    if (angle >= 360) {
        angle -= 360;
    }
    int nextStep = (int)(angle / sliceAngle) % steps;
    arrowItem->setRotation(angle - 90 /*- 0.5 * sliceAngle*/);
    if (previousStep != nextStep) {
        leaveStep(previousStep);
        enterStep(nextStep);
    }
}
