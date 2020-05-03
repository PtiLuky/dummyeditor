#ifndef MAPGRAPHICSSCENE_H
#define MAPGRAPHICSSCENE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <memory>

#include "dummyrpg/map.hpp"
#include "widgetsMap/layerItems.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////
template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

namespace Editor {
class MapTools;
//////////////////////////////////////////////////////////////////////////////
//  MapGraphicsScene class
//////////////////////////////////////////////////////////////////////////////

class MapGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum class eZoom
    {
        ZoomIn,
        ZoomOut,
    };
    explicit MapGraphicsScene(QObject* parent = nullptr);
    virtual ~MapGraphicsScene() override;

    void setMap(const Dummy::Map&, const std::vector<QPixmap>& chipsets);
    void setPreview(const QPixmap& previewPix, const QPoint& pos);
    void setSelectRect(const QRect& selectionRect);
    void drawGrid(quint16 width, quint16 height, unsigned int unit);
    void linkToolSet(MapTools* tools) { m_tools = tools; }
    void updateTilesets(const std::vector<QPixmap>& chipsets, const std::vector<Dummy::chip_id>& chipsetIds);

    QRectF selectionRect();

    void clearPreview();
    void clearSelectRect();
    void clearGrid();

    const vec_uniq<LayerGraphicItems>& graphicLayers() const;
    const vec_uniq<LayerBlockingItems>& blockingLayers() const;

    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    void wheelEvent(QGraphicsSceneWheelEvent*) override;

public slots:
    void clear();

signals:
    void zooming(eZoom);

private:
    void instantiateFloor(Dummy::Floor&, const std::vector<QPixmap>& chips,
                          const std::vector<Dummy::chip_id>& chipsetIds, uint8_t floorId, int& zIdxInOut);

    // Layers
    vec_uniq<LayerGraphicItems> m_visibleLayers;
    vec_uniq<LayerBlockingItems> m_blockingLayers;

    // Tools
    MapTools* m_tools = nullptr;
    QPoint m_firstClickPt;
    bool m_isUsingTool = false;

    // QGraphicsScene deletes those
    vec_uniq<QGraphicsItem> m_gridItems;
    std::unique_ptr<QGraphicsRectItem> m_selectionRectItem;
    std::unique_ptr<QGraphicsPixmapItem> m_previewItem;
};
} // namespace Editor

#endif // MAPGRAPHICSSCENE_H
