#include "editor/graphic_layer.hpp"

#include <dummy/core/graphic_layer.hpp>

using namespace std;

namespace Editor {

GraphicLayer::GraphicLayer(Dummy::Core::GraphicLayer& layer)
    : m_layer(layer) {
}

pair<int8_t, int8_t>& GraphicLayer::operator[](size_t index) {
    return m_layer[index];
}

uint16_t GraphicLayer::width() const {
    return m_layer.width();
}

uint16_t GraphicLayer::height() const {
    return m_layer.height();
}

} // namespace Editor
