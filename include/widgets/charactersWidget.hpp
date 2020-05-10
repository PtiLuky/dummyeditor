#ifndef CHARACTERSWIDGET_H
#define CHARACTERSWIDGET_H

#include <QWidget>
#include <memory>

#include "editor/project.hpp"

namespace Ui {
class CharactersWidget;
}

namespace Editor {
class CharactersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CharactersWidget(QWidget* parent = nullptr);
    virtual ~CharactersWidget();

    void setProject(std::shared_ptr<Editor::Project> loadedProject);
    void setCurrCharacter(Dummy::char_id id);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void on_list_characters_currentRowChanged(int currentRow);
    void on_btn_newCharacter_clicked();
    void on_input_charName_textChanged(const QString& arg1);
    void on_btn_changeSprite_clicked();
    void on_list_occurences_doubleClicked(const QModelIndex& index);
    void on_btn_addToCurrMap_clicked();

signals:
    void requestAddChar(Dummy::char_id);

private:
    void loadCharactersList();
    void updateSpritePreview();

private:
    std::unique_ptr<Ui::CharactersWidget> m_ui;
    std::shared_ptr<Editor::Project> m_loadedProject;
    Dummy::char_id m_currCharacterId = Dummy::undefChar;
};
} // namespace Editor

#endif // CHARACTERSWIDGET_H
