#include "editor/layerEvents.hpp"

#include <dummy/core/floor.hpp>
#include <dummy/core/layer.hpp>

namespace Editor {

EventsLayer::EventsLayer(Dummy::Core::Floor& floor)
    : m_floor(floor)
{}

std::uint16_t EventsLayer::width() const
{
    return m_floor.width();
}

std::uint16_t EventsLayer::height() const
{
    return m_floor.height();
}

} // namespace Editor
