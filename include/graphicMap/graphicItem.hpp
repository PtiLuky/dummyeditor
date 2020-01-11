#ifndef GRAPHICMAPITEM_H
#define GRAPHICMAPITEM_H

#include <QGraphicsItem>

namespace GraphicMap {

//////////////////////////////////////////////////////////////////////////////
//  BlockingCrossItem class
//////////////////////////////////////////////////////////////////////////////

class GraphicItem : public QGraphicsItem
{
public:
    enum eGraphicItemType
    {
        eBlockingCrossItem,
        eBlockingSquareItem,
        eEventItem
    };

    GraphicItem(eGraphicItemType type);

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*,
                       QWidget* = nullptr) override;

private:
    eGraphicItemType m_type;
};
} // namespace GraphicMap

#endif // GRAPHICMAPITEM_H
