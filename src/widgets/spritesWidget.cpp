#include "widgets/spritesWidget.hpp"
#include "ui_spriteSelectionDialog.h"
#include "ui_spritesWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

#include "dummyrpg/dummy_types.hpp"

static const float ZOOM_MAX = 16.F;
static const float ZOOM_MIN = 0.5F;

namespace Editor {

///////////////////////////////////////////////////////////////////////////////

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
    loadSpritesList(m_loadedProject.get(), m_ui->list_sprites, m_ids);
}

void SpritesWidget::setCurrentSprite(Dummy::sprite_id id)
{
    if (m_loadedProject == nullptr)
        return;

    int idx      = -1;
    size_t nbIdx = m_ids.size();
    for (size_t i = 0; i < nbIdx; ++i)
        if (m_ids[i] == id) {
            idx = static_cast<int>(i);
            break;
        }

    m_ui->list_sprites->setCurrentRow(idx);
    m_currSpriteId = id;
    updateImage();
}

void SpritesWidget::mousePressEvent(QMouseEvent* e)
{
    if (! isOverImage(e->pos()))
        return;

    m_firstClick = mapToImage(e->pos());

    QRect rect(m_firstClick, m_firstClick);
    on_input_x1_valueChanged(rect.x());
    on_input_y1_valueChanged(rect.y());
    on_input_width_valueChanged(rect.width());
    on_input_height_valueChanged(rect.height());
    updateFields();
}

void SpritesWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (! isOverImage(e->pos()))
        return;

    QPoint otherClick = mapToImage(e->pos());
    QRect rect(m_firstClick, otherClick);
    rect = rect.normalized();
    on_input_x1_valueChanged(rect.x());
    on_input_y1_valueChanged(rect.y());
    on_input_width_valueChanged(rect.width());
    on_input_height_valueChanged(rect.height());
    updateFields();
}

void SpritesWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (! isOverImage(e->pos()))
        return;

    QPoint otherClick = mapToImage(e->pos());
    QRect rect(m_firstClick, otherClick);
    rect = rect.normalized();
    on_input_x1_valueChanged(rect.x());
    on_input_y1_valueChanged(rect.y());
    on_input_width_valueChanged(rect.width());
    on_input_height_valueChanged(rect.height());
    updateFields();
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

