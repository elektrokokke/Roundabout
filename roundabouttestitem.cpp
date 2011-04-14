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
    qreal radians = angle * M_PI / 180.0;
    return QPointF(p.x() * cos(radians) - p.y() * sin(radians), p.x() * sin(radians) + p.y() * cos(radians));
}

QColor mix(QColor c1, QColor c2, double weight1, double weight2)
{
    QColor mixed;
    qreal r1, g1, b1, r2, g2, b2;
    c1.getRgbF(&r1, &g1, &b1);
    c2.getRgbF(&r2, &g2, &b2);
    mixed.setRgbF((r1 * weight1 + r2 * weight2) / (weight1 + weight2), (g1 * weight1 + g2 * weight2) / (weight1 + weight2), (b1 * weight1 + b2 * weight2) / (weight1 + weight2));
    return mixed;
}

RoundaboutTestPlayItem::RoundaboutTestPlayItem(QRectF rect, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor("lightsteelblue"),
    hoverColor("steelblue"),
    playingColor("orangered"),
    state(false),
    hover(false)
{
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(normalColor));
    setAcceptHoverEvents(true);
    QPainterPath path(rect.topLeft());
    path.lineTo(rect.right(), rect.center().y());
    path.lineTo(rect.bottomLeft());
    path.closeSubpath();
    setPath(path);
}

void RoundaboutTestPlayItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hover = true;
    setBrush(QBrush(state ? playingColor : hover ? hoverColor : normalColor));
}

void RoundaboutTestPlayItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hover = false;
    setBrush(QBrush(state ? playingColor : hover ? hoverColor : normalColor));
}


void RoundaboutTestPlayItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    state = !state;
    setBrush(QBrush(state ? playingColor : hover ? hoverColor : normalColor));
    changedState(state);
}

RoundaboutTestCenterItem::RoundaboutTestCenterItem(QRectF rect, QGraphicsItem *parent) :
    QGraphicsEllipseItem(rect, parent),
    normalColor("steelblue")
{
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(normalColor));
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

RoundaboutTestSegmentItem::RoundaboutTestSegmentItem(QRectF innerRect_, QRectF outerRect_, qreal angle_, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    innerRect(innerRect_),
    outerRect(outerRect_),
    angle(angle_),
    myShape(BENT_AT_BEGIN),
    normalColor("lightsteelblue"),
    highlightedColor(Qt::white),
    stateColor("steelblue"),
    state(true),
    hover(false),
    highlight(false),
    button(0)
{
    Q_ASSERT(innerRect.width() == innerRect.height());
    Q_ASSERT(outerRect.width() == outerRect.height());
    Q_ASSERT(innerRect.center() == outerRect.center());
    setPen(QPen(QBrush(Qt::white), 3));
    setHighlight(false);
    setAcceptHoverEvents(true);
    setShape(NORMAL);
}

void RoundaboutTestSegmentItem::setHighlight(bool highlight)
{
    this->highlight = highlight;
    if ((hover && !state) || (highlight && state)) {
        setBrush(QBrush(highlightedColor));
    } else if (hover && state) {
        setBrush(QBrush(mix(stateColor, highlightedColor, 1, 3)));
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
            QPainterPath path;
            path.arcMoveTo(innerRect, -angle);
            QPointF innerPos = path.currentPosition();
            path.arcMoveTo(outerRect, 0);
            path.arcTo(outerRect, 0, -angle);
            path.lineTo(innerPos);
            path.arcTo(innerRect, -angle, angle);
            path.closeSubpath();
            setPath(path);
        } else if (shape == BENT_AT_END) {
            QPainterPath path;
            path.arcMoveTo(outerRect, 0);
            QPointF p1 = path.currentPosition();
            path.arcMoveTo(outerRect, -angle);
            QPointF p2 = path.currentPosition();
            QPointF diff = p2 - p1;
            qreal angleBent = 45.0 - angle;
            qreal outerRadius = diff.y() / tan(angleBent * M_PI / 180.0) - diff.x();
            qreal innerRadius = outerRadius - 0.5 * (outerRect.width() - innerRect.width());
            QPointF centerOffset(outerRect.right() - outerRect.center().x() + innerRadius, 0);
            QPointF centerBent = outerRect.center() + centerOffset;
            QRectF innerRectBent(centerBent.x() - innerRadius, centerBent.y() - innerRadius, innerRadius * 2, innerRadius * 2);
            QRectF outerRectBent(centerBent.x() - outerRadius, centerBent.y() - outerRadius, outerRadius * 2, outerRadius * 2);

            path.arcMoveTo(innerRectBent, 180.0 + angleBent);
            QPointF innerPos = path.currentPosition();
            path.arcMoveTo(outerRectBent, 180.0);
            path.arcTo(outerRectBent, 180.0, angleBent);
            path.lineTo(innerPos);
            path.arcTo(innerRectBent, 180.0 + angleBent, -angleBent);
            path.closeSubpath();
            setPath(path);

            path.closeSubpath();
            setPath(path);
        } else {
            // shape == BENT_AT_BEGIN
            QPainterPath path;
            path.arcMoveTo(outerRect, 0);
            QPointF p1 = path.currentPosition();
            path.arcMoveTo(outerRect, -angle);
            QPointF p2 = path.currentPosition();
            QPointF diff = p2 - p1;
            qreal angleBent = 45.0 - angle;
            qreal outerRadius = diff.y() / tan(angleBent * M_PI / 180.0) - diff.x();
            qreal innerRadius = outerRadius - 0.5 * (outerRect.width() - innerRect.width());
            QPointF centerOffset(outerRect.right() - outerRect.center().x() + innerRadius, 0);
            QPointF centerBent = outerRect.center() + ::rotate(centerOffset, angle);
            QRectF innerRectBent(centerBent.x() - innerRadius, centerBent.y() - innerRadius, innerRadius * 2, innerRadius * 2);
            QRectF outerRectBent(centerBent.x() - outerRadius, centerBent.y() - outerRadius, outerRadius * 2, outerRadius * 2);

            path.arcMoveTo(innerRectBent, 180.0 - angleBent);
            QPointF innerPos = path.currentPosition();
            path.arcMoveTo(outerRectBent, 180.0 - 2 * angleBent);
            path.arcTo(outerRectBent, 180.0 - 2 * angleBent, angleBent);
            path.lineTo(innerPos);
            path.arcTo(innerRectBent, 180.0 - angleBent, -angleBent);
            path.closeSubpath();
            setPath(path);

            path.closeSubpath();
            setPath(path);
        }
    }
}

