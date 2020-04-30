#include "widgets/generalWindow.hpp"
#include "ui_GeneralWindow.h"

#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "dummyrpg/floor.hpp"

namespace Editor {
//////////////////////////////////////////////////////////////////////////////

GeneralWindow::GeneralWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::GeneralWindow)
    , m_mapTools(m_chipsetScene, m_mapScene, *m_ui)
{
    m_ui->setupUi(this);
    setupLoggers();

    m_ui->graphicsViewChipset->setScene(&m_chipsetScene);
    m_ui->graphicsViewChipset->scale(2.0, 2.0);

    m_ui->graphicsViewMap->setScene(&m_mapScene);
    m_ui->graphicsViewMap->scale(2.0, 2.0);
    m_ui->graphicsViewMap->setBackgroundBrush(QColor("#969696"));

    // Set default sizes of movable splitters between panels
    QList<int> horiCoef = {width() / 4, width() - (width() / 4)};
    QList<int> vertCoef = {width() / 4, width() - (width() / 4)};
    m_ui->splitter_map->setSizes(horiCoef);
    m_ui->splitter_chipset->setSizes(vertCoef);

    // Set default view state
    updateProjectView();

    // UI items connections and shortcuts

    m_ui->actionNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    m_ui->actionNew->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    m_ui->actionOpen->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    m_ui->actionSave->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionClose->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    m_ui->actionClose->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionCut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    m_ui->actionCut->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionCopy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    m_ui->actionCopy->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionPaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    m_ui->actionPaste->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    m_ui->actionUndo->setShortcutContext(Qt::ApplicationShortcut);

    m_ui->actionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    m_ui->actionRedo->setShortcutContext(Qt::ApplicationShortcut);

    // connect ui items
    connect(m_ui->btnNewMap, &QPushButton::clicked, m_ui->mapsList, &MapsTreeView::addMapAtRoot);
    // connect(m_ui->mapsList, &MapsTreeView::chipsetMapChanged, &m_chipsetScene, &ChipsetGraphicsScene::setChipset);
    connect(&m_mapScene, &MapGraphicsScene::zooming, this, &GeneralWindow::mapZoomTriggered);
}

GeneralWindow::~GeneralWindow()
{
    cleanLoggers();
}

//////////////////////////////////////////////////////////////////////////////

void GeneralWindow::closeEvent(QCloseEvent* event)
{
    bool closingAccepted = closeProject();

    if (closingAccepted)
        event->accept();
    else
        event->ignore();
}

bool GeneralWindow::loadProject(const QString& path)
{
    auto newProject = std::make_shared<Project>(QDir::cleanPath(path));

    // Check if has been successfully loaded
    // TODO add a "isLoaded" method to check loading errors
    // if not loaded, return false here

    // Use this new project
    m_loadedProject = newProject;

    // Update the view
    updateProjectView();

    return true;
}

bool GeneralWindow::closeProject()
{
    if (m_loadedProject == nullptr)
        return true; // success, nothing to do

    QMessageBox::StandardButton resBtn =
        QMessageBox::question(this, "DummyEditor", tr("Do you want to save before closing this project?"),
                              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Cancel);

    // Save project if needed
    if (resBtn == QMessageBox::Yes) {
        m_loadedProject->saveProject();
    } else if (resBtn == QMessageBox::No) {
        // Nothing to do
    } else {
        return false; // failure of closing : cancellation
    }

    // Clear project
    m_loadedProject = nullptr;

    // Clear view
    updateProjectView();

    return true; // success, we closed and cleaned
}

void GeneralWindow::updateProjectView()
{
    bool thereIsAProject = (m_loadedProject != nullptr);

    // Disable unusable components
    m_ui->panels_tabs->setEnabled(thereIsAProject);
    m_ui->toolbar_gameTool->setEnabled(thereIsAProject);
    if (! thereIsAProject) {
        m_mapTools.clear();
        m_ui->toolbar_mapTools->setEnabled(false);
        m_ui->chipsets_panel->setEnabled(false);
    }

    // update tabs content
    updateMapsAndFloorsList();
    updateChipsetsTab();

    // update usable actions
    m_ui->actionSave->setEnabled(thereIsAProject);
    m_ui->actionClose->setEnabled(thereIsAProject);
}

void GeneralWindow::updateMapsAndFloorsList()
{
    if (m_loadedProject == nullptr) {
        m_ui->mapsList->clear();
    } else {
        m_ui->mapsList->setProject(m_loadedProject);
    }
    m_ui->maps_panel->setCurrentIndex(0);
    m_ui->layer_list_tab->reset();
    m_mapScene.clear();
}

