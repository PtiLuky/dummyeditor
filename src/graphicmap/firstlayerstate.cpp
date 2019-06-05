#include <QDebug>

#include "graphicmap/blockinggraphiclayer.hpp"
#include "graphicmap/visiblegraphiclayer.hpp"
#include "graphicmap/firstlayerstate.hpp"
#include "graphicmap/startingpointlayer.hpp"
#include "graphicmap/mapgraphicsscene.hpp"
#include "graphicmap/graphiclayer.hpp"

GraphicMap::FirstLayerState::FirstLayerState(
        GraphicMap::MapGraphicsScene& mapGraphicsScene)
    : GraphicMap::PaintingGraphicLayerState(mapGraphicsScene)
{
    qDebug() << "First layer state.";

}

GraphicMap::FirstLayerState::~FirstLayerState() {

}

void GraphicMap::FirstLayerState::adjustLayers() {
    qDebug() << "First layer: adjust.";
    m_mapGraphicsScene.secondLayer()->setOpacity(0.5);
    m_mapGraphicsScene.thirdLayer()->setOpacity(0.25);
    m_mapGraphicsScene.fourthLayer()->setOpacity(0.125);
    m_mapGraphicsScene.blockingLayer()->setOpacity(0);
    m_mapGraphicsScene.startingPointLayer()->setOpacity(0);
}

void GraphicMap::FirstLayerState::onNewMap() {
    // Refresh the active layer.
    m_mapGraphicsScene.setActiveLayer(m_mapGraphicsScene.firstLayer());
}

void GraphicMap::FirstLayerState::sceneCleared() {
    // Do nothing.
}
