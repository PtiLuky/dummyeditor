#ifndef MAPSTREE_H
#define MAPSTREE_H

#include <QDomNode>
#include <QStandardItem>
#include <QTreeView>

#include "editor/project.hpp"
#include "widgets/mapEditDialog.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  MapsTreeView class
// a MapsTreeView represents a visual list of maps.
// It contains only the graphical representation and interactions. Data are
// kept in MapsTreeModel.
//////////////////////////////////////////////////////////////////////////////

class MapsTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit MapsTreeView(QWidget* parent = nullptr);

    void setProject(std::shared_ptr<Project> project);
    void clear();

public slots:
    void showNewMapDlg();
    void showEditDlg();
    void showContextMenu(const QPoint&);

    void addMapAtRoot();
    void createMap(int dlgButton);
    void editMap(int dlgButton);

signals:
    void chipsetMapChanged(QString);
    void mapChanged(const QString& mapName);

private:
    std::shared_ptr<Project> m_project;
    QMenu* m_mapMenu              = nullptr;
    QAction* m_newMapAction       = nullptr;
    QAction* m_editAction         = nullptr;
    MapEditDialog* m_newMapDialog = nullptr;
    MapEditDialog* m_editDialog   = nullptr;
    QModelIndex m_selectedIndex;
};

//////////////////////////////////////////////////////////////////////////////
//  MapTreeModel class
// a MapTreeModel is a wrapper around data to display in in MapsTreeView
//////////////////////////////////////////////////////////////////////////////

class MapsTreeModel : public QStandardItemModel
{
public:
    explicit MapsTreeModel(const QDomNode& mapsNode);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    void XmlMapToQItem(const QDomNode& mapsNode, QStandardItem* parent);
};
} // namespace Editor

#endif // MAPSTREE_H
