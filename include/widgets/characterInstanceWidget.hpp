#ifndef CHARACTERINSTANCEWIDGET_H
#define CHARACTERINSTANCEWIDGET_H

#include <QDialog>
#include <memory>

#include "editor/project.hpp"

namespace Ui {
class CharacterInstanceWidget;
}
namespace Editor {


class CharacterInstanceWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CharacterInstanceWidget(std::shared_ptr<Project> loadedProject, Dummy::Floor&, Dummy::CharacterInstance&,
                                     QWidget* parent);
    virtual ~CharacterInstanceWidget();

private slots:
    void on_btn_delete_clicked();
    void on_choice_up_clicked();
    void on_choice_left_clicked();
    void on_choice_right_clicked();
    void on_choice_down_clicked();

    void onEventChanged(Dummy::event_id);

private:
    std::unique_ptr<Ui::CharacterInstanceWidget> m_ui;
    std::shared_ptr<Project> m_loadedProject;
    Dummy::Floor& m_floor;
    Dummy::CharacterInstance& m_charInst;
};
} // namespace Editor
#endif // CHARACTERINSTANCEWIDGET_H