void SpritesWidget::updateFields()
{
    if (m_loadedProject == nullptr)
        return;
    const auto* pSprite = m_loadedProject->game().sprite(m_currSpriteId);
    if (pSprite == nullptr)
        return;

    const auto& sprite = *pSprite;

    // Block signal to avoid redundant signals
    m_ui->check_anim1->blockSignals(true);
    m_ui->check_anim2->blockSignals(true);
    m_ui->check_anim3->blockSignals(true);
    m_ui->check_anim4->blockSignals(true);
    m_ui->input_frameCount1->blockSignals(true);
    m_ui->input_frameCount2->blockSignals(true);
    m_ui->input_frameCount3->blockSignals(true);
    m_ui->input_frameCount4->blockSignals(true);
    m_ui->input_x1->blockSignals(true);
    m_ui->input_x2->blockSignals(true);
    m_ui->input_x3->blockSignals(true);
    m_ui->input_x4->blockSignals(true);
    m_ui->input_y1->blockSignals(true);
    m_ui->input_y2->blockSignals(true);
    m_ui->input_y3->blockSignals(true);
    m_ui->input_y4->blockSignals(true);
    m_ui->input_width->blockSignals(true);
    m_ui->input_height->blockSignals(true);
    m_ui->check_useMultiDir->blockSignals(true);

    // Set values
    m_ui->check_useMultiDir->setChecked(sprite.has4Directions);
    m_ui->input_width->setValue(sprite.width);
    m_ui->input_height->setValue(sprite.height);
    // Anim 1
    m_ui->check_anim1->setChecked(true);
    m_ui->check_anim1->setEnabled(false);
    m_ui->input_frameCount1->setValue(sprite.nbFrames);
    m_ui->input_x1->setValue(sprite.x);
    m_ui->input_y1->setValue(sprite.y);
    // Anim 2
    bool useAnim2 = sprite.nbFrames2 > 0;
    m_ui->check_anim2->setChecked(useAnim2);
    m_ui->check_anim2->setEnabled(sprite.nbFrames3 == 0); // Can only uncheck if next is empty
    m_ui->input_frameCount2->setEnabled(useAnim2);
    m_ui->input_frameCount2->setValue(sprite.nbFrames2);
    m_ui->input_x2->setEnabled(useAnim2);
    m_ui->input_x2->setValue(sprite.x2);
    m_ui->input_y2->setEnabled(useAnim2);
    m_ui->input_y2->setValue(sprite.y2);
    // anim3
    bool useAnim3 = sprite.nbFrames3 > 0;
    m_ui->check_anim3->setChecked(useAnim3);
    m_ui->check_anim3->setEnabled((sprite.nbFrames4 == 0 && useAnim3) || (sprite.nbFrames2 > 0 && ! useAnim3));
    m_ui->input_frameCount3->setEnabled(useAnim3);
    m_ui->input_frameCount3->setValue(sprite.nbFrames3);
    m_ui->input_x3->setEnabled(useAnim3);
    m_ui->input_x3->setValue(sprite.x3);
    m_ui->input_y3->setEnabled(useAnim3);
    m_ui->input_y3->setValue(sprite.y3);
    // anim4
    bool useAnim4 = sprite.nbFrames4 > 0;
    m_ui->check_anim4->setChecked(useAnim4);
    m_ui->check_anim4->setEnabled(useAnim4 || (sprite.nbFrames3 > 0 && ! useAnim4));
    m_ui->input_frameCount4->setEnabled(useAnim4);
    m_ui->input_frameCount4->setValue(sprite.nbFrames4);
    m_ui->input_x4->setEnabled(useAnim4);
    m_ui->input_x4->setValue(sprite.x4);
    m_ui->input_y4->setEnabled(useAnim4);
    m_ui->input_y4->setValue(sprite.y4);

    // Unblock signals
    m_ui->check_anim1->blockSignals(false);
    m_ui->check_anim2->blockSignals(false);
    m_ui->check_anim3->blockSignals(false);
    m_ui->check_anim4->blockSignals(false);
    m_ui->input_frameCount1->blockSignals(false);
    m_ui->input_frameCount2->blockSignals(false);
    m_ui->input_frameCount3->blockSignals(false);
    m_ui->input_frameCount4->blockSignals(false);
    m_ui->input_x1->blockSignals(false);
    m_ui->input_x2->blockSignals(false);
    m_ui->input_x3->blockSignals(false);
    m_ui->input_x4->blockSignals(false);
    m_ui->input_y1->blockSignals(false);
    m_ui->input_y2->blockSignals(false);
    m_ui->input_y3->blockSignals(false);
    m_ui->input_y4->blockSignals(false);
    m_ui->input_width->blockSignals(false);
    m_ui->input_height->blockSignals(false);
    m_ui->check_useMultiDir->blockSignals(false);

    updateImageDisplay();
}

