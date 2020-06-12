#include "widgetsMap/mapGraphicsScene.hpp"

#include <QGraphicsSceneMouseEvent>

#include "dummyrpg/floor.hpp"
#include "utils/definitions.hpp"
#include "widgets/characterInstanceWidget.hpp"
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

void MapGraphicsScene::setMap(std::shared_ptr<Project> p, const Dummy::Map& map, const std::vector<QPixmap>& chipsets)
{
    // Clear the scene
    clear();

    // Clear the loaded layers
    m_visibleLayers.clear();
    m_blockingLayers.clear();
    m_objectsLayers.clear();
    m_loadedProject = p;

    int zindex            = 0;
    const size_t nbFloors = map.floors().size();
    for (uint8_t i = 0; i < nbFloors; ++i) {
        instantiateFloor(*map.floorAt(i), chipsets, map.chipsetsUsed(), i, zindex);
    }
    setCurrFloor(0);
}

void MapGraphicsScene::setCurrFloor(uint8_t id)
{
    m_activeFloor = id;
}

void MapGraphicsScene::setPreview(const QPixmap& previewPix, const QPoint& pos)
{
    m_previewTileItem = std::make_unique<QGraphicsPixmapItem>(previewPix);
    m_previewTileItem->setZValue(Z_PREVIEW);
    m_previewTileItem->setPos(pos);
    addItem(m_previewTileItem.get());
}
void MapGraphicsScene::setLocationCharacter(const QPoint& p, Dummy::char_id id)
{
    m_locationIndicatorItem = std::make_unique<GraphicItem>(GraphicItem::eGraphicItemType::Cell);
    m_locationIndicatorItem->setZValue(Z_PREVIEW);
    m_locationIndicatorItem->setPos(p);
    addItem(m_locationIndicatorItem.get());
    m_toolMode       = eMode::AddChar;
    m_charBeingAdded = id;
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
    for (const auto& layerGraph : m_visibleLayers)
        layerGraph->updateTilesets(tilesets, chipsetIds);
    update();
}

void MapGraphicsScene::clear()
{
    clearPreview();
    clearLocationIndicator();
    clearSelectRect();
    clearGrid();
    QGraphicsScene::clear();
}
void MapGraphicsScene::clearPreview()
{
    m_previewTileItem.reset();
}
void MapGraphicsScene::clearLocationIndicator()
{
    m_locationIndicatorItem.reset();
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

    // Add 1 objects/event layer
    {
        ++zindex;
        auto pObjectsLayer = std::make_unique<LayerObjectItems>(floor, zindex);
        addItem(pObjectsLayer->graphicItems());
        m_objectsLayers.push_back(std::move(pObjectsLayer));
    }
}

Dummy::Coord MapGraphicsScene::scenePosToCoord(const QPoint& p) const
{
    if (m_visibleLayers.size() == 0)
        return {0, 0};

    int x   = p.x() / CELL_W;
    int y   = p.y() / CELL_H;
    auto& l = m_visibleLayers[0]->layer();

    x = std::min(std::max(x, 0), l.width() - 1);
    y = std::min(std::max(y, 0), l.height() - 1);
    return {static_cast<uint16_t>(x), static_cast<uint16_t>(y)};
}

Dummy::CharacterInstance* MapGraphicsScene::npcAt(Dummy::Coord coord)
{
    size_t nbLayers = m_objectsLayers.size();
    for (size_t i = 0; i < nbLayers; ++i) {
        size_t nbNpc = m_objectsLayers[i]->floor().npcs().size();
        for (size_t j = 0; j < nbNpc; ++j) {
            auto& chara = m_objectsLayers[i]->floor().npc(static_cast<Dummy::char_id>(j));
            if (chara.pos().coord == coord)
                return &chara;
        }
    }
    return nullptr;
}

void MapGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_toolMode == eMode::AddChar) {
        // Nothing do to, action is on release
    }
    // Selecting an npc or item ?
    else if (e->button() == Qt::RightButton) {
        Dummy::Coord mouseCoord     = scenePosToCoord(e->scenePos().toPoint());
        Dummy::CharacterInstance* c = npcAt(mouseCoord);

        if (c != nullptr) {
            CharacterInstanceWidget editCharDialog(m_loadedProject, m_objectsLayers[0]->floor(), *c, nullptr);
            editCharDialog.exec();
            for (const auto& objLay : m_objectsLayers)
                objLay->update();
        }
    } else if (m_tools != nullptr && e->button() == Qt::LeftButton) {
        m_toolMode     = eMode::Tool;
        m_firstClickPt = e->scenePos().toPoint();
        m_tools->previewTool(QRect(m_firstClickPt, m_firstClickPt));
    }
}

void MapGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_toolMode == eMode::AddChar && m_locationIndicatorItem != nullptr) {
        Dummy::Coord mouseCoord = scenePosToCoord(e->scenePos().toPoint());
        m_locationIndicatorItem->setPos(mouseCoord.x * CELL_W, mouseCoord.y * CELL_H);
    } else if (m_toolMode == eMode::Tool && m_tools != nullptr) {
        QPoint otherClick = e->scenePos().toPoint();
        m_tools->previewTool(QRect(m_firstClickPt, otherClick));
    }
}

void MapGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    // Setting position of a character
    if (m_toolMode == eMode::AddChar && m_locationIndicatorItem != nullptr && e->button() == Qt::LeftButton) {
        Dummy::Coord mouseCoord = scenePosToCoord(e->scenePos().toPoint());
        if (npcAt(mouseCoord) != nullptr)
            return; // don't exit "Add char" mode

        emit characterPlacedOnFloor(m_charBeingAdded, mouseCoord, m_activeFloor);
        m_objectsLayers[m_activeFloor]->addChar(m_charBeingAdded, mouseCoord);
        clearLocationIndicator();
    }
    // using a map-drawing tool
    else if (m_toolMode == eMode::Tool && m_tools != nullptr) {
        QPoint otherClick = e->scenePos().toPoint();
        m_tools->useTool(QRect(m_firstClickPt, otherClick));
    }
    m_toolMode = eMode::None;
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
