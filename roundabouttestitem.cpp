#include "roundabouttestitem.h"
#include <QPen>
#include <QBrush>
#include <QFont>

QRectF operator*(qreal scale, const QRectF &r1)
{
    return QRectF(r1.x() * scale, r1.y() * scale, r1.width() * scale, r1.height() * scale);
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
    QGraphicsEllipseItem *centerItem = new QGraphicsEllipseItem(centerRect, this);
    centerItem->setPen(QPen(Qt::NoPen));
    centerItem->setBrush(QBrush(QColor("steelblue")));
    // create a circle segment for each step:
    QRectF stepOuterRect = rect();
    QRectF stepInnerRect = 0.85 * rect();
    qreal stepAngle = 360.0 / steps;
    qreal gapAngle = 1;
    QColor stepColor("lightsteelblue");
    QColor stepHighlightedColor("steelblue");
    for (unsigned int i = 0; i < steps; i++) {
        QGraphicsPathItem *stepItem = new QGraphicsPathItem(this);
        QPainterPath stepPath;
        stepPath.arcMoveTo(stepInnerRect, -(stepAngle * (i + 1) - gapAngle * 0.5));
        QPointF innerPos = stepPath.currentPosition();
        stepPath.arcMoveTo(stepOuterRect, -(stepAngle * i + gapAngle * 0.5));
        stepPath.arcTo(stepOuterRect, -(stepAngle * i + gapAngle * 0.5), -(stepAngle - gapAngle));
        stepPath.lineTo(innerPos);
        stepPath.arcTo(stepInnerRect, -(stepAngle * (i + 1) - gapAngle * 0.5), stepAngle - gapAngle);
        stepPath.closeSubpath();
        stepItem->setPath(stepPath);
        stepItem->setPen(QPen(Qt::NoPen));
        if (i % 4 == 0) {
            stepItem->setBrush(QBrush(stepHighlightedColor));
        } else {
            stepItem->setBrush(QBrush(stepColor));
        }
    }
//    // add a number in the center:
//    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem("4/4", this);
//    QFont textFont = textItem->font();
//    textItem->setBrush(QBrush(Qt::white));
//    textFont.setPointSize(36);
//    textItem->setFont(textFont);
//    // center it on the center circle:
//    textItem->setPos(-0.5 * textItem->boundingRect().width(), -0.5 * textItem->boundingRect().height());
}
