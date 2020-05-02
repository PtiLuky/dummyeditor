#include "widgets/spritesWidget.hpp"
#include "ui_spritesWidget.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>

#include "dummyrpg/dummy_types.hpp"

static const float ZOOM_MAX = 16.f;
static const float ZOOM_MIN = 0.5f;

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

void SpritesWidget::mousePressEvent(QMouseEvent* e)
{
    if (! isOverImage(e->pos()))
        return;

    QPoint clickPos = mapToImage(e->pos());
    m_firstClick    = clickPos;
    setRect(QRect(m_firstClick, m_firstClick));
}

void SpritesWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (! isOverImage(e->pos()))
        return;

    QPoint otherClick = mapToImage(e->pos());
    setRect(QRect(m_firstClick, otherClick).normalized());
}

void SpritesWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (! isOverImage(e->pos()))
        return;

    QPoint otherClick = mapToImage(e->pos());
    setRect(QRect(m_firstClick, otherClick).normalized());
}

bool SpritesWidget::isOverImage(const QPoint& p) const
{
    QPoint scrollTL = m_ui->scrollwrapper->mapTo(this, m_ui->scrollwrapper->rect().topLeft());
    QPoint scrollBR = m_ui->scrollwrapper->mapTo(this, m_ui->scrollwrapper->rect().bottomRight());
    return QRect(scrollTL, scrollBR).contains(p);
}

void SpritesWidget::wheelEvent(QWheelEvent* e)
{
    if (e->modifiers().testFlag(Qt::ControlModifier) && (e->delta() > 0)) {
        zoomIn();
    } else if (e->modifiers().testFlag(Qt::ControlModifier) && (e->delta() < 0)) {
        zoomOut();
    }
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
        m_loadedSpriteSheet = QPixmap();
        return;
    }

    m_ui->input_x->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_width->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_y->setMaximum(m_loadedSpriteSheet.height());
    m_ui->input_height->setMaximum(m_loadedSpriteSheet.height());

    updateImageDisplay();
}

void SpritesWidget::updateImageDisplay()
{
    if (m_loadedSpriteSheet.isNull())
        return;

    QPixmap displayImg = m_loadedSpriteSheet.scaledToWidth(static_cast<int>(m_loadedSpriteSheet.width() * m_zoom));

    // Pain overlays
    QPainter p(&displayImg);
    // grid
    if (m_showGrid) {
        QVector<QLine> lines;
        // vectical lines
        for (int x = 0; x < displayImg.width(); x += scaled(Dummy::TILE_SIZE))
            lines.push_back({{x, 0}, {x, displayImg.height()}});
        // Horizontal lines
        for (int y = 0; y < displayImg.height(); y += scaled(Dummy::TILE_SIZE))
            lines.push_back({{0, y}, {displayImg.width(), y}});

        p.setPen(QColor(0, 0, 0, 150));
        p.drawLines(lines);
    }
    // Selection rectangle
    if (! m_activeRect.isNull()) {
        QRect scaledRect(scaled(m_activeRect.x()), scaled(m_activeRect.y()), //
                         scaled(m_activeRect.width()) - 1, scaled(m_activeRect.height()) - 1);

        p.setPen(Qt::red);
        p.drawRect(scaledRect);
    }

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

void SpritesWidget::setRect(const QRect& rect)
{
    m_activeRect = rect.normalized();

    // block signals
    m_ui->input_x->blockSignals(true);
    m_ui->input_y->blockSignals(true);
    m_ui->input_width->blockSignals(true);
    m_ui->input_height->blockSignals(true);

    // And set the new values
    m_ui->input_x->setValue(rect.x());
    m_ui->input_y->setValue(rect.y());
    m_ui->input_width->setValue(rect.width());
    m_ui->input_height->setValue(rect.height());

    m_ui->input_x->blockSignals(false);
    m_ui->input_y->blockSignals(false);
    m_ui->input_width->blockSignals(false);
    m_ui->input_height->blockSignals(false);

    auto* pSprite = m_loadedProject->spriteAt(m_currSpriteId);
    if (pSprite != nullptr) {
        pSprite->x      = static_cast<uint16_t>(rect.x());
        pSprite->y      = static_cast<uint16_t>(rect.y());
        pSprite->width  = static_cast<uint16_t>(rect.width());
        pSprite->height = static_cast<uint16_t>(rect.height());
    }
    updateImageDisplay();
}

void SpritesWidget::zoomIn()
{
    m_zoom = std::min(m_zoom * 2.f, ZOOM_MAX);
    updateImageDisplay();
}

void SpritesWidget::zoomOut()
{
    m_zoom = std::max(m_zoom / 2.f, ZOOM_MIN);
    updateImageDisplay();
}

void SpritesWidget::zoomOne()
{
    m_zoom = 1;
    updateImageDisplay();
}

void SpritesWidget::setGrid(bool showGrid)
{
    m_showGrid = showGrid;
    updateImageDisplay();
}

int SpritesWidget::scaled(int in) const
{
    return static_cast<int>(in * m_zoom);
}

QPoint SpritesWidget::mapToImage(const QPoint& pt) const
{
    const QPoint imgPos = m_ui->image_center->mapTo(this, {0, 0});

    QPoint posInImg = pt - imgPos;
    if (m_zoom > 0) {
        posInImg.setX(static_cast<int>(posInImg.x() / m_zoom));
        posInImg.setY(static_cast<int>(posInImg.y() / m_zoom));
    }

    QPoint outPt;
    QSize maxSize = m_loadedSpriteSheet.size();
    outPt.setX(std::max(std::min(posInImg.x(), maxSize.width() - 1), 0));
    outPt.setY(std::max(std::min(posInImg.y(), maxSize.height() - 1), 0));

    return outPt;
}

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

void SpritesWidget::on_input_x_valueChanged(int val)
{
    m_activeRect.setX(val);
    setRect(m_activeRect);
}
void SpritesWidget::on_input_y_valueChanged(int val)
{
    m_activeRect.setY(val);
    setRect(m_activeRect);
}
void SpritesWidget::on_input_width_valueChanged(int val)
{
    m_activeRect.setWidth(val);
    setRect(m_activeRect);
}
void SpritesWidget::on_input_height_valueChanged(int val)
{
    m_activeRect.setHeight(val);
    setRect(m_activeRect);
}

} // namespace Editor
