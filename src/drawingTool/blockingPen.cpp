#include "drawingTool/blockingPen.hpp"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "drawingTool/drawingVisitor.hpp"
#include "graphicMap/layerGraphicBlocking.hpp"

namespace DrawingTools {

BlockingPen::BlockingPen(GraphicMap::MapGraphicsScene& mapGraphicsScene,
                         GraphicMap::BlockingGraphicLayer* blockingGraphicLayer)
    : BlockingGeneralTool(QIcon(":/icons/icon_pen_2.png"), mapGraphicsScene,
                          blockingGraphicLayer)
{}

void BlockingPen::mapMouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (nullptr != blockingLayer()) {
        QPoint point(mouseEvent->scenePos().toPoint());
        point.setX(point.x() - (point.x() % 8));
        point.setY(point.y() - (point.y() % 8));

        if (m_mouseClicked) {
            // XXX: set blocking tile
            blockingLayer()->setTile(point.x(), point.y(), true);
        }
    }
}

void BlockingPen::mapMousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Blocking pen press.";
    if (nullptr == blockingLayer()) {
        return;
    }

    QPoint point(event->scenePos().toPoint());
    point.setX(point.x() - (point.x() % 8));
    point.setY(point.y() - (point.y() % 8));
    // XXX: set blocking tile.
    blockingLayer()->setTile(point.x(), point.y(), true);

    m_mouseClicked = true;
}

void BlockingPen::mapMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Pen release.";
    m_mouseClicked = false;
}

void BlockingPen::mapKeyPressEvent(QKeyEvent* event)
{
    qDebug() << "key press.";
}

void BlockingPen::mapKeyReleaseEvent(QKeyEvent* event)
{
    qDebug() << "key release.";
}

void BlockingPen::mapMouseLeaveEvent() {}

void BlockingPen::accept(DrawingVisitor& visitor)
{
    visitor.visitTool(*this);
}

void BlockingPen::emitDrawingToolSelected()
{
    // TODO : check if emiting 2 different signals is really what we want?
    BlockingGeneralTool::emitDrawingToolSelected();
    emit drawingToolSelected(this);
}

void BlockingPen::onUnselected() {}

void BlockingPen::onSelected() {}

} // namespace DrawingTools