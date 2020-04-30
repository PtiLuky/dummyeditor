#include "widgets/mapEditDialog.hpp"
#include "ui_mapeditdialog.h"

#include <QFileDialog>
#include <QMessageBox>


MapEditDialog::MapEditDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::MapEditDialog)
{
    m_ui->setupUi(this);
}

MapEditDialog::~MapEditDialog() {}

void MapEditDialog::setup(const Editor::Project& project, const Dummy::Map* map)
{
    if (map != nullptr) {
        // m_ui->lineEditMapName->setText(map.);
        // m_ui->lineEditChipset->setText(QString::fromStdString(map->chipset()));
        // m_ui->lineEditMusic->setText(QString::fromStdString(map->music()));
        m_ui->spinBoxMapHeight->setValue(map->height());
        m_ui->spinBoxMapWidth->setValue(map->width());
    }


    // cleanPath() uses slashes, remove weird paths as "folder/../folder"
    m_chipsetPath = QDir::cleanPath(project.projectPath() + "/images");
}

QString MapEditDialog::getMapName() const
{
    return m_ui->lineEditMapName->text();
}

uint16_t MapEditDialog::getWidth() const
{
    return static_cast<uint16_t>(m_ui->spinBoxMapWidth->value());
}

uint16_t MapEditDialog::getHeight() const
{
    return static_cast<uint16_t>(m_ui->spinBoxMapHeight->value());
}

QString MapEditDialog::getChipset() const
{
    return m_ui->lineEditChipset->text();
}

QString MapEditDialog::getMusic() const
{
    return m_ui->lineEditMusic->text();
}

bool MapEditDialog::inputsAreValid(QString* errorMessage)
{
    if (m_ui->lineEditMapName->text().isEmpty()) {
        if (errorMessage != nullptr)
            *errorMessage = tr("You must enter a map name.");

        return false;

    } else if (m_ui->lineEditChipset->text().isEmpty()) {
        if (errorMessage != nullptr)
            *errorMessage = tr("You must enter a tileset filename.");

        return false;

    } else if (m_ui->spinBoxMapHeight->value() < 1) { // should not happen
        if (errorMessage != nullptr)
            *errorMessage = tr("The map's height must be above or equal to 1.");

        return false;

    } else if (m_ui->spinBoxMapWidth->value() < 1) { // should not happen
        if (errorMessage != nullptr)
            *errorMessage = tr("The map's width must be above or equal to 1.");

        return false;
    }
    return true;
}

void MapEditDialog::on_pushButtonBrowseChipset_clicked()
{
    QFileDialog dlg(this, tr("Choose the tileset file for your map."), m_chipsetPath, "PNG files (*.png)");

    if (dlg.exec() != QDialog::Accepted)
        return;

    // If user has selected several files, we only use the first one

    QString selectedChipset     = QDir::cleanPath(dlg.selectedFiles().at(0));
    int indexOfFileNameInString = selectedChipset.indexOf(QDir::cleanPath(m_chipsetPath));
    if (indexOfFileNameInString < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please select a tileset inside the 'images' folder."));
    } else {
        m_ui->lineEditChipset->setText(selectedChipset.mid(indexOfFileNameInString + 1 + m_chipsetPath.size()));
    }
}

void MapEditDialog::on_pushButtonOK_clicked()
{
    QString errorMessage;
    if (inputsAreValid(&errorMessage)) {
        accept();
    } else {
        QMessageBox::critical(this, tr("Error"), errorMessage);
    }
}

void MapEditDialog::on_pushButtonCancel_clicked()
{
    reject();
}