void SpritesWidget::updateImage()
{
    const auto* sprite = m_loadedProject->game().sprite(m_currSpriteId);
    // Enable panels
    m_ui->panel_drawSprite->setEnabled(sprite != nullptr);
    m_ui->panel_preview->setEnabled(sprite != nullptr);

    if (sprite == nullptr)
        return;

    // Set the image and its name
    const auto& spritesSheet = m_loadedProject->game().spriteSheet(sprite->spriteSheetId);
    if (spritesSheet.empty()) {
        m_ui->label->setText(QString::number(m_currSpriteId) + " - " + tr("Undefined"));
        m_ui->image_center->setText(tr("Select a sprite sheet"));
        m_ui->image_center->setEnabled(false);
        m_loadedSpriteSheet = QPixmap();
        return;
    } else {
        QString sheetName   = QString::fromStdString(spritesSheet);
        QString sheetPath   = QString::fromStdString(m_loadedProject->game().spriteSheetPath(sprite->spriteSheetId));
        m_loadedSpriteSheet = QPixmap(sheetPath);
        m_ui->label->setText(QString::number(m_currSpriteId) + " - " + sheetName);
    }

    m_ui->input_width->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_height->setMaximum(m_loadedSpriteSheet.height());
    m_ui->input_x1->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_y1->setMaximum(m_loadedSpriteSheet.height());
    m_ui->input_x2->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_y2->setMaximum(m_loadedSpriteSheet.height());
    m_ui->input_x3->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_y3->setMaximum(m_loadedSpriteSheet.height());
    m_ui->input_x4->setMaximum(m_loadedSpriteSheet.width());
    m_ui->input_y4->setMaximum(m_loadedSpriteSheet.height());

    updateFields();
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
        const int imgW = displayImg.width();
        for (int x = 0; x < imgW; x += scaled(Dummy::TILE_SIZE))
            lines.push_back({{x, 0}, {x, displayImg.height()}});
        // Horizontal lines
        const int imgH = displayImg.height();
        for (int y = 0; y < imgH; y += scaled(Dummy::TILE_SIZE))
            lines.push_back({{0, y}, {displayImg.width(), y}});

        p.setPen(QColor(0, 0, 0, 150));
        p.drawLines(lines);
    }

    const auto* s = m_loadedProject->game().sprite(m_currSpriteId);
    if (s == nullptr)
        return; // ???

    // Selection rectangle
    std::vector<std::pair<QRect, QColor>> toDraw;
    for (uint8_t fr = 0; fr < s->nbFrames; ++fr) {
        toDraw.push_back({QRect(s->x + fr * s->width, s->y, s->width, s->height), fr == 0 ? Qt::red : Qt::magenta});
        if (s->has4Directions)
            toDraw.push_back({QRect(s->x + fr * s->width, s->y + s->height, s->width, s->height * 3), Qt::magenta});
    }
    for (uint8_t fr = 0; fr < s->nbFrames2; ++fr) {
        toDraw.push_back({QRect(s->x2 + fr * s->width, s->y2, s->width, s->height), Qt::magenta});
        if (s->has4Directions)
            toDraw.push_back({QRect(s->x2 + fr * s->width, s->y2 + s->height, s->width, s->height * 3), Qt::magenta});
    }
    for (uint8_t fr = 0; fr < s->nbFrames3; ++fr) {
        toDraw.push_back({QRect(s->x3 + fr * s->width, s->y3, s->width, s->height), Qt::magenta});
        if (s->has4Directions)
            toDraw.push_back({QRect(s->x3 + fr * s->width, s->y3 + s->height, s->width, s->height * 3), Qt::magenta});
    }
    for (uint8_t fr = 0; fr < s->nbFrames4; ++fr) {
        toDraw.push_back({QRect(s->x4 + fr * s->width, s->y4, s->width, s->height), Qt::magenta});
        if (s->has4Directions)
            toDraw.push_back({QRect(s->x4 + fr * s->width, s->y4 + s->height, s->width, s->height * 3), Qt::magenta});
    }

    for (const auto& pair : toDraw) {
        const auto& rec   = pair.first;
        const auto& color = pair.second;
        QRect scaledRect(scaled(rec.x()), scaled(rec.y()), scaled(rec.width()) - 1, scaled(rec.height()) - 1);

        p.setPen(color);
        p.drawRect(scaledRect);
    }

    m_ui->image_center->setEnabled(true);
    m_ui->image_center->setPixmap(displayImg);
}

void SpritesWidget::loadSpritesList(const Editor::Project* p, QListWidget* list, std::vector<Dummy::sprite_id> ids)
{
    if (p == nullptr) {
        list->clear();
        return;
    }

    list->blockSignals(true);
    list->clear();

    ids.clear();
    for (const auto& sprite : p->game().sprites()) {
        QString spritesheet = QString::fromStdString(p->game().spriteSheet(sprite.second.spriteSheetId));
        if (spritesheet.isEmpty())
            spritesheet = tr("Undefined");
        list->addItem(QString::number(sprite.second.id) + " - " + spritesheet);

        ids.push_back(sprite.second.id);
    }

    list->blockSignals(false);
}

void SpritesWidget::zoomIn()
{
    m_zoom = std::min(m_zoom * 2.F, ZOOM_MAX);
    updateImageDisplay();
}

void SpritesWidget::zoomOut()
{
    m_zoom = std::max(m_zoom / 2.F, ZOOM_MIN);
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

    auto* pSprite = m_loadedProject->game().sprite(m_currSpriteId);
    if (pSprite != nullptr) {
        std::string filename   = spritesheetFile.fileName().toStdString();
        pSprite->spriteSheetId = m_loadedProject->game().registerSpriteSheet(filename);
    }
    loadSpritesList(m_loadedProject.get(), m_ui->list_sprites, m_ids);
}

void SpritesWidget::on_btn_newSprite_clicked()
{
    if (m_loadedProject == nullptr)
        return;

    auto id = m_loadedProject->game().registerSprite();
    m_loadedProject->changed();

    loadSpritesList(m_loadedProject.get(), m_ui->list_sprites, m_ids);
    setCurrentSprite(id);
    m_loadedProject->changed();
}

void SpritesWidget::on_btn_delete_clicked()
{
    if (m_loadedProject == nullptr)
        return;

    auto btn =
        QMessageBox::question(this, tr("Confirmation"), tr("You are about to delete this sprite. Are you sure?"));
    if (btn != QMessageBox::Yes)
        return;

    m_loadedProject->game().unregisterSprite(m_currSpriteId);

    loadSpritesList(m_loadedProject.get(), m_ui->list_sprites, m_ids);
    setCurrentSprite(Dummy::undefSprite);
    m_loadedProject->changed();
}

