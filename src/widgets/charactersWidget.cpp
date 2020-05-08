#include "widgets/charactersWidget.hpp"
#include "ui_charactersWidget.h"

#include "widgets/spritesWidget.hpp"

namespace Editor {


CharactersWidget::CharactersWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::CharactersWidget)
{
    m_ui->setupUi(this);

    // Setup default panel sizes
    int minSize         = width() / 12;
    QList<int> horiCoef = {2 * minSize, 8 * minSize, 2 * minSize};
    m_ui->splitter->setSizes(horiCoef);
}

CharactersWidget::~CharactersWidget() {}

void CharactersWidget::setProject(std::shared_ptr<Editor::Project> loadedProject)
{
    m_loadedProject = loadedProject;
    loadCharactersList();
    setCurrCharacter(Dummy::undefChar);
}

void CharactersWidget::setCurrCharacter(Dummy::char_id id)
{
    m_currCharacterId = id;

    const Dummy::Character* chara = nullptr;
    if (m_loadedProject != nullptr)
        chara = m_loadedProject->game().character(id);

    m_ui->panel_properties->setEnabled(chara != nullptr);
    m_ui->panel_apparitions->setEnabled(chara != nullptr);
    m_ui->lbl_charId->setText((chara == nullptr) ? tr("Undefined") : QString::number(id));
    m_ui->input_charName->setText((chara == nullptr) ? "" : QString::fromStdString(chara->name()));

    // TODO update apparitions list

    updateSpritePreview();
}

void CharactersWidget::showEvent(QShowEvent* event)
{
    loadCharactersList();

    if (m_loadedProject != nullptr)
        m_ui->btn_addToCurrMap->setEnabled(m_loadedProject->currMap() != nullptr);

    QWidget::showEvent(event);
}

void CharactersWidget::loadCharactersList()
{
    if (m_loadedProject == nullptr) {
        m_ui->list_characters->clear();
        return;
    }

    int selectedRow = m_ui->list_characters->currentRow();
    m_ui->list_characters->blockSignals(true);
    m_ui->list_characters->clear();

    const size_t nbChars = m_loadedProject->game().characters().size();
    for (Dummy::sprite_id i = 0; i < nbChars; ++i) {
        const auto& charac = m_loadedProject->game().characters()[i];

        m_ui->list_characters->addItem(QString::number(i) + " - " + QString::fromStdString(charac.name()));
    }
    m_ui->list_characters->blockSignals(false);
    m_ui->list_characters->setCurrentRow(selectedRow);
}

void CharactersWidget::updateSpritePreview()
{
    if (m_loadedProject == nullptr)
        return;

    const auto* chara                   = m_loadedProject->game().character(m_currCharacterId);
    const Dummy::AnimatedSprite* sprite = nullptr;

    if (chara != nullptr)
        sprite = m_loadedProject->game().sprite(chara->spriteId());

    if (sprite == nullptr) {
        m_ui->lbl_spriteName->setText(tr("Undefined"));
        m_ui->img_preview->setPixmap(QPixmap());
    } else {
        std::string spriteSheet = m_loadedProject->game().spriteSheet(sprite->spriteSheetId);
        if (spriteSheet.empty()) {
            m_ui->lbl_spriteName->setText(tr("%1 - No spritesheet").arg(chara->spriteId()));
            m_ui->img_preview->setPixmap(QPixmap());
        } else {
            QString sheetName = QString::fromStdString(spriteSheet);
            m_ui->lbl_spriteName->setText(QString("%1 - ").arg(chara->spriteId()) + sheetName);
            // TODO m_ui->img_preview->setPixmap();
        }
    }
}

void CharactersWidget::on_list_characters_currentRowChanged(int currentRow)
{
    setCurrCharacter(static_cast<Dummy::char_id>(currentRow));
}

void CharactersWidget::on_btn_newCharacter_clicked()
{
    if (m_loadedProject == nullptr)
        return;
    m_loadedProject->createCharacter();
    loadCharactersList();
}

void CharactersWidget::on_input_charName_textChanged(const QString& name)
{
    if (m_loadedProject == nullptr)
        return;

    auto* chara = m_loadedProject->game().character(m_currCharacterId);
    if (chara == nullptr)
        return;

    chara->setName(name.toStdString());
    m_loadedProject->changed();
}

void CharactersWidget::on_btn_changeSprite_clicked()
{
    if (m_loadedProject == nullptr)
        return;

    auto* chara = m_loadedProject->game().character(m_currCharacterId);
    if (chara == nullptr)
        return;

    auto dlg = std::make_unique<SpriteSelectionDialog>(m_loadedProject, this);
    dlg->setCurrentSprite(chara->spriteId());
    if (dlg->exec() == QDialog::Accepted) {
        chara->setSprite(dlg->currentSprite());
        updateSpritePreview();
        m_loadedProject->changed();
    }
}

void CharactersWidget::on_list_occurences_doubleClicked(const QModelIndex& index) {}

void CharactersWidget::on_btn_addToCurrMap_clicked() {}

} // namespace Editor
