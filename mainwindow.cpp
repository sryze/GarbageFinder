#include <QtDebug>
#include <QStorageInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    delete ui;
}
