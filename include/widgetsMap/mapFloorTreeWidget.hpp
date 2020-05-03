#ifndef MAPFLOORLISTWIDGET_H
#define MAPFLOORLISTWIDGET_H

#include <QWidget>
#include <memory>

#include "widgetsMap/mapFloorTreeModel.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class MapFloorsList;
} // namespace Ui

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  Widget class
// This widget display a tree-view of the list of all floors of a map
//////////////////////////////////////////////////////////////////////////////

class FloorListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FloorListWidget(QWidget* parent = nullptr);
    virtual ~FloorListWidget();

    void setEditorMap(const Dummy::Map&);
    void selectFirstVisLayer();
    void reset();

    MapFloorTreeModel* model() const;

public slots:
    void on_treeViewFloors_clicked(const QModelIndex& index);
    void on_treeViewFloors_doubleClicked(const QModelIndex& index);

private:
    std::unique_ptr<Ui::MapFloorsList> m_ui;
    std::unique_ptr<MapFloorTreeModel> m_floorTreeModel;
};

} // namespace Editor

#endif // MAPFLOORLISTWIDGET_H
