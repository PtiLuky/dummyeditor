#include "widgets/spritesWidget.hpp"
#include "ui_spritesWidget.h"
namespace Editor {


SpritesWidget::SpritesWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::spritesWidget)
{
    m_ui->setupUi(this);

    int minSize         = width() / 12;
    QList<int> horiCoef = {2 * minSize, 7 * minSize, 3 * minSize};
    m_ui->splitter->setSizes(horiCoef);
}

SpritesWidget::~SpritesWidget() {}

void SpritesWidget::setProject(std::shared_ptr<Editor::Project> loadedProject)
{
    m_loadedProject = loadedProject;
}

void SpritesWidget::zoomIn() {}
void SpritesWidget::zoomOut() {}
void SpritesWidget::zoomFit() {}
void SpritesWidget::setGrid(bool showGrid) {}

void Editor::SpritesWidget::on_btn_loadImage_clicked() {}

void Editor::SpritesWidget::on_btn_newSprite_clicked() {}

void Editor::SpritesWidget::on_list_sprites_clicked(const QModelIndex& index) {}

void Editor::SpritesWidget::on_check_useMultiDir_clicked(bool checked) {}

void Editor::SpritesWidget::on_check_useAnimation_clicked(bool checked) {}

} // namespace Editor
