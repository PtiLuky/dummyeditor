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
    case eGraphicItemType::eBlockingCross:
    case eGraphicItemType::eBlockingSquare:
    case eGraphicItemType::eEvent:
        return QRectF(0, 0, CELL_W, CELL_H);
    default:
        return QRectF(0, 0, 0, 0);
    }
}

void GraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    switch (m_type) {
    case eGraphicItemType::eBlockingCross:
        painter->setBrush(Qt::black);
        painter->drawLine(0, 0, CELL_W, CELL_W);
        painter->drawLine(0, CELL_W, CELL_W, 0);
        break;
    case eGraphicItemType::eBlockingSquare:
        painter->fillRect(QRect(0, 0, CELL_W, CELL_W), QColor(255, 0, 0, 100));
        break;
    case eGraphicItemType::eEvent:
        painter->fillRect(QRect(0, 0, CELL_W, CELL_H), QColor(255, 255, 255, 100));
        break;
    default:
        break;
    }
}
} // namespace Editor
