#include "RectangleWindow.h"
#include <testdialog.h>

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    TestDialog dlg;
    dlg.show();

    return app.exec();
}
