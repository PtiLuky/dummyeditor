#ifndef MAPEDITDIALOG_H
#define MAPEDITDIALOG_H

#include <QDialog>
#include <memory>

#include "editor/project.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class MapEditDialog;
}

//////////////////////////////////////////////////////////////////////////////
//  MapEditDialog class
//////////////////////////////////////////////////////////////////////////////

class MapEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MapEditDialog(QWidget* parent = nullptr);
    virtual ~MapEditDialog(); // we need this because of smart ptr + forward declaration..

    void setup(const Editor::Project& project, const Dummy::Map*, const QString& mapName);

    QString getMapName() const;
    uint16_t getWidth() const;
    uint16_t getHeight() const;
    QString getChipset() const;
    QString getMusic() const;

public slots:
    void on_pushButtonBrowseChipset_clicked();
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();

private:
    bool inputsAreValid(QString* errorMessage = nullptr);
    std::unique_ptr<Ui::MapEditDialog> m_ui;
    QString m_chipsetPath;
};

#endif // MAPEDITDIALOG_H
