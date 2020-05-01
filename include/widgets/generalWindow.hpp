#ifndef GENERALWINDOW_H
#define GENERALWINDOW_H

#include <QMainWindow>
#include <memory>

#include "editor/project.hpp"
#include "utils/logger.hpp"
#include "widgetsMap/chipsetGraphicsScene.hpp"
#include "widgetsMap/mapGraphicsScene.hpp"
#include "widgets/mapTools.hpp"

namespace Ui {
class GeneralWindow;
}

//////////////////////////////////////////////////////////////////////////////
//  GeneralWindow class
//////////////////////////////////////////////////////////////////////////////

namespace Editor {
class GeneralWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GeneralWindow(QWidget* parent = nullptr);
    virtual ~GeneralWindow();

public:
    bool loadProject(const QString& path);
    bool closeProject();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionClose_triggered();
    void on_actionPlay_triggered();
    void on_mapsList_doubleClicked(const QModelIndex& selectedIndex);
    void on_btnSwapBackground_clicked(bool isDown);
    void on_toggleGridChipset_clicked(bool isDown);
    void on_maps_panel_currentChanged(int);

    void on_actionEraser_triggered();
    void on_actionPen_triggered();
    void on_actionSelection_triggered();
    void on_actionToggleGrid_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionResize_triggered();

    void mapZoomTriggered(MapGraphicsScene::eZoom);
    void activeLayerChanged(eLayerType type, uint8_t floorIdx, uint8_t layerIdx);
    void layerVisibilityChanged(bool newVisibility, eLayerType type, uint8_t floorIdx, uint8_t layerIdx);

    void loadMap(const QString& mapName);

    void saveStatusChanged(bool isSaved);

private:
    void closeEvent(QCloseEvent* event) override;
    void updateProjectView();
    void updateMapsAndFloorsList();
    void updateChipsetsTab();

    void setupLoggers();
    void cleanLoggers();

    std::unique_ptr<Ui::GeneralWindow> m_ui;

    ChipsetGraphicsScene m_chipsetScene;
    MapGraphicsScene m_mapScene;
    MapTools m_mapTools;

    std::shared_ptr<Editor::Project> m_loadedProject;
    std::vector<std::shared_ptr<Logger>> m_loggers;
};

// This is a wrapper around status bar to use log system
class LoggerStatusBar : public Logger
{
public:
    explicit LoggerStatusBar(QStatusBar* stsBar);
    void print(const std::string& message, eLogType type) override;

private:
    QStatusBar* m_statusBar = nullptr;
};
} // namespace Editor

#endif // GENERALWINDOW_H
