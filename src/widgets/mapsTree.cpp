#include "widgets/mapsTree.hpp"

#include <QMenu>
#include <QMessageBox>

#include "utils/logger.hpp"

namespace Editor {

MapsTreeView::MapsTreeView(QWidget* parent)
    : QTreeView(parent)
    , m_mapMenu(new QMenu(this))
    , m_newMapAction(new QAction(tr("Add new map"), this))
    , m_editAction(new QAction(tr("Properties"), this))
    , m_newMapDialog(new MapEditDialog(this))
    , m_editDialog(new MapEditDialog(this))
{
    // add actions to menu
    m_mapMenu->addAction(m_newMapAction);
    m_mapMenu->addAction(m_editAction);
    // connect actions
    connect(m_newMapAction, &QAction::triggered, this, &MapsTreeView::showNewMapDlg);
    connect(m_editAction, &QAction::triggered, this, &MapsTreeView::showEditDlg);
    // connect menu pop-up
    connect(this, &QWidget::customContextMenuRequested, this, &MapsTreeView::showContextMenu);
    m_mapMenu->setEnabled(false);

    // connect new map dialog
    connect(m_newMapDialog, &QDialog::finished, this, &MapsTreeView::createMap);
    // connect edit dialog
    connect(m_editDialog, &QDialog::finished, this, &MapsTreeView::editMap);
}

void MapsTreeView::setProject(std::shared_ptr<Project> project)
{
    m_project = project;

    if (project != nullptr) {
        setModel(project->mapsModel());
        m_mapMenu->setEnabled(true);
    } else {
        setModel(nullptr);
        m_mapMenu->setEnabled(false);
    }
}

void MapsTreeView::clear()
{
    setProject(nullptr);
}

void MapsTreeView::showContextMenu(const QPoint& point)
{
    m_selectedIndex = indexAt(point);

    // Activate "Properties" Action if we clicked on an item
    bool clickedOnItem = m_selectedIndex.isValid();
    m_editAction->setEnabled(clickedOnItem);

    // map names should be modified only though properties and not by clicking on the map twice slowly.

    // And finally show the contextual menu
    m_mapMenu->exec(viewport()->mapToGlobal(point));
}

void MapsTreeView::showNewMapDlg()
{
    if (m_project == nullptr)
        return; // not supposed to happen

    m_newMapDialog->setup(*m_project, nullptr, tr("New map"));
    m_newMapDialog->open();
}

void MapsTreeView::addMapAtRoot()
{
    m_selectedIndex = QModelIndex();
    showNewMapDlg();
}

void MapsTreeView::createMap(int result)
{
    if (result != QDialog::Accepted)
        return;

    const auto* mapsModel = qobject_cast<QStandardItemModel*>(model());
    if (mapsModel == nullptr)
        return;

    auto* selectedParentMap = mapsModel->invisibleRootItem();
    if (m_selectedIndex.isValid()) {
        selectedParentMap = mapsModel->itemFromIndex(m_selectedIndex);
    }

    if (m_project == nullptr || selectedParentMap == nullptr)
        return;

    tMapInfo mapInfo;
    QString mapName = Project::sanitizeMapName(m_newMapDialog->getMapName());

    if (m_project->mapExists(mapName)) {
        Log::error(tr("A map with this name already exists (%1)").arg(mapName));
        return;
    }

    mapInfo.m_mapName     = mapName.toStdString();
    mapInfo.m_chispetPath = m_newMapDialog->getChipset().toStdString();
    mapInfo.m_musicPath   = m_newMapDialog->getMusic().toStdString();
    mapInfo.m_width       = m_newMapDialog->getWidth();
    mapInfo.m_height      = m_newMapDialog->getHeight();

    m_project->createMap(mapInfo, *selectedParentMap);

    expand(m_selectedIndex);

    emit mapChanged(mapName);
}

void MapsTreeView::editMap(int result)
{
    if (result != QDialog::Accepted)
        return;

    auto* map = m_project->currMap();
    if (map == nullptr)
        return;

    m_project->renameCurrMap(m_editDialog->getMapName());

    uint16_t w = m_editDialog->getWidth();
    uint16_t h = m_editDialog->getHeight();
    if (w < map->width() || h < map->height()) {
        auto btn = QMessageBox::question(this, tr("Resize?"),
                                         tr("You are about to REDUCE map size, some data will be lost. Continue?"));
        if (btn == QMessageBox::No)
            return;
    }

    if (w != map->width() || h != map->height()) {
        map->resize(w, h);
    }

    emit mapChanged(m_editDialog->getMapName());
}

void MapsTreeView::showEditDlg()
{
    const QString mapName = m_project->mapsModel()->itemFromIndex(m_selectedIndex)->text();
    m_project->loadMap(mapName);
    const auto* map = m_project->currMap();

    m_editDialog->setup(*m_project, map, mapName);
    m_editDialog->open();
}

///////////////////////////////////////////////////////////////////////////////


MapsTreeModel::MapsTreeModel(const QDomNode& mapsNode)
{
    XmlMapToQItem(mapsNode, invisibleRootItem());
}

void MapsTreeModel::XmlMapToQItem(const QDomNode& node, QStandardItem* parent)
{
    const auto& children = node.childNodes();
    int nbChildren       = children.count();

    for (int i = 0; i < nbChildren; ++i) {
        const auto& n = children.at(i);

        if (n.nodeName() == "map") {
            QString mapName        = n.attributes().namedItem("name").nodeValue();
            QStandardItem* mapItem = new QStandardItem(mapName);
            parent->appendRow(mapItem);

            XmlMapToQItem(n, mapItem);
        }
    }
}

QVariant MapsTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole          //
        && orientation == Qt::Horizontal //
        && section == 0)
        return QString("Maps");

    return QVariant();
}
} // namespace Editor
