#include <QApplication>
#include <QList>
#include <QToolBar>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setGeometry(w.geometry().x(), w.geometry().y(), 1000, 600);

    QList<QToolBar *> toolbars = w.findChildren<QToolBar *>();
    for (auto toolbar : toolbars) {
        w.removeToolBar(toolbar);
    }

    w.showMountedVolumes();

    return a.exec();
}
