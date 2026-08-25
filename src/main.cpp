#include "ui/MainWindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication::setApplicationName("ASCII Image Converter");
    QApplication::setOrganizationName("scritshell");

    ascii_converter::ui::MainWindow window;
    window.show();

    return app.exec();
}
