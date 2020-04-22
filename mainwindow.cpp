#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QFileSystemModel *model, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), model(model), arcSettingsDialog(new ArcSettingsDialog)
{
    ui->setupUi(this);
    ui->listView->setModel(this->model);
    ui->listView->setRootIndex(this->model->index(QDir::rootPath()));

    connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(setRootIndex(const QModelIndex &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setRootIndex(const QModelIndex &index) {

    if (model->isDir(index)) {
        ui->listView->setRootIndex(index);
    }
     ui->listView->selectionModel()->clearSelection();
}

void MainWindow::on_back_clicked()
{
    if (ui->listView->rootIndex().isValid()) {

        ui->listView->setRootIndex(model->parent(ui->listView->rootIndex()));
    }
    ui->listView->selectionModel()->clearSelection();
}

void MainWindow::on_archive_clicked()
{
    QStringList list;
    foreach (const QModelIndex &index, ui->listView->selectionModel()->selectedIndexes()) {

        if (!model->isDir(index)) {

            list.append(model->filePath(index));
        }
    }
    if (!list.isEmpty()) {
        arcSettingsDialog->getComboBox()->clear();
        arcSettingsDialog->getComboBox()->addItem(model->filePath(ui->listView->rootIndex()));
        arcSettingsDialog->setSelectedItems(list);
        arcSettingsDialog->show();
    }
}

void MainWindow::on_unarchive_clicked()
{
    QStringList list;
    foreach (const QModelIndex &index, ui->listView->selectionModel()->selectedIndexes()) {

        if (!model->isDir(index)) {

            list.append(model->filePath(index));
        }
    }
    if (!list.isEmpty()) {
        arcSettingsDialog->setSelectedItems(list);
        arcSettingsDialog->decompress();
    }
}
