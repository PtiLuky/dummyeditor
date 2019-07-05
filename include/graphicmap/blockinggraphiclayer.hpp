#pragma once

#include <QtGlobal>
#include <QVector>

#include "graphicmap/graphiclayer.hpp"

namespace Editor {
class BlockingLayer;
} // namespace Editor

namespace GraphicMap {
    class BlockingSquareItem;
    class MapGraphicsScene;
    class MapSceneLayer;
    class BlockingGraphicLayer : public GraphicLayer
    {
    public:
        BlockingGraphicLayer(
            Editor::BlockingLayer&,
            MapGraphicsScene&,
            int zValue
        );
        ~BlockingGraphicLayer() override;
        MapSceneLayer& removeTile(quint16, quint16) override;
        void toggleTile(quint16, quint16);
        void setTile(quint16, quint16, bool);
        Editor::Layer& editorLayer() override;

        std::vector<std::unique_ptr<DrawingTool::DrawingTool>>
        getDrawingTools() override;
    private:
        void _draw(int, quint16, quint16);
        Editor::BlockingLayer& m_blockingLayer;
        QVector<BlockingSquareItem*> m_crossItems;
    };
}
