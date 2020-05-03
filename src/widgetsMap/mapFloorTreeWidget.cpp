#include "widgetsMap/mapFloorTreeWidget.hpp"
#include "ui_mapFloorTreeWidget.h"

namespace Editor {

FloorListWidget::FloorListWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::MapFloorsList)
{
    m_ui->setupUi(this);
}
FloorListWidget::~FloorListWidget() {}

void FloorListWidget::setEditorMap(const Dummy::Map& map)
{
    m_floorTreeModel.reset(new MapFloorTreeModel(map));
    m_ui->treeViewFloors->setModel(m_floorTreeModel.get());
}

void FloorListWidget::reset()
{
    m_floorTreeModel.reset();
    m_ui->treeViewFloors->setModel(new QStandardItemModel());
}

void FloorListWidget::selectFirstVisLayer()
{
    // Select first layer first floor
    auto firstFloorIdx = m_floorTreeModel->index(0, 0);
    m_ui->treeViewFloors->expand(firstFloorIdx);
    auto firstVisLayerIdx = m_floorTreeModel->index(1, 0, firstFloorIdx);
    m_ui->treeViewFloors->setCurrentIndex(firstVisLayerIdx);
    on_treeViewFloors_clicked(firstVisLayerIdx); // trigger signal manually...
}

MapFloorTreeModel* FloorListWidget::model() const
{
    return m_floorTreeModel.get();
}

void FloorListWidget::on_treeViewFloors_doubleClicked(const QModelIndex& idx)
{
    m_floorTreeModel->floorItemFromIdx(idx)->toggle();
}

void FloorListWidget::on_treeViewFloors_clicked(const QModelIndex& idx)
{
    if (m_floorTreeModel == nullptr)
        return;

    auto* itemSelected = m_floorTreeModel->floorItemFromIdx(idx);
    if (m_floorTreeModel == nullptr)
        return;

    itemSelected->setSelected(); // This line will trigger a signal
}

} // namespace Editor
