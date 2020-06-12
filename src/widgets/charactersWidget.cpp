#include "widgets/charactersWidget.hpp"
#include "ui_charactersWidget.h"

#include "widgets/spritesWidget.hpp"

#include <QMessageBox>

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

    int idx      = -1;
    size_t nbIdx = m_ids.size();
    for (size_t i = 0; i < nbIdx; ++i)
        if (m_ids[i] == id) {
            idx = static_cast<int>(i);
            break;
        }

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

    m_ui->list_characters->blockSignals(true);
    m_ui->list_characters->clear();

    m_ids.clear();
    for (const auto& chara : m_loadedProject->game().characters()) {
        m_ui->list_characters->addItem(QString::number(chara.second.id()) + " - "
                                       + QString::fromStdString(chara.second.name()));
        m_ids.push_back(chara.second.id());
    }

    m_ui->list_characters->blockSignals(false);
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
    size_t idx = static_cast<size_t>(currentRow);
    if (idx < m_ids.size())
        setCurrCharacter(m_ids[idx]);
}

void CharactersWidget::on_btn_newCharacter_clicked()
{
    if (m_loadedProject == nullptr)
        return;
    auto id = m_loadedProject->game().registerCharacter(tr("Unnamed").toStdString());
    m_loadedProject->changed();

    loadCharactersList();
    setCurrCharacter(id);
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
    loadCharactersList();
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

void CharactersWidget::on_btn_delete_clicked()
{
    if (m_loadedProject == nullptr)
        return;

    auto btn = QMessageBox::question(
        this, tr("Confirmation"), tr("You are about to delete this character and all its occurences. Are you sure?"));
    if (btn != QMessageBox::Yes)
        return;

    auto* openedMap = m_loadedProject->currMap();
    if (openedMap)
        openedMap->unregisterCharacter(m_currCharacterId);

    m_loadedProject->game().unregisterCharacter(m_currCharacterId);

    loadCharactersList();
    setCurrCharacter(Dummy::undefChar);
    m_loadedProject->changed();
}

void CharactersWidget::on_list_occurences_doubleClicked(const QModelIndex& index) {}

void CharactersWidget::on_btn_addToCurrMap_clicked()
{
    if (m_loadedProject != nullptr //
        && m_loadedProject->game().character(m_currCharacterId) != nullptr)
        emit requestAddChar(m_currCharacterId);
}

} // namespace Editor
