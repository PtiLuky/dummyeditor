#ifndef CINEMATICSWIDGET_H
#define CINEMATICSWIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class CinematicsWidget;
}

namespace Editor {

class CinematicsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CinematicsWidget(QWidget* parent = nullptr);
    virtual ~CinematicsWidget(); // we need this because of smart ptr + forward declaration..

private:
    std::unique_ptr<Ui::CinematicsWidget> m_ui;
};
} // namespace Editor
#endif // CINEMATICSWIDGET_H
