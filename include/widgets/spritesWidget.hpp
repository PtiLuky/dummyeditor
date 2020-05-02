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
    void setCurrentSprite(Dummy::sprite_id);

public slots:
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void setGrid(bool showGrid);

    void on_btn_loadImage_clicked();
    void on_btn_newSprite_clicked();
    void on_list_sprites_currentRowChanged(int);
    void on_check_useMultiDir_clicked(bool checked);
    void on_check_useAnimation_clicked(bool checked);

private:
    void loadSpritesList();
    void updateImage();

private:
    std::unique_ptr<Ui::spritesWidget> m_ui;
    std::shared_ptr<Editor::Project> m_loadedProject;

    QPixmap m_loadedSpriteSheet;
    Dummy::sprite_id m_currSpriteId = Dummy::undefSprite;

    float m_zoom = 4.F;
    QPoint m_firstClick;
};
} // namespace Editor

#endif // SPRITESWIDGET_H
