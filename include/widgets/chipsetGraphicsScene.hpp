#ifndef CHIPSETGRAPHICSCENE_H
#define CHIPSETGRAPHICSCENE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <memory>

#include "dummyrpg/dummy_types.hpp"

//////////////////////////////////////////////////////////////////////////////
//  ChipsetGraphicsScene class
//////////////////////////////////////////////////////////////////////////////

class ChipsetGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit ChipsetGraphicsScene(QObject* parent = nullptr);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    const std::vector<QPixmap> chipsets() const { return {m_chipset}; }
    const QRect& selectionRect() const { return m_currentSelection; }
    Dummy::chip_id currId() const { return m_currId; }
    QPixmap selectionPixmap() const;

    void setChipset(const std::vector<QString>& chipsetPaths, const std::vector<Dummy::chip_id>& chipsetIds);

public slots:
    void clear();
    void setGridVisible(bool visible = true);

private:
    void drawGrid();
    void setSelectRect(const QRect&);

private:
    QPixmap m_chipset;
    Dummy::chip_id m_currId = 0;
    std::unique_ptr<QGraphicsRectItem> m_selectionRectItem;
    std::vector<std::unique_ptr<QGraphicsItem>> m_gridItems;
    bool m_isSelecting = false;
    QRect m_currentSelection;
    QPoint m_selectionStart;
};

#endif // CHIPSETGRAPHICSCENE_H
