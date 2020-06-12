#ifndef MAPGRAPHICSSCENE_H
#define MAPGRAPHICSSCENE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <memory>

#include "dummyrpg/map.hpp"
#include "editor/project.hpp"
#include "widgetsMap/graphicItem.hpp"
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

    void setMap(std::shared_ptr<Project> p, const Dummy::Map&, const std::vector<QPixmap>& chipsets);
    void setCurrFloor(uint8_t); // doesn't visually set the layer but only prepare the link for "add charac" action
    void setPreview(const QPixmap& previewPix, const QPoint& pos);
    void setSelectRect(const QRect& selectionRect);
    void setLocationCharacter(const QPoint&, Dummy::char_id);
    void drawGrid(quint16 width, quint16 height, unsigned int unit);
    void linkToolSet(MapTools* tools) { m_tools = tools; }
    void updateTilesets(const std::vector<QPixmap>& chipsets, const std::vector<Dummy::chip_id>& chipsetIds);

    QRectF selectionRect();

    void clearPreview();
    void clearLocationIndicator();
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
    void characterPlacedOnFloor(Dummy::char_id, Dummy::Coord, uint8_t floor);

private:
    void instantiateFloor(Dummy::Floor&, const std::vector<QPixmap>& chips,
                          const std::vector<Dummy::chip_id>& chipsetIds, uint8_t floorId, int& zIdxInOut);
    Dummy::Coord scenePosToCoord(const QPoint& p) const;
    Dummy::CharacterInstance* npcAt(Dummy::Coord);

    // Layers
    vec_uniq<LayerGraphicItems> m_visibleLayers;
    vec_uniq<LayerBlockingItems> m_blockingLayers;
    vec_uniq<LayerObjectItems> m_objectsLayers;

    // Tools
    enum class eMode
    {
        None,
        Tool,
        AddChar
    };
    MapTools* m_tools = nullptr;
    QPoint m_firstClickPt;
    eMode m_toolMode                = eMode::None;
    Dummy::char_id m_charBeingAdded = Dummy::undefChar;
    uint8_t m_activeFloor           = 0;
    std::shared_ptr<Project> m_loadedProject;

    // QGraphicsScene deletes those
    vec_uniq<QGraphicsItem> m_gridItems;
    std::unique_ptr<QGraphicsRectItem> m_selectionRectItem; // when seleting tiles
    std::unique_ptr<QGraphicsPixmapItem> m_previewTileItem; // when drawing tiles
    std::unique_ptr<GraphicItem> m_locationIndicatorItem;   // when placing a character or item
};
} // namespace Editor

#endif // MAPGRAPHICSSCENE_H