void RoundaboutTestSegmentItem::changeShape()
{
    if (myShape == NORMAL) {
        setShape(BENT_AT_END);
    } else if (myShape == BENT_AT_END) {
        setShape(BENT_AT_BEGIN);
    } else if (myShape == BENT_AT_BEGIN) {
        setShape(NORMAL);
    }
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

void RoundaboutTestSegmentItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    changeShape();
    state = !state;
    setHighlight(highlight);
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

RoundaboutTestKeyItem::RoundaboutTestKeyItem(QRectF innerRect, QRectF outerRect, qreal startAngle, qreal angle, QColor color, QColor stateColor_, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    normalColor(color),
    highlightedColor(Qt::white),
    stateColor(stateColor_),
    state(false),
    hover(false),
    highlight(false)
{
    setPen(QPen(QBrush(Qt::white), 2));
    setBrush(QBrush(normalColor));
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, state);
    setAcceptHoverEvents(true);
    QPainterPath path;
    path.arcMoveTo(innerRect, -(startAngle + angle));
    QPointF innerPos = path.currentPosition();
    path.arcMoveTo(outerRect, -startAngle);
    path.arcTo(outerRect, -startAngle, -angle);
    path.lineTo(innerPos);
    path.arcTo(innerRect, -(startAngle + angle), angle);
    path.closeSubpath();
    setPath(path);
}

void RoundaboutTestKeyItem::setHighlight(bool highlight)
{
    this->highlight = highlight;
    if ((hover && !state) || (highlight && state)) {
        setBrush(QBrush(highlightedColor));
    } else if (hover && state) {
        setBrush(QBrush(mix(stateColor, highlightedColor, 1, 3)));
    } else if (state) {
        setBrush(QBrush(stateColor));
    } else {
        setBrush(QBrush(normalColor));
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

RoundaboutTestKeyboardItem::RoundaboutTestKeyboardItem(QRectF innerRect, QRectF outerRect, Direction dir, qreal angle, QColor color, QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    hover(false),
    baseOpacity(0)
{
    setPen(QPen(QBrush(Qt::white), 3));
    setBrush(QBrush(color));
    setAcceptHoverEvents(true);

    QPainterPath path;
    path.arcMoveTo(innerRect, -angle);
    QPointF innerPos = path.currentPosition();
    path.arcMoveTo(outerRect, 0);
    path.arcTo(outerRect, 0, -angle);
    path.lineTo(innerPos);
    path.arcTo(innerRect, -angle, angle);
    path.closeSubpath();
    setPath(path);

    child = new QGraphicsPathItem(path, this);
    child->setPen(QPen(Qt::NoPen));
    child->setBrush(QBrush(Qt::NoBrush));
    child->setOpacity(baseOpacity);

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
        RoundaboutTestKeyItem *keyItem = new RoundaboutTestKeyItem(innerKeyRect, outerKeyRect, 0, angle, QColor("lightsteelblue"),QColor("steelblue"), child);
        path |= keyItem->path();
        keyItems.append(keyItem);
    }
    // black keys:
    qreal blackKeyAngle = 0.6 * angle;
    qreal blackKeyWidth = 0.8 * keyWidth;
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
            keyItem = new RoundaboutTestKeyItem(innerKeyRect, outerKeyRect, 0, blackKeyAngle, QColor("steelblue"), QColor("black"), child);
        } else {
            keyItem = new RoundaboutTestKeyItem(innerKeyRect, outerKeyRect, angle - blackKeyAngle, blackKeyAngle, QColor("steelblue"), QColor("black"), child);
        }
        for (int j = 0; j < keys; j++) {
            QPainterPath keyPath = keyItems[j]->path() - keyItem->path();
            keyItems[j]->setPath(keyPath);
        }
        path |= keyItem->path();
        keyItems.append(keyItem);
    }
}

void RoundaboutTestKeyboardItem::setHighlight(bool highlight)
{
    for (int i = 0; i < keyItems.size(); i++) {
        keyItems[i]->setHighlight(highlight);
    }
}

void RoundaboutTestKeyboardItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hover = true;
    child->setOpacity(hover ? 1 : baseOpacity);
}

void RoundaboutTestKeyboardItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hover = false;
    child->setOpacity(hover ? 1 : baseOpacity);
}

RoundaboutTestItem::RoundaboutTestItem(QGraphicsItem *parent) :
    QGraphicsEllipseItem(-200, -200, 400, 400, parent),
    steps(16),
    stepAngle(360.0 / steps),
    bpm(120)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(mix(QColor("lightsteelblue"), QColor(Qt::white), 1, 1)));
    // create a circle in the center:
    QRectF centerRect = 0.25 * rect();
    new RoundaboutTestCenterItem(centerRect, this);
    // create a play "button":
    QRectF playRect = 0.3 * centerRect;
    playRect.translate(playRect.width() / 6.0, 0);
    RoundaboutTestPlayItem *playItem = new RoundaboutTestPlayItem(playRect, this);
    // create a circle segment for each step:
    QRectF stepOuterRect = rect();
    QRectF stepInnerRect = 0.85 * rect();
    // create a keyboard for each step:
    QRectF keyboardOuterRect = 0.85 * rect();
    QRectF keyboardInnerRect = 0.25 * rect();
    QColor keybordBackgroundColor(mix(QColor("lightsteelblue"), QColor(Qt::white), 1, 1));
    for (int i = 0; i < steps; i++) {
        RoundaboutTestSegmentItem *segmentItem = new RoundaboutTestSegmentItem(stepInnerRect, stepOuterRect, stepAngle, this);
        segmentItem->setRotation(-90 + stepAngle * i);
        segmentItems.append(segmentItem);
        RoundaboutTestKeyboardItem *keyboardItem;
        if (i < steps / 2) {
            keyboardItem = new RoundaboutTestKeyboardItem(keyboardInnerRect, keyboardOuterRect, RoundaboutTestKeyboardItem::INNER_TO_OUTER, stepAngle, keybordBackgroundColor, this);
        } else {
            keyboardItem = new RoundaboutTestKeyboardItem(keyboardInnerRect, keyboardOuterRect, RoundaboutTestKeyboardItem::OUTER_TO_INNER, stepAngle, keybordBackgroundColor, this);
        }
        keyboardItem->setRotation(-90 + stepAngle * i);
        keyboardItems.append(keyboardItem);
    }
    // create arrow:
    QRectF arrowOuterRect = rect();
    QRectF arrowInnerRect = 0.85 * rect();
    qreal arrowAngle = 12;
    qreal arrowTipOffset = 5;
    arrowItem = new RoundaboutTestArrowItem(arrowInnerRect, arrowOuterRect, arrowAngle, arrowTipOffset, this);
    arrowItem->setRotation(-90);

    timer.setInterval(20);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    QObject::connect(playItem, SIGNAL(changedState(bool)), this, SLOT(changeState(bool)));
}

void RoundaboutTestItem::enterStep(int step)
{
    segmentItems[step]->setHighlight(true);
    if (segmentItems[step]->getState()) {
        keyboardItems[step]->setHighlight(true);
    }
}

void RoundaboutTestItem::leaveStep(int step)
{
    segmentItems[step]->setHighlight(false);
    keyboardItems[step]->setHighlight(false);
}

void RoundaboutTestItem::changeState(bool play)
{
    qreal angle = arrowItem->rotation() + 90;
    if (angle < 0) {
        angle += 360;
    }
    int step = (int)(angle / stepAngle) % steps;
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
    qreal angle = arrowItem->rotation() + 90;
    if (angle < 0) {
        angle += 360;
    }
    int previousStep = (int)(angle / stepAngle) % steps;
    angle += (qreal)timer.interval() / 60000.0 * 360.0 / 4.0 * bpm;
    if (angle >= 360) {
        angle -= 360;
    }
    int nextStep = (int)(angle / stepAngle) % steps;
    arrowItem->setRotation(angle - 90);
    if (previousStep != nextStep) {
        leaveStep(previousStep);
        enterStep(nextStep);
    }
}
