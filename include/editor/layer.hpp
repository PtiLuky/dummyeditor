#pragma once

#include <QObject>

namespace Editor {
class Layer : public ::QObject {
    Q_OBJECT
public:
    Layer();
    bool visible() {
        return m_visible;
    }
    void setVisible(bool visible);
signals:
    void visibilityChanged(bool);
    void setSelected();
private:
    bool m_visible;
};
} // namespace Editor