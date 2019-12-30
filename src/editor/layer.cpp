#include "editor/layer.hpp"

namespace Editor {

Layer::Layer()
    : m_visible(true)
{}

void Layer::setVisible(bool visible) {
    m_visible = visible;
    emit visibilityChanged(m_visible);
}

} // namespace Editor
