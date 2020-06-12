#include "widgetsMap/graphicItem.hpp"

#include <QPainter>

#include "utils/definitions.hpp"

namespace Editor {

GraphicItem::GraphicItem(eGraphicItemType type)
    : m_type(type)
{}

QRectF GraphicItem::boundingRect() const
{
    switch (m_type) {
    case eGraphicItemType::BlockingSquare:
    case eGraphicItemType::Event:
    case eGraphicItemType::Character:
    case eGraphicItemType::Cell:
        return QRectF(0, 0, CELL_W, CELL_H);
    default:
        return QRectF(0, 0, 0, 0);
    }
}

void GraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const int w   = CELL_W;
    const int h   = CELL_H;
    const int w2  = w / 2; // half
    const int h2  = h / 2;
    const int w4  = w / 4; // quarter
    const int h4  = h / 4;
    const int w34 = w * 3 / 4; // 3 quarter
    const int h34 = h * 3 / 4;
    switch (m_type) {
    case eGraphicItemType::BlockingSquare:
        painter->fillRect(QRect(0, 0, w, h), QColor(255, 0, 0, 100));
        break;
    case eGraphicItemType::Event:
        painter->fillRect(QRect(0, 0, w, h), QColor(255, 255, 255, 100));
        break;
    case eGraphicItemType::Character:
        painter->setPen(QPen(Qt::black, 0.5));
        painter->setBrush(QColor(0, 255, 255, 100));
        painter->drawEllipse(QRect(w4, h4, w2, h2));
        painter->drawLine(w4, h2, w34, h2);
        painter->drawLine(w2, h4, w2, h34);
        break;
    case eGraphicItemType::Cell:
        painter->setPen(QPen(Qt::black, 0.5));
        painter->fillRect(QRect(0, 0, w, h), QColor(0, 255, 255, 100));
        painter->drawLine(0, 0, w, h);
        painter->drawLine(0, h, w, 0);
        break;
    default:
        break;
    }
}
} // namespace Editor
