#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "core/graphic_layer.hpp"

#include "editor/map.hpp"
#include "editor/project.hpp"

#include "graphicmap/visiblegraphiclayer.hpp"
#include "graphicmap/blockinggraphiclayer.hpp"
#include "graphicmap/mapgraphicsscene.hpp"


namespace GraphicMap {

MapGraphicsScene::MapGraphicsScene(QObject* parent)
    : QGraphicsScene(parent),
      m_mapDocument(nullptr),
      m_map(nullptr),
      m_isDrawing(false),
      m_drawingTool(nullptr)

{
    //m_paintingLayerState = new GraphicMap::NotPaintingState(*this);
    //m_drawingTool = new NoDrawingTool(*this);
    installEventFilter(this);
}

MapGraphicsScene::~MapGraphicsScene()
{
    //delete m_drawingTool;
    //delete m_paintingLayerState;
}

void MapGraphicsScene::_drawGrid()
{

    QPen pen(Qt::black, 0.5);

    quint16 width = m_map->width();
    quint16 height = m_map->height();

    for (int i = 0; i <= width; ++i) {
        QGraphicsItem* item = addLine(i * 16, 0, i * 16, 16 * height, pen);
        item->setZValue(99);
    }

    for (int i = 0; i <= height; ++i) {
        QGraphicsItem* item = addLine(0, i * 16, 16 * width, 16 * i, pen);
        item->setZValue(99);
    }

}

MapGraphicsScene& MapGraphicsScene::setMapDocument
(const std::shared_ptr<Misc::MapDocument>& mapDocument) {
    if (m_map != nullptr) {
        QRect invalidateRegion(0, 0,
                               m_map->width() * 16, m_map->height() * 16);
        qDebug() << "INVALIDATE " << invalidateRegion;
        invalidate(invalidateRegion);

        for (auto& graphicLayer: m_graphicLayers) {
            QObject::disconnect(
                &graphicLayer->editorLayer(),
                SIGNAL(visibilityChanged(bool)),
                graphicLayer,
                SLOT(setVisibility(bool))
            );
        }

    }
    // Clear the scene
    clear();

    m_mapDocument = mapDocument;
    m_map = m_mapDocument->map();

    const Editor::Project& project = m_mapDocument->project();
    m_mapChipset = QPixmap(
        QString(
            (project.coreProject().projectPath()
             / "chipsets"
             / m_map->chipset()
            ).string().c_str())
    );

    for (auto& graphicLayer: m_graphicLayers) {
        delete graphicLayer;
    }
    m_graphicLayers.clear();

    int zindex = 0;
    for (const auto& level: m_map->levels()) {
        for (const auto& [position, layer]: level->graphicLayers()) {
            qDebug() << "Position: " << position;

            auto graphicLayer = new VisibleGraphicLayer(
                *layer,
                *this,
                m_mapChipset,
                zindex++
            );

            m_graphicLayers.push_back(graphicLayer);

            QObject::connect(
                &graphicLayer->editorLayer(),
                SIGNAL(visibilityChanged(bool)),
                graphicLayer,
                SLOT(setVisibility(bool))
            );

            QObject::connect(
                &graphicLayer->editorLayer(),
                SIGNAL(setSelected()),
                graphicLayer,
                SLOT(setSelected())
            );
        }

        // Add blocking layer
        auto graphicLayer = new BlockingGraphicLayer(
            level->blockingLayer(),
            *this,
            ++zindex
        );
        m_graphicLayers.push_back(graphicLayer);

        QObject::connect(
            &graphicLayer->editorLayer(),
            SIGNAL(visibilityChanged(bool)),
            graphicLayer,
            SLOT(setVisibility(bool))
        );
        QObject::connect(
            &graphicLayer->editorLayer(),
            SIGNAL(setSelected()),
            graphicLayer,
            SLOT(setSelected())
        );
    }

    //changeSelection(QRect(0,0,0,0));

    return *this;
}

void MapGraphicsScene::changeMapDocument(
    const std::shared_ptr<Misc::MapDocument>& mapDocument
) {
    setMapDocument(mapDocument);
}

void MapGraphicsScene::adjustLayers() const {
    /*
    if (nullptr != m_paintingLayerState)
    {
        m_paintingLayerState->adjustLayers();
    }
    */
}

void MapGraphicsScene::setDrawingTool(DrawingTool::DrawingTool* drawingTool) {
    m_drawingTool = drawingTool;
    qDebug() << "tool is set.";
}

void MapGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (nullptr != m_drawingTool) {
        m_drawingTool->mapMousePressEvent(event);
    }
}

void MapGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (nullptr != m_drawingTool) {
        m_drawingTool->mapMouseMoveEvent(event);
    }
}

void MapGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (nullptr != m_drawingTool) {
        m_drawingTool->mapMouseReleaseEvent(event);
    }
}

void MapGraphicsScene::keyPressEvent(QKeyEvent* event) {
    if (nullptr != m_drawingTool) {
        m_drawingTool->mapKeyPressEvent(event);
    }
}

void MapGraphicsScene::keyReleaseEvent(QKeyEvent* event) {
    if (nullptr != m_drawingTool) {
        m_drawingTool->mapKeyReleaseEvent(event);
    }
}


} // namespace GraphicMap
