#include "widgetsMap/layerItems.hpp"

#include <QGraphicsItem>

#include "widgetsMap/graphicItem.hpp"

namespace Editor {

MapSceneLayer::MapSceneLayer(uint8_t floorIdx, uint8_t layerIdx, int zIndex)
    : m_floorIdx(floorIdx)
    , m_layerIdx(layerIdx)
{
    m_items->setZValue(zIndex);
}

void MapSceneLayer::clear()
{
    for (auto* item : m_items->childItems())
        m_items->removeFromGroup(item);

    m_indexedItems.clear();
}

void MapSceneLayer::setVisibility(bool visible)
{
    m_items->setVisible(visible);
}

bool MapSceneLayer::isThisFloor(uint8_t floorIdx) const
{
    return m_floorIdx == floorIdx;
}

bool MapSceneLayer::isThisLayer(uint8_t floorIdx, uint8_t layerIdx) const
{
    return (m_floorIdx == floorIdx) && (m_layerIdx == layerIdx);
}

//////////////////////////////////////////////////////////////////////////////

LayerGraphicItems::LayerGraphicItems(Dummy::GraphicLayer& layer, const std::vector<QPixmap>& chipsets,
                                     const std::vector<Dummy::chip_id>& chipsetIds, uint8_t floorIdx, uint8_t layerIdx,
                                     int zIndex)
    : MapSceneLayer(floorIdx, layerIdx, zIndex)
    , m_graphicLayer(layer)
    , m_chipsets(chipsets)
    , m_chipsetIds(chipsetIds)
{
    const size_t nbCells = layer.size();
    indexedItems().resize(nbCells);

    uint16_t w = m_graphicLayer.width();
    uint16_t h = m_graphicLayer.height();
    for (uint16_t x = 0; x < w; ++x)
        for (uint16_t y = 0; y < h; ++y) {
            Dummy::Coord coord {x, y};
            setTile(coord, m_graphicLayer.at(coord));
        }
}

void LayerGraphicItems::setTile(Dummy::Coord coord, Dummy::Tileaspect aspect)
{
    if (coord.x > m_graphicLayer.width() || coord.y > m_graphicLayer.height())
        return;

    size_t index = (coord.y * m_graphicLayer.width()) + coord.x;

    if (nullptr != indexedItems()[index]) {
        delete indexedItems()[index];
        indexedItems()[index] = nullptr;
    }

    size_t idxOfChip = idxOfId(aspect.chipId);
    if (aspect == Dummy::undefAspect || idxOfChip >= m_chipsets.size()) {
        m_graphicLayer.set(coord, Dummy::undefAspect);
    } else {
        const QPixmap& chip = m_chipsets[idxOfChip];
        indexedItems()[index] =
            new QGraphicsPixmapItem(chip.copy(QRect(aspect.x * CELL_W, aspect.y * CELL_H, CELL_W, CELL_H)));
        indexedItems()[index]->setPos(coord.x * CELL_W, coord.y * CELL_H);
        graphicItems()->addToGroup(indexedItems()[index]);

        m_graphicLayer.set(coord, aspect);
    }
}

void LayerGraphicItems::updateTilesets(const std::vector<QPixmap>& chipsets,
                                       const std::vector<Dummy::chip_id>& chipsetIds)
{
    m_chipsets   = chipsets;
    m_chipsetIds = chipsetIds;

    uint16_t w = m_graphicLayer.width();
    uint16_t h = m_graphicLayer.height();
    for (uint16_t x = 0; x < w; ++x)
        for (uint16_t y = 0; y < h; ++y) {
            Dummy::Coord coord {x, y};
            setTile(coord, m_graphicLayer.at(coord));
        }
}

const Dummy::GraphicLayer& LayerGraphicItems::layer()
{
    return m_graphicLayer;
}

size_t LayerGraphicItems::idxOfId(Dummy::chip_id id)
{
    const size_t nbOfChips = m_chipsetIds.size();
    for (size_t i = 0; i < nbOfChips; ++i)
        if (id == m_chipsetIds[i])
            return i;

    return static_cast<size_t>(-1);
}

//////////////////////////////////////////////////////////////////////////////

LayerBlockingItems::LayerBlockingItems(Dummy::BlockingLayer& layer, uint8_t floorIdx, uint8_t layerIdx, int zIndex)
    : MapSceneLayer(floorIdx, layerIdx, zIndex)
    , m_blockingLayer(layer)
{
    const size_t nbCells = layer.size();
    indexedItems().resize(nbCells);

    uint16_t w = layer.width();
    uint16_t h = layer.height();

    for (uint16_t x = 0; x < w; ++x)
        for (uint16_t y = 0; y < h; ++y)
            if (m_blockingLayer.at({x, y}) != 0)
                setTile({x, y}, true);
}

void LayerBlockingItems::toogleTile(Dummy::Coord coord)
{
    if (coord.x > m_blockingLayer.width() || coord.y > m_blockingLayer.height())
        return;

    if (m_blockingLayer.at(coord) != 0) {
        setTile(coord, false);
    } else {
        setTile(coord, true);
    }
}

void LayerBlockingItems::setTile(Dummy::Coord coord, bool isBlock)
{
    if (coord.x > m_blockingLayer.width() || coord.y > m_blockingLayer.height())
        return;

    size_t index((coord.y * m_blockingLayer.width()) + coord.x);

    if (indexedItems()[index] != nullptr) {
        delete indexedItems()[index];
        indexedItems()[index] = nullptr;
    }

    if (isBlock) {
        indexedItems()[index] = new GraphicItem(GraphicItem::eGraphicItemType::BlockingSquare);
        indexedItems()[index]->setPos(QPointF(coord.x * CELL_W, coord.y * CELL_H));
        graphicItems()->addToGroup(indexedItems()[index]);
    }

    m_blockingLayer.set(coord, isBlock);
}

const Dummy::BlockingLayer& LayerBlockingItems::layer()
{
    return m_blockingLayer;
}

//////////////////////////////////////////////////////////////////////////////

LayerObjectItems::LayerObjectItems(Dummy::Floor& floor, int zIndex)
    : MapSceneLayer(0, 0, zIndex)
    , m_floor(floor)
{
    update();
}

void LayerObjectItems::update()
{
    clear();
    for (auto& chara : m_floor.npcs()) {
        addChar(chara.characterId(), chara.pos().coord);
    }
}

void LayerObjectItems::addChar(Dummy::char_id, const Dummy::Coord& coord)
{
    indexedItems().push_back(new GraphicItem(GraphicItem::eGraphicItemType::Character));
    indexedItems().back()->setPos(QPointF(coord.x * CELL_W, coord.y * CELL_H));
    graphicItems()->addToGroup(indexedItems().back());
}

Dummy::Floor& LayerObjectItems::floor()
{
    return m_floor;
}

} // namespace Editor
;
