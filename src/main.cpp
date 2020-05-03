#include <QApplication>

#include "widgets/generalWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Editor::GeneralWindow mainWindow;

    mainWindow.show();

    return QApplication::exec();
}
