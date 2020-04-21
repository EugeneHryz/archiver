#include "mainwindow.h"

#include <QApplication>
#include <QFileSystemModel>
#include <QListView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath("");

    MainWindow *mainWindow = new MainWindow(model);

    mainWindow->show();
    return a.exec();
}

