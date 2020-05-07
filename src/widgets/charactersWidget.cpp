#include "widgets/charactersWidget.hpp"
#include "ui_charactersWidget.h"

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
    if (m_loadedProject != nullptr && id < m_loadedProject->game().characters.size())
        chara = &m_loadedProject->game().characters[id];

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

    const size_t nbChars = m_loadedProject->game().characters.size();
    for (Dummy::sprite_id i = 0; i < nbChars; ++i) {
        const auto& charac = m_loadedProject->game().characters[i];

        m_ui->list_characters->addItem(QString::number(i) + " - " + QString::fromStdString(charac.name()));
    }
    m_ui->list_characters->blockSignals(false);
    m_ui->list_characters->setCurrentRow(selectedRow);
}

void CharactersWidget::updateSpritePreview()
{
    Dummy::AnimatedSprite* sprite = nullptr;
    auto& charas                  = m_loadedProject->game().characters;
    auto& sprites                 = m_loadedProject->game().sprites;

    if (m_loadedProject != nullptr                                //
        && m_currCharacterId < charas.size()                      //
        && charas[m_currCharacterId].spriteId() < sprites.size()) //
        sprite = &sprites[charas[m_currCharacterId].spriteId()];

    if (sprite == nullptr) {
        m_ui->lbl_spriteName->setText(tr("Undefined"));
        m_ui->img_preview->setPixmap(QPixmap());
    } else {
        if (sprite->spriteSheetId < m_loadedProject->game().spriteSheets.size()) {
            QString sheetName = QString::fromStdString(m_loadedProject->game().spriteSheets[sprite->spriteSheetId]);
            m_ui->lbl_spriteName->setText(QString("%1 - ").arg(sprite->spriteSheetId) + sheetName);
            // TODO m_ui->img_preview->setPixmap();
        } else {
            m_ui->lbl_spriteName->setText(tr("%1 - No spritesheet").arg(sprite->spriteSheetId));
            m_ui->img_preview->setPixmap(QPixmap());
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

void CharactersWidget::on_input_charName_textChanged(const QString& arg1) {}

void CharactersWidget::on_btn_changeSprite_clicked() {}

void CharactersWidget::on_list_occurences_doubleClicked(const QModelIndex& index) {}

void CharactersWidget::on_btn_addToCurrMap_clicked() {}

} // namespace Editor
