#include "widgets/spritesWidget.hpp"
#include "ui_spritesWidget.h"

#include <QFileDialog>

namespace Editor {


SpritesWidget::SpritesWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::spritesWidget)
{
    m_ui->setupUi(this);

    // Setup default panel sizes
    int minSize         = width() / 12;
    QList<int> horiCoef = {2 * minSize, 7 * minSize, 3 * minSize};
    m_ui->splitter->setSizes(horiCoef);
}

SpritesWidget::~SpritesWidget() {}

void SpritesWidget::setProject(std::shared_ptr<Editor::Project> loadedProject)
{
    m_loadedProject = loadedProject;
    loadSpritesList();
}

void SpritesWidget::setCurrentSprite(Dummy::sprite_id id)
{
    if (m_loadedProject == nullptr)
        return;

    // Enable panels
    m_ui->panel_drawSprite->setEnabled(true);
    m_ui->panel_preview->setEnabled(true);

    m_currSpriteId = id;
    updateImage();
}

void SpritesWidget::updateImage()
{
    const auto& spritesSheets = m_loadedProject->game().spriteSheets;
    const auto& sprites       = m_loadedProject->game().sprites;
    if (m_currSpriteId >= sprites.size())
        return;

    const auto& sprite = sprites[m_currSpriteId];
    // Set the image and its name
    if (sprites[m_currSpriteId].spriteSheetId < spritesSheets.size()) {
        QString sheetName   = QString::fromStdString(spritesSheets[sprite.spriteSheetId]);
        QString sheetPath   = QString::fromStdString(m_loadedProject->game().spriteSheetPath(sprite.spriteSheetId));
        m_loadedSpriteSheet = QPixmap(sheetPath);
        m_ui->label->setText(QString::number(m_currSpriteId) + " - " + sheetName);
    } else {
        m_ui->label->setText(QString::number(m_currSpriteId) + " - " + tr("Undefined"));
        m_ui->image_center->setText(tr("Select a sprite sheet"));
        m_ui->image_center->setEnabled(false);
        return;
    }

    QPixmap displayImg = m_loadedSpriteSheet.scaledToWidth(static_cast<int>(m_loadedSpriteSheet.width() * m_zoom));
    m_ui->image_center->setEnabled(true);
    m_ui->image_center->setPixmap(displayImg);
}

void SpritesWidget::loadSpritesList()
{
    if (m_loadedProject == nullptr)
        return;

    int selectedRow = m_ui->list_sprites->currentRow();
    m_ui->list_sprites->blockSignals(true);
    m_ui->list_sprites->clear();

    const auto& spritesSheets = m_loadedProject->game().spriteSheets;
    const size_t nbSprites    = m_loadedProject->game().sprites.size();
    for (Dummy::sprite_id i = 0; i < nbSprites; ++i) {
        const auto& sprite = m_loadedProject->game().sprites[i];

        QString spritesheet = tr("Undefined");
        if (sprite.spriteSheetId < spritesSheets.size())
            spritesheet = QString::fromStdString(spritesSheets[sprite.spriteSheetId]);

        m_ui->list_sprites->addItem(QString::number(i) + " - " + spritesheet);
    }
    m_ui->list_sprites->blockSignals(false);
    m_ui->list_sprites->setCurrentRow(selectedRow);
}

void SpritesWidget::zoomIn() {}
void SpritesWidget::zoomOut() {}
void SpritesWidget::zoomFit() {}
void SpritesWidget::setGrid(bool showGrid) {}

void SpritesWidget::on_btn_loadImage_clicked()
{
    QString imagesDir = m_loadedProject->projectPath() + "/images";
    QFileDialog dlg(this, tr("Choose the spritesheet file for your sprite."), imagesDir, "PNG files (*.png)");

    if (dlg.exec() != QDialog::Accepted)
        return;

    // If user has selected several files, we only use the first one

    QFileInfo spritesheetFile(dlg.selectedFiles().at(0));
    QDir imgDir(imagesDir);
    if (spritesheetFile.dir() != imgDir) {
        QFile::copy(spritesheetFile.filePath(), imagesDir + "/" + spritesheetFile.fileName());
    }

    auto* pSprite = m_loadedProject->spriteAt(m_currSpriteId);
    if (pSprite != nullptr) {
        std::string filename   = spritesheetFile.fileName().toStdString();
        pSprite->spriteSheetId = m_loadedProject->game().registerSpriteSheet(filename);
    }
    loadSpritesList();
}

void SpritesWidget::on_btn_newSprite_clicked()
{
    if (m_loadedProject == nullptr)
        return;

    m_loadedProject->createSprite();
    loadSpritesList();
}

void SpritesWidget::on_list_sprites_currentRowChanged(int row)
{
    setCurrentSprite(static_cast<Dummy::sprite_id>(row));
}

void SpritesWidget::on_check_useMultiDir_clicked(bool checked) {}

void SpritesWidget::on_check_useAnimation_clicked(bool checked) {}


} // namespace Editor
