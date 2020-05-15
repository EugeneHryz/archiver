#include "mainwindow.h"

#include <QApplication>
#include <QFileSystemModel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFileSystemModel *fileSystemModel = new QFileSystemModel;
    fileSystemModel->setRootPath("C:/");
    fileSystemModel->setReadOnly(false);

    QStringList list = QStringList();
    StringListModel *stringListModel = new StringListModel(list);

    MainWindow *mainWindow = new MainWindow(fileSystemModel, stringListModel);
    mainWindow->setWindowTitle("Archiver");

    mainWindow->show();
    return a.exec();
}

