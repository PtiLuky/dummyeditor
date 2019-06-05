#include <QDebug>

#include "graphicmap/mapgraphicsscene.hpp"
#include "graphicmap/notpaintingstate.hpp"
#include "graphicmap/firstlayerstate.hpp"
#include "graphicmap/visiblegraphiclayer.hpp"
#include "graphicmap/selectiondrawingtool.hpp"
#include "graphicmap/selectiondrawingclipboard.hpp"

GraphicMap::NotPaintingState::NotPaintingState(
    GraphicMap::MapGraphicsScene& mapGraphicsScene)
    : GraphicMap::PaintingLayerState(mapGraphicsScene,
                                     QPair<int, int>(16, 16))
{
    qDebug() << "Not painting.";
}

void GraphicMap::NotPaintingState::adjustLayers() {
    // Do nothing.
}

void GraphicMap::NotPaintingState::onNewMap() {
    // Switch to painting first layer.
    m_mapGraphicsScene.setPaitingLayerState(
        new GraphicMap::FirstLayerState(m_mapGraphicsScene));
}

void GraphicMap::NotPaintingState::sceneCleared() {
    // Do nothing.
}

void GraphicMap::NotPaintingState::drawWithPen(const QPoint& point) const
{
    // Do nothing.
    Q_UNUSED(point);
}

void GraphicMap::NotPaintingState::drawWithRectangle(const QPoint& point,
                                                     const QRect& rect) const
{
    // Do nothing.
    Q_UNUSED(point);
    Q_UNUSED(rect);
}

void
GraphicMap::NotPaintingState::drawWithSelection(
    const QPoint& point,
    const SelectionDrawingClipboard& clipboard) const
{
    Q_UNUSED(point);
    Q_UNUSED(clipboard);
}

void GraphicMap::NotPaintingState::drawCurrentSelection(
    const QPoint& point, QGraphicsItem* item) const
{
    Q_UNUSED(point);
    Q_UNUSED(item);
}
