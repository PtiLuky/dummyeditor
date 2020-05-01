#ifndef MAPFLOORTREEMODEL_H
#define MAPFLOORTREEMODEL_H

#include <QStandardItemModel>
#include <memory>

#include "dummyrpg/map.hpp"
#include "utils/definitions.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  MapTreeItem interface
//////////////////////////////////////////////////////////////////////////////

class MapTreeItem : public QStandardItem
{
public:
    virtual void toggle()         = 0;
    virtual void setVisible(bool) = 0;
    virtual void setSelected()    = 0;

protected:
    void setVisibilityIcon(bool isVisible);
    bool isVisible() const;

private:
    bool m_isVisible = true;
};

//////////////////////////////////////////////////////////////////////////////
//  MapFloorTreeModel class
// This class is the model (data) of the list of all floors of a map
//////////////////////////////////////////////////////////////////////////////

class MapFloorTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit MapFloorTreeModel(const Dummy::Map&);
    MapTreeItem* floorItemFromIdx(const QModelIndex&) const;

signals:
    void layerVisibilityChanged(bool newVisibility, eLayerType type, uint8_t floorIdx, uint8_t layerIdx);
    void activeLayerChanged(eLayerType type, uint8_t floorIdx, uint8_t layerIdx);
};

//////////////////////////////////////////////////////////////////////////////
//  MapFloorTreeItem class
// This class is the model (data) of a floor (containing several layers)
//////////////////////////////////////////////////////////////////////////////

class FloorTreeItem : public MapTreeItem
{
public:
    explicit FloorTreeItem(MapFloorTreeModel& parent, const Dummy::Floor&, uint8_t floorIdx);

    QVariant data(int role = Qt::UserRole + 1) const override;

    void toggle() override;
    void setVisible(bool) override;
    void setSelected() override;

private:
    std::size_t m_index;
};

//////////////////////////////////////////////////////////////////////////////
//  MapLayerTreeItem class.
// This class is the model (data) of a layer
//////////////////////////////////////////////////////////////////////////////

class LayerTreeItem : public MapTreeItem
{
public:
    LayerTreeItem(MapFloorTreeModel& parent, eLayerType type, uint8_t floorIdx, uint8_t layerIdx = 0);

    QVariant data(int role = Qt::UserRole + 1) const override;

    void toggle() override;
    void setVisible(bool isVisible) override;
    void setSelected() override;

private:
    MapFloorTreeModel& m_parent;
    eLayerType m_type;
    uint8_t m_floorIdx;
    uint8_t m_layerIdx;
};

} // namespace Editor

#endif // MAPFLOORTREEMODEL_H
