#include <QtDebug>
#include <QStorageInfo>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    
}

void MainWindow::showMountedVolumes()
{
    QList<QStorageInfo> mountedVolumes = QStorageInfo::mountedVolumes();
    qDebug() << "Mounted volumes:\n";
    for (auto volumne : mountedVolumes) {
        qDebug() << "Volume: " << volumne << "\n";
    }
}

MainWindow::~MainWindow()
{
    
}
