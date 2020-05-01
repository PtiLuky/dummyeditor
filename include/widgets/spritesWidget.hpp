#ifndef SPRITESWIDGET_H
#define SPRITESWIDGET_H

#include <QWidget>
#include <memory>

#include "editor/project.hpp"

namespace Ui {
class spritesWidget;
}


//////////////////////////////////////////////////////////////////////////////
//  GeneralWindow class
//////////////////////////////////////////////////////////////////////////////

namespace Editor {
class SpritesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpritesWidget(QWidget* parent = nullptr);
    virtual ~SpritesWidget();

    void setProject(std::shared_ptr<Editor::Project> loadedProject);

public slots:
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void setGrid(bool showGrid);

    void on_btn_loadImage_clicked();
    void on_btn_newSprite_clicked();
    void on_list_sprites_clicked(const QModelIndex& index);
    void on_check_useMultiDir_clicked(bool checked);
    void on_check_useAnimation_clicked(bool checked);

private:
    std::unique_ptr<Ui::spritesWidget> m_ui;
    std::shared_ptr<Editor::Project> m_loadedProject;
};
} // namespace Editor

#endif // SPRITESWIDGET_H
