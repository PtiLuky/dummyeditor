#pragma once

class QGraphicsSceneMouseEvent;
class QRect;

namespace GraphicMap {
    class MapGraphicsScene;

    class DrawingTool
    {
    public:
        DrawingTool(MapGraphicsScene&);
        virtual ~DrawingTool();
        virtual void chipsetSelectionChanged(const QRect&) = 0;
        virtual void onMousePress(QGraphicsSceneMouseEvent*) = 0;
        virtual void onMouseLeave() = 0;
        virtual void onMouseMove(QGraphicsSceneMouseEvent*) = 0;
        virtual void onMouseRelease(QGraphicsSceneMouseEvent*) = 0;

    protected:
        MapGraphicsScene& m_mapGraphicScene;
        bool m_isDrawing;
    };
}
