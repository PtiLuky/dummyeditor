#pragma once
#include <memory>
#include <QGraphicsScene>
#include <QPixmap>

#include "drawing_tool/graphic/eraser.hpp"
#include "drawing_tool/graphic/pen.hpp"
#include "drawing_tool/graphic/rectangle.hpp"

#include "graphicmap/graphiclayer.hpp"

namespace Editor {
class GraphicLayer;
} // namespace Editor

namespace LayerClipboard {
class Clipboard;
} // namespace LayerClipboard

namespace Dummy {
namespace Core {
class GraphicLayer;
} // namespace Core
} // namespace Dummy


namespace GraphicMap {
// Forward declaration:
class MapGraphicsScene;

class VisibleGraphicLayer : public GraphicLayer {

public:
    VisibleGraphicLayer(Editor::GraphicLayer&,
                        MapGraphicsScene&,
                        const QPixmap&,
                        int);
    ~VisibleGraphicLayer() override;
    inline const QPixmap& chipsetPixmap() const {
        return m_chipsetPixmap;
    }

    inline const Editor::GraphicLayer& layer() const {
        return m_graphicLayer;
    }

    VisibleGraphicLayer& setTile(quint16 x,
                                 quint16 y,
                                 qint16 chipsetX,
                                 qint16 chipsetY);

    VisibleGraphicLayer& setChipsetPixmap(const QPixmap*);

    virtual MapSceneLayer& removeTile(quint16, quint16) override;

    Editor::Layer& editorLayer() override;

    std::vector<DrawingTools::DrawingTool*> drawingTools() override;

    std::shared_ptr<LayerClipboard::Clipboard>
    getClipboardRegion(const QRect& clip) override;

    void accept(GraphicLayerVisitor&) override;
private:
    Editor::GraphicLayer& m_graphicLayer;
    const QPixmap& m_chipsetPixmap;
};
} // namespace GraphicMap
