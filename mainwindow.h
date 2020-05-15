#pragma once

#include <QMainWindow>
#include <QFileSystemModel>
#include <QStringList>
#include <QLinkedList>
#include <QAction>
#include <QWidget>
#include <QMessageBox>
#include "stringlistmodel.h"

#include "arcsettingsdialog.h"

#include "decoding.h"
#include "decompressing.h"

#include <stdio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; class StringListModel; }
QT_END_NAMESPACE

class StringListModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QFileSystemModel *fileSystemModel, StringListModel *stringListModel, QWidget *parent = nullptr);
    ~MainWindow();

    void unarchiveFiles(const QString &outDir, const QStringList &selectedFiles);

public  slots:
    void setRootIndex(const QModelIndex &index);

    void customMenuRequested(QPoint pos);

    void newDirCreated(const QModelIndex &parent, int first, int last);

    void sortByName();
    void sortBySize();
    void sortByType();
    void sortByDate();

    void renameItem();
    void createDir();
    void copySelectedItems();
    void pasteFiles();

    void on_back_clicked();
    void on_add_clicked();
    void on_extract_clicked();
    void on_remove_clicked();

private:
    void recreateArchiveDir(const QString &outDir, const QString &archivePath, QString currentPath, QStringListIterator &iter);

    Ui::MainWindow *ui;

    QFileSystemModel *fileSystemModel;
    StringListModel *stringListModel;

    ArcSettingsDialog *arcSettingsDialog;

    QModelIndex currentParent;

    QProgressDialog *progress;

    QMenu *contextMenu;
    QMenu *viewMenu;

    QStringList copiedItems;
    QString newDir;
};

void unarchive(const QString &encodedFileName, const QString &decodedFileName);