void SpritesWidget::on_list_sprites_currentRowChanged(int row)
{
    size_t idx = static_cast<size_t>(row);
    if (idx < m_ids.size())
        setCurrentSprite(m_ids[idx]);
}

void SpritesWidget::on_check_useMultiDir_clicked(bool checked)
{
    m_loadedProject->game().sprite(m_currSpriteId)->has4Directions = checked;
    m_loadedProject->changed();
    updateImageDisplay();
}

void SpritesWidget::on_input_width_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->width = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_height_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->height = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
///////////////////////////////////////////////////////////////////////////////
// Animation 1
void SpritesWidget::on_check_anim1_clicked(bool checked)
{
    uint8_t val = static_cast<uint8_t>(std::min(m_ui->input_frameCount1->value(), 1));

    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames = checked ? val : 0;
    m_loadedProject->changed();
    updateFields();
}
void SpritesWidget::on_input_frameCount1_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames = static_cast<uint8_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_x1_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->x = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_y1_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->y = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
///////////////////////////////////////////////////////////////////////////////
// Animation 2
void SpritesWidget::on_check_anim2_clicked(bool checked)
{
    uint8_t val = static_cast<uint8_t>(std::min(m_ui->input_frameCount2->value(), 1));

    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames2 = checked ? val : 0;
    m_loadedProject->changed();
    updateFields();
}
void SpritesWidget::on_input_frameCount2_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames2 = static_cast<uint8_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_x2_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->x2 = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_y2_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->y2 = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
///////////////////////////////////////////////////////////////////////////////
// Animation 3
void SpritesWidget::on_check_anim3_clicked(bool checked)
{
    uint8_t val = static_cast<uint8_t>(std::min(m_ui->input_frameCount3->value(), 1));

    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames3 = checked ? val : 0;
    m_loadedProject->changed();
    updateFields();
}
void SpritesWidget::on_input_frameCount3_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames3 = static_cast<uint8_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_x3_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->x3 = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_y3_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->y3 = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
///////////////////////////////////////////////////////////////////////////////
// Animation 4
void SpritesWidget::on_check_anim4_clicked(bool checked)
{
    uint8_t val = static_cast<uint8_t>(std::min(m_ui->input_frameCount4->value(), 1));

    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames4 = checked ? val : 0;
    m_loadedProject->changed();
    updateFields();
}
void SpritesWidget::on_input_frameCount4_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->nbFrames4 = static_cast<uint8_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_x4_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->x4 = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
void SpritesWidget::on_input_y4_valueChanged(int val)
{
    m_loadedProject->game().sprite(m_currSpriteId)->y4 = static_cast<uint16_t>(val);
    m_loadedProject->changed();
    updateImageDisplay();
}
///////////////////////////////////////////////////////////////////////////////

SpriteSelectionDialog::SpriteSelectionDialog(std::shared_ptr<Editor::Project> project, QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::spriteSelectionDialog)
    , m_loadedProject(project)
{
    m_ui->setupUi(this);
    SpritesWidget::loadSpritesList(m_loadedProject.get(), m_ui->list_sprites, m_ids);
}

SpriteSelectionDialog::~SpriteSelectionDialog() {}

void SpriteSelectionDialog::setCurrentSprite(Dummy::sprite_id id)
{
    if (m_loadedProject == nullptr)
        return;

    const auto& game = m_loadedProject->game();

    m_currSpriteId     = id;
    const auto* sprite = game.sprite(id);
    if (sprite == nullptr) {
        m_ui->lbl_spriteName->setText(tr("Select a sprite"));
        m_ui->list_sprites->blockSignals(true);
        m_ui->list_sprites->setCurrentRow(-1);
        m_ui->list_sprites->blockSignals(false);
    } else {
        QString spritesheet = QString::fromStdString(game.spriteSheet(sprite->spriteSheetId));
        if (spritesheet.isEmpty())
            spritesheet = tr("Undefined");

        m_ui->lbl_spriteName->setText(QString::number(id) + " - " + spritesheet);
        m_ui->list_sprites->blockSignals(true);
        m_ui->list_sprites->setCurrentRow(id);
        m_ui->list_sprites->blockSignals(false);
    }
}

void SpriteSelectionDialog::on_list_sprites_clicked(const QModelIndex& index)
{
    size_t idx = static_cast<size_t>(index.row());
    if (idx < m_ids.size())
        setCurrentSprite(m_ids[idx]);
}

} // namespace Editor
