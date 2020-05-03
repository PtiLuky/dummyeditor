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

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

public slots:
    void zoomIn();
    void zoomOut();
    void zoomOne(); ///< zoom to scale 1:1
    void setGrid(bool showGrid);

    void on_btn_loadImage_clicked();
    void on_btn_newSprite_clicked();
    void on_list_sprites_currentRowChanged(int);

    void on_check_useMultiDir_clicked(bool checked);
    void on_input_width_valueChanged(int);
    void on_input_height_valueChanged(int);

    void on_check_anim1_clicked(bool checked);
    void on_input_frameCount1_valueChanged(int);
    void on_input_x1_valueChanged(int);
    void on_input_y1_valueChanged(int);

    void on_check_anim2_clicked(bool checked);
    void on_input_frameCount2_valueChanged(int);
    void on_input_x2_valueChanged(int);
    void on_input_y2_valueChanged(int);

    void on_check_anim3_clicked(bool checked);
    void on_input_frameCount3_valueChanged(int);
    void on_input_x3_valueChanged(int);
    void on_input_y3_valueChanged(int);

    void on_check_anim4_clicked(bool checked);
    void on_input_frameCount4_valueChanged(int);
    void on_input_x4_valueChanged(int);
    void on_input_y4_valueChanged(int);

private:
    void loadSpritesList();
    void updateFields();
    void updateImage();        ///< fetch and update image
    void updateImageDisplay(); ///< update only elments drawn over the image
    int scaled(int) const;     ///< apply zoom
    bool isOverImage(const QPoint&) const;
    QPoint mapToImage(const QPoint&) const; ///< widget coord to sprite sheet coord

private:
    std::unique_ptr<Ui::spritesWidget> m_ui;
    std::shared_ptr<Editor::Project> m_loadedProject;

    QPixmap m_loadedSpriteSheet;
    Dummy::sprite_id m_currSpriteId = Dummy::undefSprite;

    float m_zoom    = 4.F;
    bool m_showGrid = false;
    QPoint m_firstClick;
};
} // namespace Editor

#endif // SPRITESWIDGET_H