void GeneralWindow::updateChipsetsTab()
{
    m_chipsetScene.clear();
    m_ui->chipsets_panel->setTabText(0, "Tileset1");
}

void GeneralWindow::setupLoggers()
{
    // Console logger
    std::shared_ptr<Logger> pConsoleLog = std::make_shared<LoggerConsole>();
    m_loggers.push_back(pConsoleLog);
    Logger::registerLogger(pConsoleLog);

    // Status bar Logger
    std::shared_ptr<Logger> pStatusBarLog = std::make_shared<LoggerStatusBar>(m_ui->statusbar);
    m_loggers.push_back(pStatusBarLog);
    Logger::registerLogger(pStatusBarLog);

    // File logger

    std::shared_ptr<Logger> pFileLog = std::make_shared<LoggerFile>();
    m_loggers.push_back(pFileLog);
    Logger::registerLogger(pFileLog);
}

void GeneralWindow::cleanLoggers()
{
    for (const auto& pLogger : m_loggers)
        Logger::unregisterLogger(pLogger);
    m_loggers.clear();
}

//////////////////////////////////////////////////////////////////////////////

void GeneralWindow::on_actionNew_triggered()
{
    // Open a file dialog to select a folder
    QString projectDirectory = QFileDialog::getExistingDirectory(this, tr("Choose your project directory"));

    if (projectDirectory == "")
        return;

    bool projectClosed = closeProject();
    if (! projectClosed)
        return;

    // Initialize a project into this directory
    m_loadedProject = Project::create(projectDirectory);

    if (m_loadedProject == nullptr)
        return;

    Log::info(tr("Project created at %1").arg(projectDirectory));

    // Update the view
    updateProjectView();
}

void GeneralWindow::on_actionOpen_triggered()
{
    // Close current project
    bool projectClosed = closeProject();
    if (! projectClosed)
        return;

    // Ask where new is
    QString projectFile =
        QFileDialog::getOpenFileName(this, tr("Choose an existing project file"), "", tr("Dummy Project (*.xml)"));
    if (projectFile == "")
        return;

    // Open new
    loadProject(projectFile);
    Log::info(tr("Project %1 opened").arg(projectFile));
}

void GeneralWindow::on_actionSave_triggered()
{
    if (nullptr == m_loadedProject)
        return;

    // Save current project
    m_loadedProject->saveProject();

    // TODO: create a way to check the save action and get this info to
    // condition the output info
    Log::info(tr("Project saved"));
}

void GeneralWindow::on_actionClose_triggered()
{
    closeProject();
}

void GeneralWindow::on_actionPlay_triggered()
{
    if (m_loadedProject)
        m_loadedProject->testMap();
}

void GeneralWindow::on_mapsList_doubleClicked(const QModelIndex& selectedIndex)
{
    // fetch map data
    QString mapName = m_loadedProject->mapsModel()->itemFromIndex(selectedIndex)->text();

    bool bRes = m_loadedProject->loadMap(mapName);
    if (! bRes)
        return;
    auto* map = m_loadedProject->currMap();
    if (map == nullptr)
        return;

    m_mapTools.clear();

    // update chipset scene
    std::vector<QString> chipsets;
    for (Dummy::chip_id chipId : map->chipsetsUsed()) {
        QString chipFile = QString::fromStdString(m_loadedProject->game().tileSets[chipId]);
        QString chipPath = QDir::cleanPath(m_loadedProject->projectPath() + "/images/" + chipFile);
        chipsets.push_back(chipPath);
    }

    m_chipsetScene.setChipset(chipsets, map->chipsetsUsed());
    m_ui->chipsets_panel->setEnabled(true);
    m_ui->chipsetAddButton->setEnabled(false); // TODO remove this when feature enabled
    m_ui->graphicsViewChipset->viewport()->update();

    // update map scene
    m_mapScene.setMap(*map, m_chipsetScene.chipsets());
    m_ui->graphicsViewMap->setSceneRect(QRect(0, 0, map->width() * CELL_W, map->height() * CELL_H));

    // update floor list
    m_ui->layer_list_tab->setEditorMap(*map);
    const auto* floorTree = m_ui->layer_list_tab->model();
    connect(floorTree, &MapFloorTreeModel::activeLayerChanged, this, &GeneralWindow::activeLayerChanged);
    connect(floorTree, &MapFloorTreeModel::layerVisibilityChanged, this, &GeneralWindow::layerVisibilityChanged);
    m_ui->layer_list_tab->selectFirstVisLayer();

    // update layer list
    m_ui->maps_panel->setCurrentIndex(1);
}

