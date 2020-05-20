#include "widgets/characterInstanceWidget.hpp"
#include "ui_characterInstanceWidget.h"

#include "dummyrpg/floor.hpp"

namespace Editor {


CharacterInstanceWidget::CharacterInstanceWidget(std::shared_ptr<Editor::Project> loadedProject, Dummy::Floor& floor,
                                                 Dummy::CharacterInstance& charaInst, QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::CharacterInstanceWidget)
    , m_loadedProject(loadedProject)
    , m_floor(floor)
    , m_charInst(charaInst)
{
    m_ui->setupUi(this);

    if (m_loadedProject == nullptr)
        return;

    auto* chara = m_loadedProject->game().character(m_charInst.characterId());
    if (chara == nullptr)
        return;

    // Set name
    m_ui->lbl_name->setText(QString::fromStdString(chara->name()));
    // Set event
    // TODO
    // Set direction
    if (m_charInst.pos().dir == Dummy::Direction::Top)
        m_ui->choice_up->setChecked(true);
    else if (m_charInst.pos().dir == Dummy::Direction::Right)
        m_ui->choice_right->setChecked(true);
    else if (m_charInst.pos().dir == Dummy::Direction::Bottom)
        m_ui->choice_down->setChecked(true);
    else
        m_ui->choice_left->setChecked(true);
}

CharacterInstanceWidget::~CharacterInstanceWidget() {}

void CharacterInstanceWidget::on_btn_delete_clicked()
{
    m_floor.deleteNpcAt(m_charInst.pos().coord);
    m_loadedProject->changed();
    this->reject();
}

void CharacterInstanceWidget::on_choice_up_clicked()
{
    auto pos = m_charInst.pos();
    pos.dir  = Dummy::Direction::Top;
    m_charInst.setPos(pos);
    m_loadedProject->changed();
}

void CharacterInstanceWidget::on_choice_left_clicked()
{
    auto pos = m_charInst.pos();
    pos.dir  = Dummy::Direction::Left;
    m_charInst.setPos(pos);
    m_loadedProject->changed();
}

void CharacterInstanceWidget::on_choice_right_clicked()
{
    auto pos = m_charInst.pos();
    pos.dir  = Dummy::Direction::Right;
    m_charInst.setPos(pos);
    m_loadedProject->changed();
}

void CharacterInstanceWidget::on_choice_down_clicked()
{
    auto pos = m_charInst.pos();
    pos.dir  = Dummy::Direction::Bottom;
    m_charInst.setPos(pos);
    m_loadedProject->changed();
}

} // namespace Editor
