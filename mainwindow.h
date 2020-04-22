#pragma once

#include <QMainWindow>
#include <QListView>
#include <QFileSystemModel>
#include <QStringList>
#include "arcsettingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QFileSystemModel *model, QWidget *parent = nullptr);
    ~MainWindow();

public  slots:
    void setRootIndex(const QModelIndex &index);


private slots:
    void on_back_clicked();

    void on_archive_clicked();

    void on_unarchive_clicked();

private:
    Ui::MainWindow *ui;

    QFileSystemModel *model;

    ArcSettingsDialog *arcSettingsDialog;
};
