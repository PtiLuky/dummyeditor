#ifndef BLOCKINGERASER_H
#define BLOCKINGERASER_H

#include "drawing_tool/blocking/blocking_tool.hpp"

//////////////////////////////////////////////////////////////////////////////
//  pre-declaration
//////////////////////////////////////////////////////////////////////////////

class QGraphicsSceneMouseEvent;

namespace GraphicMap {
    class BlockingGraphicLayer;
}

namespace DrawingTools {
namespace Blocking {

//////////////////////////////////////////////////////////////////////////////
//  BlockingEraser class
//////////////////////////////////////////////////////////////////////////////

class BlockingEraser : public BlockingTool {
    Q_OBJECT
public:
    BlockingEraser(GraphicMap::MapGraphicsScene&,
           GraphicMap::BlockingGraphicLayer* = nullptr);
    void accept(Visitor&) override;
    void mapMousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mapMouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mapMouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    void mapKeyPressEvent(QKeyEvent*) override;
    void mapKeyReleaseEvent(QKeyEvent*) override;
    void mapMouseLeaveEvent() override;
    void emitDrawingToolSelected() override;
    void onUnselected() override;
    void onSelected() override;

signals:
    void drawingToolSelected(::DrawingTools::DrawingTool*);

private:
    bool m_mouseClicked;
};

} // namespace Blocking
} // namespace DrawingTools

#endif // BLOCKINGERASER_H
