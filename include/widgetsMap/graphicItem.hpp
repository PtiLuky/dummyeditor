#ifndef GRAPHICMAPITEM_H
#define GRAPHICMAPITEM_H

#include <QGraphicsItem>

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  BlockingCrossItem class
//////////////////////////////////////////////////////////////////////////////

class GraphicItem : public QGraphicsItem
{
public:
    enum class eGraphicItemType
    {
        BlockingSquare,
        Event,
        Character,
        Cell,
    };

    explicit GraphicItem(eGraphicItemType type);

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    eGraphicItemType m_type;
};
} // namespace Editor
#endif // GRAPHICMAPITEM_H
