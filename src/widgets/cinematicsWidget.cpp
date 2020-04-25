#include "widgets/cinematicsWidget.hpp"
#include "ui_CinematicsWidget.h"

namespace Editor {



CinematicsWidget::CinematicsWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::CinematicsWidget)
{
    m_ui->setupUi(this);
}

CinematicsWidget::~CinematicsWidget() {}

} // namespace Editor