void GeneralWindow::on_toggleGridChipset_clicked(bool isDown)
{
    m_chipsetScene.setGridVisible(isDown);
}

void GeneralWindow::activeLayerChanged(eLayerType type, uint8_t floorIdx, uint8_t layerIdx)
{
    if (type == eLayerType::Graphic) {
        m_ui->toolbar_mapTools->setEnabled(true);
        for (auto& layerWrap : m_mapScene.graphicLayers())
            if (layerWrap->isThisLayer(floorIdx, layerIdx))
                m_mapTools.setActiveLayer(*layerWrap);

    } else if (type == eLayerType::Blocking) {
        m_ui->toolbar_mapTools->setEnabled(true);
        for (auto& layerWrap : m_mapScene.blockingLayers())
            if (layerWrap->isThisFloor(floorIdx))
                m_mapTools.setActiveLayer(*layerWrap);

    } else {
        m_ui->toolbar_mapTools->setEnabled(false);
    }
}

void GeneralWindow::layerVisibilityChanged(bool newVisibility, eLayerType type, uint8_t floorIdx, uint8_t layerIdx)
{
    if (type == eLayerType::Graphic) {
        for (auto& layerWrap : m_mapScene.graphicLayers())
            if (layerWrap->isThisLayer(floorIdx, layerIdx))
                layerWrap->setVisibility(newVisibility);

    } else if (type == eLayerType::Blocking) {
        for (auto& layerWrap : m_mapScene.blockingLayers())
            if (layerWrap->isThisFloor(floorIdx))
                layerWrap->setVisibility(newVisibility);
    }
}

void GeneralWindow::on_actionEraser_triggered()
{
    m_mapTools.setTool(MapTools::eTools::Eraser);
}

void GeneralWindow::on_actionPen_triggered()
{
    m_mapTools.setTool(MapTools::eTools::Pen);
}

void GeneralWindow::on_actionSelection_triggered()
{
    m_mapTools.setTool(MapTools::eTools::Selection);
}

void GeneralWindow::on_actionToggleGrid_triggered()
{
    m_mapTools.updateGridDisplay();
}

void GeneralWindow::on_actionCut_triggered()
{
    m_mapTools.copyCut(MapTools::eCopyCut::Cut);
}

void GeneralWindow::on_actionCopy_triggered()
{
    m_mapTools.copyCut(MapTools::eCopyCut::Copy);
}

void GeneralWindow::on_actionPaste_triggered()
{
    m_mapTools.setTool(MapTools::eTools::Paste);
}
void GeneralWindow::on_actionUndo_triggered()
{
    m_mapTools.undo();
}
void GeneralWindow::on_actionRedo_triggered()
{
    m_mapTools.redo();
}
void GeneralWindow::on_actionZoomIn_triggered()
{
    m_ui->graphicsViewMap->scale(2.0, 2.0);
}
void GeneralWindow::on_actionZoomOut_triggered()
{
    m_ui->graphicsViewMap->scale(0.5, 0.5);
}
void GeneralWindow::on_actionResize_triggered()
{
    if (m_mapScene.height() < 1 || m_mapScene.width() < 1)
        return;

    qreal minScale = std::min(m_ui->graphicsViewMap->height() / m_mapScene.height(),
                              m_ui->graphicsViewMap->width() / m_mapScene.width());
    m_ui->graphicsViewMap->resetTransform();
    m_ui->graphicsViewMap->scale(minScale, minScale);
}

void GeneralWindow::mapZoomTriggered(MapGraphicsScene::eZoom zoom)
{
    if (zoom == MapGraphicsScene::eZoom::ZoomIn) {
        m_ui->graphicsViewMap->scale(1.25, 1.25);
    } else if (zoom == MapGraphicsScene::eZoom::ZoomOut) {
        m_ui->graphicsViewMap->scale(0.75, 0.75);
    }
}

//////////////////////////////////////////////////////////////////////////////

LoggerStatusBar::LoggerStatusBar(QStatusBar* stsBar)
    : m_statusBar(stsBar)
{}

void LoggerStatusBar::print(const std::string& message, eLogType type)
{
    switch (type) {
    case eLogType::INFORMATION:
    case eLogType::ERROR:
        m_statusBar->showMessage(QString::fromStdString(message));
        break;
    case eLogType::LOG:
    case eLogType::DEBUG:
    default:
        break; // do nothing log default and unknown types
    }
}
} // namespace Editor
