#include "widgets/mapFloorTreeModel.hpp"

#include "dummyrpg/floor.hpp"

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  MapFloorTreeModel class
// This class is the model (data) of the list of all floors of a map
//////////////////////////////////////////////////////////////////////////////

MapFloorTreeModel::MapFloorTreeModel(const Dummy::Map& map)
{
    for (size_t i = map.floors().size(); i > 0; --i) {
        uint8_t uiIdx = static_cast<uint8_t>(i - 1);

        const auto* pFloor = map.floorAt(uiIdx);
        if (pFloor == nullptr)
            continue;

        auto* floorItem = new FloorTreeItem(*this, *pFloor, uiIdx);
        appendRow(floorItem);
    }
}

MapTreeItem* MapFloorTreeModel::floorItemFromIdx(const QModelIndex& index) const
{
    return dynamic_cast<MapTreeItem*>(itemFromIndex(index));
}

bool MapTreeItem::isVisible() const
{
    return m_isVisible;
}

void MapTreeItem::setVisibilityIcon(bool isVisible)
{
    setEnabled(isVisible);
    if (isVisible) {
        setIcon(QIcon(":/icons/icon_eye.png"));
    } else {
        setIcon(QIcon(":/icons/icon_eye_crossed.png"));
    }
    m_isVisible = isVisible;
}

//////////////////////////////////////////////////////////////////////////////
//  MapFloorTreeItem class
// This class is the model (data) of a floor (containing several layers)
//////////////////////////////////////////////////////////////////////////////

FloorTreeItem::FloorTreeItem(MapFloorTreeModel& parent, const Dummy::Floor& floor, uint8_t floorIdx)
    : m_index(floorIdx)
{
    setVisibilityIcon(true);

    // Put blocking layer at the top.
    appendRow(new LayerTreeItem(parent, eLayerType::Blocking, floorIdx));

    // Then put other layers
    const uint8_t nbFloors = static_cast<uint8_t>(floor.graphicLayers().size());
    for (uint8_t i = 0; i < nbFloors; ++i) {
        appendRow(new LayerTreeItem(parent, eLayerType::Graphic, floorIdx, i));
    }
}

QVariant FloorTreeItem::data(int role) const
{
    if (role == Qt::DisplayRole)
        return QObject::tr("Floor %1").arg(m_index);
    return QStandardItem::data(role);
}

void FloorTreeItem::toggle()
{
    setVisible(! isVisible());
}

void FloorTreeItem::setVisible(bool visible)
{
    // update visibility icon
    setVisibilityIcon(visible);

    // propagate to children
    int nbRows = rowCount();
    for (int i = 0; i < nbRows; ++i) {
        auto* layerItem = reinterpret_cast<LayerTreeItem*>(child(i));
        layerItem->setVisible(visible);
    }
}

void FloorTreeItem::setSelected()
{
    // Nothing to do.
}

//////////////////////////////////////////////////////////////////////////////
//  MapLayerTreeItem class.
// This class is the model (data) of a layer
//////////////////////////////////////////////////////////////////////////////

LayerTreeItem::LayerTreeItem(MapFloorTreeModel& parent, eLayerType type, uint8_t floorIdx, uint8_t layerIdx)
    : m_parent(parent)
    , m_type(type)
    , m_floorIdx(floorIdx)
    , m_layerIdx(layerIdx)
{
    setVisibilityIcon(true);
}

QVariant LayerTreeItem::data(int role) const
{
    if (role != Qt::DisplayRole) {
        return QStandardItem::data(role);
    }

    if (m_type == eLayerType::Graphic) {
        return QObject::tr("Layer %1").arg(m_layerIdx);
    } else if (m_type == eLayerType::Blocking) {
        return QObject::tr("Blocking layer");
    }

    return QObject::tr("Unknown layer");
}


void LayerTreeItem::toggle()
{
    setVisible(! isVisible());
}

void LayerTreeItem::setVisible(bool visible)
{
    setVisibilityIcon(visible);
    emit m_parent.layerVisibilityChanged(visible, m_type, m_floorIdx, m_layerIdx);
}

void LayerTreeItem::setSelected()
{
    emit m_parent.activeLayerChanged(m_type, m_floorIdx, m_layerIdx);
}

} // namespace Editor
