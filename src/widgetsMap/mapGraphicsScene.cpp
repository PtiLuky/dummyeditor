#include "widgetsMap/mapGraphicsScene.hpp"

#include <QGraphicsSceneMouseEvent>

#include "dummyrpg/floor.hpp"
#include "utils/definitions.hpp"
#include "widgets/mapTools.hpp"

namespace Editor {

MapGraphicsScene::MapGraphicsScene(QObject* parent)
    : QGraphicsScene(parent)
{}

MapGraphicsScene::~MapGraphicsScene() {}

const vec_uniq<LayerGraphicItems>& MapGraphicsScene::graphicLayers() const
{
    return m_visibleLayers;
}
const vec_uniq<LayerBlockingItems>& MapGraphicsScene::blockingLayers() const
{
    return m_blockingLayers;
}

void MapGraphicsScene::setMap(const Dummy::Map& map, const std::vector<QPixmap>& chipsets)
{
    // Clear the scene
    clear();

    // Clear the loaded layers
    m_visibleLayers.clear();
    m_blockingLayers.clear();

    int zindex            = 0;
    const size_t nbFloors = map.floors().size();
    for (uint8_t i = 0; i < nbFloors; ++i) {
        instantiateFloor(*map.floorAt(i), chipsets, map.chipsetsUsed(), i, zindex);
    }
}

void MapGraphicsScene::setPreview(const QPixmap& previewPix, const QPoint& pos)
{
    clearPreview();
    m_previewItem = std::make_unique<QGraphicsPixmapItem>(previewPix);
    m_previewItem->setZValue(Z_PREVIEW);
    m_previewItem->setPos(pos);
    addItem(m_previewItem.get());
}

void MapGraphicsScene::setSelectRect(const QRect& selectionRect)
{
    clearSelectRect();

    QBrush brush(QColor(66, 135, 245));

    m_selectionRectItem = std::unique_ptr<QGraphicsRectItem>(addRect(selectionRect));
    m_selectionRectItem->setZValue(Z_SELEC);
    m_selectionRectItem->setBrush(brush);
    m_selectionRectItem->setOpacity(0.5);
}

void MapGraphicsScene::drawGrid(quint16 width, quint16 height, unsigned int unit)
{
    clearGrid();

    QPen pen(QColor(0, 0, 0, 155), 0.5);

    for (int i = 0; i <= width; ++i) {
        QGraphicsItem* item = addLine(i * unit, 0, i * unit, unit * height, pen);
        item->setZValue(Z_GRID);
        m_gridItems.push_back(std::unique_ptr<QGraphicsItem>(item));
    }

    for (int i = 0; i <= height; ++i) {
        QGraphicsItem* item = addLine(0, i * unit, unit * width, unit * i, pen);
        item->setZValue(Z_GRID);
        m_gridItems.push_back(std::unique_ptr<QGraphicsItem>(item));
    }
}
void MapGraphicsScene::updateTilesets(const std::vector<QPixmap>& tilesets,
                                      const std::vector<Dummy::chip_id>& chipsetIds)
{
    for (auto& layerGraph : m_visibleLayers)
        layerGraph->updateTilesets(tilesets, chipsetIds);
    update();
}

void MapGraphicsScene::clear()
{
    clearPreview();
    clearSelectRect();
    clearGrid();
    QGraphicsScene::clear();
}

void MapGraphicsScene::clearPreview()
{
    m_previewItem.reset();
}

void MapGraphicsScene::clearSelectRect()
{
    m_selectionRectItem.reset();
}

void MapGraphicsScene::clearGrid()
{
    m_gridItems.clear();
}

void MapGraphicsScene::instantiateFloor(Dummy::Floor& floor, const std::vector<QPixmap>& chips,
                                        const std::vector<Dummy::chip_id>& chipsetIds, uint8_t floorId, int& zindex)
{
    // Add graphic layers
    const size_t nbFloors = floor.graphicLayers().size();
    for (uint8_t i = 0; i < nbFloors; ++i) {
        ++zindex;
        auto pGraphicLayer =
            std::make_unique<LayerGraphicItems>(floor.graphicLayersAt(i), chips, chipsetIds, floorId, i, zindex);
        addItem(pGraphicLayer->graphicItems());
        m_visibleLayers.push_back(std::move(pGraphicLayer));
    }

    // Add 1 blocking layer
    {
        ++zindex;
        auto pBlockingLayer = std::make_unique<LayerBlockingItems>(floor.blockingLayer(), floorId, 0, zindex);
        addItem(pBlockingLayer->graphicItems());
        m_blockingLayers.push_back(std::move(pBlockingLayer));
    }
}

void MapGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_tools == nullptr || ! e->buttons().testFlag(Qt::LeftButton))
        return;

    m_isUsingTool  = true;
    m_firstClickPt = e->scenePos().toPoint();
    m_tools->previewTool(QRect(m_firstClickPt, m_firstClickPt));
}

void MapGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_tools == nullptr || ! m_isUsingTool)
        return;

    QPoint otherClick = e->scenePos().toPoint();
    m_tools->previewTool(QRect(m_firstClickPt, otherClick));
}

void MapGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_tools == nullptr || ! m_isUsingTool)
        return;

    m_isUsingTool     = false;
    QPoint otherClick = e->scenePos().toPoint();
    m_tools->useTool(QRect(m_firstClickPt, otherClick));
}

QRectF MapGraphicsScene::selectionRect()
{
    if (m_selectionRectItem == nullptr)
        return QRectF();
    else
        return m_selectionRectItem->rect();
}

void MapGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* e)
{
    if (e->modifiers().testFlag(Qt::ControlModifier) && (e->delta() > 0)) {
        emit zooming(eZoom::ZoomIn);
    } else if (e->modifiers().testFlag(Qt::ControlModifier) && (e->delta() < 0)) {
        emit zooming(eZoom::ZoomOut);
    }
}
} // namespace Editor
