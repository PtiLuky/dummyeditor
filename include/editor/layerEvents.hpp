#ifndef EVENTSLAYER_H
#define EVENTSLAYER_H

#include <dummy/core/floor.hpp>

#include "editor/layer.hpp"

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  EventsLayer class
//////////////////////////////////////////////////////////////////////////////

class EventsLayer : public Layer
{
public:
    explicit EventsLayer(Dummy::Core::Floor&);
    Dummy::Core::Events& touchEvents() { return m_floor.touchEvents(); }
    std::uint16_t width() const;
    std::uint16_t height() const;
    Dummy::Core::Floor& floor() { return m_floor; }

private:
    Dummy::Core::Floor& m_floor;
};

} // namespace Editor

#endif // EVENTSLAYER_H
