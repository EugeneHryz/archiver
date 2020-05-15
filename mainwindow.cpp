#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QFileSystemModel *fileSystemModel, StringListModel *stringListModel, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow), fileSystemModel(fileSystemModel), stringListModel(stringListModel),
      arcSettingsDialog(new ArcSettingsDialog), contextMenu(new QMenu(this)), viewMenu(new QMenu(this)), copiedItems(QStringList())
{
    ui->setupUi(this);
    ui->tableView->setModel(this->fileSystemModel);
    ui->tableView->setRootIndex(this->fileSystemModel->index(QDir::rootPath()));
    currentParent = ui->tableView->rootIndex();
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setCornerButtonEnabled(false);
    ui->tableView->setShowGrid(false);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setFocusPolicy(Qt::NoFocus);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tableView->setColumnWidth(0, 250);
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    QHeaderView *headerView = ui->tableView->verticalHeader();
    headerView->setSectionResizeMode(QHeaderView::Fixed);
    headerView->setDefaultSectionSize(20);
    headerView->setVisible(false);

    QAction *addAction = new QAction("Add", this);
    QAction *extractAction = new QAction("Extract", this);
    QAction *copyAction = new QAction("Copy", this);
    QAction *pasteAction = new QAction("Paste", this);
    QAction *renameAction = new QAction("Rename", this);
    QAction *createDirAction = new QAction("New Folder", this);
    QAction *deleteAction = new QAction("Delete", this);

    QAction *selectAllAction = new QAction("Select All", this);
    QAction *clearSelectionAction = new QAction("Clear All", this);
    QAction *sortByNameAction = new QAction("Name", this);
    QAction *sortByTypeAction = new QAction("Type", this);
    QAction *sortBySizeAction = new QAction("Size", this);
    QAction *sortByDateAction = new QAction("Date", this);

    contextMenu->addAction(addAction);
    contextMenu->addAction(extractAction);
    contextMenu->addAction(copyAction);
    contextMenu->addAction(pasteAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(createDirAction);
    contextMenu->addAction(deleteAction);

    connect(addAction, SIGNAL(triggered()), this, SLOT(on_add_clicked()));
    connect(extractAction, SIGNAL(triggered()), this, SLOT(on_extract_clicked()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copySelectedItems()));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteFiles()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(renameItem()));
    connect(createDirAction, SIGNAL(triggered()), this, SLOT(createDir()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(on_remove_clicked()));

    connect(selectAllAction, SIGNAL(triggered()), ui->tableView, SLOT(selectAll()));
    connect(clearSelectionAction, SIGNAL(triggered()), ui->tableView, SLOT(clearSelection()));
    connect(sortByNameAction, SIGNAL(triggered()), this, SLOT(sortByName()));
    connect(sortByTypeAction, SIGNAL(triggered()), this, SLOT(sortByType()));
    connect(sortBySizeAction, SIGNAL(triggered()), this, SLOT(sortBySize()));
    connect(sortByDateAction, SIGNAL(triggered()), this, SLOT(sortByDate()));

    QMenu *mainMenu = ui->menubar->addMenu("File");
    mainMenu->addAction(addAction);
    mainMenu->addAction(extractAction);
    mainMenu->addAction(copyAction);
    mainMenu->addAction(pasteAction);
    mainMenu->addAction(renameAction);
    mainMenu->addAction(createDirAction);
    mainMenu->addAction(deleteAction);

    QMenu *editMenu = ui->menubar->addMenu("Edit");
    editMenu->addAction(selectAllAction);
    editMenu->addAction(clearSelectionAction);

    viewMenu = ui->menubar->addMenu("View");
    viewMenu->addAction(sortByNameAction);
    viewMenu->addAction(sortByTypeAction);
    viewMenu->addAction(sortBySizeAction);
    viewMenu->addAction(sortByDateAction);

    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(setRootIndex(const QModelIndex &)));
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));
    connect(fileSystemModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(newDirCreated(const QModelIndex &, int, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setRootIndex(const QModelIndex &index) {

    if (ui->tableView->model() == fileSystemModel) {

        QModelIndex index = ui->tableView->selectionModel()->selectedRows(0).at(0);

        if (fileSystemModel->isDir(index)) {

            ui->tableView->setRootIndex(index);
            currentParent = index;

        } else {

            QFileInfo fileInfo = fileSystemModel->filePath(index);

            if (fileInfo.completeSuffix().endsWith("lzh")) {

                Strings *list = get_archived_file_names(fileSystemModel->filePath(index).toLocal8Bit().constData());
                QStringList fileNames;

                while (list != NULL) {

                    fileNames << QString(list->string);
                    list = list->next;
                }

                stringListModel->setAcrhivedFileNames(fileNames);
                stringListModel->setArchivePath(fileSystemModel->filePath(index));
                ui->tableView->setModel(stringListModel);

                QList<QAction*> actions = contextMenu->actions();
                for (int i = 0; i < actions.size(); i++) {

                    if (i == 1) {
                        actions.at(i)->setDisabled(false);
                    } else {
                        actions.at(i)->setDisabled(true);
                    }
                }

                viewMenu->setDisabled(true);
                ui->add->setDisabled(true);
                ui->remove->setDisabled(true);
            }
        }
    }

    ui->tableView->selectionModel()->clearSelection();
}

void MainWindow::on_back_clicked()
{
    if (ui->tableView->model() == stringListModel) {

        ui->tableView->setModel(fileSystemModel);
        ui->tableView->setRootIndex(currentParent);

        QList<QAction*> actions = contextMenu->actions();
        for (int i = 0; i < actions.size(); i++) {

            actions.at(i)->setDisabled(false);
        }

        viewMenu->setDisabled(false);
        ui->add->setDisabled(false);
        ui->remove->setDisabled(false);

    } else {

        currentParent = fileSystemModel->parent(currentParent);
        ui->tableView->setRootIndex(currentParent);
    }

    ui->tableView->selectionModel()->clearSelection();
}

void MainWindow::on_add_clicked()
{
    QStringList filePaths;
    QFileInfo fileInfo;

    foreach (const QModelIndex &index, ui->tableView->selectionModel()->selectedRows()) {

        fileInfo.setFile(fileSystemModel->filePath(index));

        if (fileInfo.isDir()) {

            filePaths.append(fileInfo.absoluteFilePath());
            QDirIterator iter(fileInfo.absoluteFilePath(), QDirIterator::Subdirectories);

            while (iter.hasNext()) {

                filePaths.append(iter.next());
            }

        } else {

            if (fileInfo.isFile()) {

                filePaths.append(fileSystemModel->filePath(index));
            }
        }
    }

    bool validFiles = true;
    QList<QString>::iterator iter;

    for (iter = filePaths.begin(); iter != filePaths.end() && validFiles; iter++) {

        fileInfo.setFile(*iter);
        if (fileInfo.isFile() && fileInfo.completeSuffix().endsWith("lzh")) {

            validFiles = false;

            QMessageBox messageBox;
            messageBox.setWindowTitle("Message");
            messageBox.setText("Archived file(s) cannot be archived again.");
            messageBox.setStandardButtons(QMessageBox::Ok);
            messageBox.exec();
        }
    }

    if (validFiles && !filePaths.isEmpty()) {

        QString archiveName;

        foreach (const QString &filePath, filePaths) {

            fileInfo.setFile(filePath);
            if (fileInfo.isFile()) {

                archiveName += fileSystemModel->filePath(currentParent);
                if (ui->tableView->rootIndex() != fileSystemModel->index(QDir::rootPath())) {

                    archiveName += "/";
                }
                archiveName += fileInfo.baseName() + ".lzh";

                break;
            }
        }

        if (!archiveName.isEmpty()) {

            arcSettingsDialog->getComboBox()->addItem(archiveName);
            arcSettingsDialog->getComboBox()->setCurrentText(archiveName);
            arcSettingsDialog->setSelectedItems(filePaths);
            arcSettingsDialog->show();
            ui->tableView->clearSelection();
        }
    }
}

void unarchiveText(const QString &encodedFileName, const QString &decompressedFileName, const QString &fileToExtract) {

    FILE *encodedFile = NULL;
    fopen_s(&encodedFile, encodedFileName.toLocal8Bit().constData(), "rb+");

    FILE *decodedFile =  NULL;
    tmpfile_s(&decodedFile);

    if (encodedFile == NULL || decodedFile == NULL) {
        return;
    }

    if (decode_file(&encodedFile, decodedFile, encodedFileName.toLocal8Bit().constData(), fileToExtract.toLocal8Bit().constData())) {

        FILE *decompressedFile = NULL;
        fopen_s(&decompressedFile, decompressedFileName.toLocal8Bit().constData(), "wb");

        decompress_file(decodedFile, decompressedFile);
        fclose(decompressedFile);
    }

    fclose(decodedFile);
    fclose(encodedFile);
}

void unarchiveBin(const QString &encodedFileName, const QString &decodedFileName, const QString &fileToExtract) {

    FILE *encodedFile = NULL;
    fopen_s(&encodedFile, encodedFileName.toLocal8Bit().constData(), "rb+");

    FILE *decodedFile =  NULL;
    fopen_s(&decodedFile, decodedFileName.toLocal8Bit().constData(), "wb");

    if (encodedFile == NULL || decodedFile == NULL) {
        return;
    }

    decode_file(&encodedFile, decodedFile, encodedFileName.toLocal8Bit().constData(), fileToExtract.toLocal8Bit().constData());

    fclose(decodedFile);
    fclose(encodedFile);
}

void MainWindow::unarchiveFiles(const QString &outDir, const QStringList &selectedFiles) {

    if (!selectedFiles.isEmpty()) {

        QProgressDialog progress("Unarchiving...", "Cancel", 0, selectedFiles.size(), this);
        progress.setWindowTitle("Unarchiving...");
        progress.setWindowModality(Qt::WindowModal);
        progress.setCancelButton(nullptr);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.open();

        if (!selectedFiles.at(0).endsWith(".lzh") && !QFileInfo(selectedFiles.at(0)).exists()) {

            QString archivePath = stringListModel->getArchivePath();

            QDir dir = QDir(outDir + "/" + QFileInfo(archivePath).baseName());
            if (!dir.exists()) {

                dir.mkdir(dir.path());
            }

            progress.setLabelText("Unarchiving " + archivePath + "...");

            QStringListIterator iter = QStringListIterator(selectedFiles);
            recreateArchiveDir(dir.path(), archivePath, "/", iter);

            progress.setValue(selectedFiles.size());

            on_back_clicked();

        } else {

            if (selectedFiles.at(0).endsWith(".lzh")) {

                foreach (const QString &archivePath, selectedFiles) {

                    progress.setLabelText("Unarchiving " + archivePath + "...");

                    QDir dir = QDir(outDir + "/" + QFileInfo(archivePath).completeBaseName());
                    if (!dir.exists()) {

                        dir.mkdir(dir.path());
                    }

                    QStringList archivedFiles;
                    Strings *list = get_archived_file_names(archivePath.toLocal8Bit().constData());

                    while (list != NULL) {

                        archivedFiles << QString(list->string);
                        list = list->next;
                    }

                    QStringListIterator iter = QStringListIterator(archivedFiles);
                    recreateArchiveDir(dir.path(), archivePath, "/", iter);

                    progress.setValue(progress.value() + 1);
                }
            }
        }
    }
}

void MainWindow::on_extract_clicked()
{
    QStringList selectedFiles;
    QFileInfo fileInfo;

    foreach (const QModelIndex &index, ui->tableView->selectionModel()->selectedRows()) {

        if (ui->tableView->model() == stringListModel) {

            selectedFiles.append(stringListModel->data(index, Qt::DisplayRole).toString());

        } else {

            fileInfo.setFile(fileSystemModel->filePath(index));

            if (fileInfo.isFile()) {

                if (isArchive(fileSystemModel->filePath(index))) {

                    selectedFiles.append(fileSystemModel->filePath(index));

                } else {

                    selectedFiles.clear();

                    QMessageBox messageBox;
                    messageBox.setWindowTitle("Message");
                    messageBox.setText("File is not defined as archive.");
                    messageBox.exec();

                    break;
                }
            }
        }
    }

    if (!selectedFiles.isEmpty()) {

        QString outDir = QFileDialog::getExistingDirectory(this, "Select directory", fileSystemModel->filePath(currentParent));

        if (!outDir.isEmpty() && QFileInfo(outDir).exists()) {

            unarchiveFiles(outDir, selectedFiles);
            ui->tableView->clearSelection();
        }
    }
}

void MainWindow::recreateArchiveDir(const QString &outDir, const QString &archivePath, QString currentPath, QStringListIterator &iter) {

    if (iter.hasNext()) {

        int start = currentPath.lastIndexOf(QChar('/')) + 1;
        int end = iter.peekNext().indexOf("/", start);
        if (end == -1) {

            QString relativePath = iter.next();
            if ((outDir + relativePath).endsWith("txt")) {

                unarchiveText(archivePath, outDir + relativePath, relativePath);

            } else {

                unarchiveBin(archivePath, outDir + relativePath, relativePath);
            }

            recreateArchiveDir(outDir, archivePath, "/", iter);

        } else {

            QDir dir = QDir(outDir + (currentPath + iter.peekNext().mid(start, end - start + 1)).chopped(1));
            if (!dir.exists()) {

                dir.mkdir(dir.path());
            }
            recreateArchiveDir(outDir, archivePath, currentPath + iter.peekNext().mid(start, end - start + 1), iter);
        }
    }
}

void MainWindow::on_remove_clicked()
{
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();

    if (!selectedRows.isEmpty()) {

        QMessageBox messageBox;
        messageBox.setWindowTitle("Delete files");
        if (selectedRows.size() == 1) {

            messageBox.setInformativeText("Are you sure you want to delete this item?");

        } else {

            messageBox.setInformativeText("Are you sure you want to delete these items?");
        }
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int result = messageBox.exec();

        if (result == QMessageBox::Yes) {

            foreach (const QModelIndex &index, ui->tableView->selectionModel()->selectedRows()) {

                fileSystemModel->remove(index);
            }
        }
    }
}

void MainWindow::customMenuRequested(QPoint pos) {

    contextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::sortByName() {

    QHeaderView *header = ui->tableView->horizontalHeader();
    if (header->sortIndicatorOrder() == Qt::AscendingOrder) {

        fileSystemModel->sort(0, Qt::DescendingOrder);
        header->setSortIndicator(0, Qt::DescendingOrder);

    } else {

        fileSystemModel->sort(0, Qt::AscendingOrder);
        header->setSortIndicator(0, Qt::AscendingOrder);
    }
}

void MainWindow::sortBySize() {

    QHeaderView *header = ui->tableView->horizontalHeader();
    if (header->sortIndicatorOrder() == Qt::AscendingOrder) {

        fileSystemModel->sort(1, Qt::DescendingOrder);
        header->setSortIndicator(1, Qt::DescendingOrder);

    } else {

        fileSystemModel->sort(1, Qt::AscendingOrder);
        header->setSortIndicator(1, Qt::AscendingOrder);
    }
}

void MainWindow::sortByType() {

    QHeaderView *header = ui->tableView->horizontalHeader();
    if (header->sortIndicatorOrder() == Qt::AscendingOrder) {

        fileSystemModel->sort(2, Qt::DescendingOrder);
        header->setSortIndicator(2, Qt::DescendingOrder);

    } else {

        fileSystemModel->sort(2, Qt::AscendingOrder);
        header->setSortIndicator(2, Qt::AscendingOrder);
    }
}

void MainWindow::sortByDate() {

    QHeaderView *header = ui->tableView->horizontalHeader();
    if (header->sortIndicatorOrder() == Qt::AscendingOrder) {

        fileSystemModel->sort(3, Qt::DescendingOrder);
        header->setSortIndicator(3, Qt::DescendingOrder);

    } else {

        fileSystemModel->sort(3, Qt::AscendingOrder);
        header->setSortIndicator(3, Qt::DescendingOrder);
    }
}

void MainWindow::renameItem() {

    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (!selectedRows.isEmpty() && selectedRows.size() == 1) {

        ui->tableView->edit(selectedRows.at(0));
    }
}

QString getUniqueDirName(const QString &existingDir) {

    QDir dir = QDir(existingDir);
    QString uniqueDir = QString(existingDir);
    int length = uniqueDir.size();

    for (int i = 1; dir.exists(); i++) {

        uniqueDir.truncate(length);
        uniqueDir.append(" (" + QString::number(i) + ")");
        dir.setPath(uniqueDir);
    }

    return uniqueDir;
}

void MainWindow::newDirCreated(const QModelIndex &parent, int first, int last) {

    if (!newDir.isEmpty()) {

        ui->tableView->selectionModel()->clearSelection();
        ui->tableView->selectionModel()->setCurrentIndex(fileSystemModel->index(newDir), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        ui->tableView->edit(fileSystemModel->index(newDir));
        newDir.clear();
    }
}

void MainWindow::createDir() {

    QString dirName = fileSystemModel->filePath(currentParent) + QDir::separator() + "New folder";
    QDir dir(dirName);

    if (dir.exists()) {
        dirName = getUniqueDirName(dirName);
    }

    newDir = dirName;
    dir.mkdir(dirName);
}

void copyDirectory(const QString &srcDir, const QString &dstDir) {

    QDir dir(srcDir);
    if (!dir.exists())
        return;

    QFileInfo fileInfo;
    foreach (const QString &entry, dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot)) {

        fileInfo.setFile(srcDir + QDir::separator() + entry);

        if (fileInfo.isDir()) {

            dir.mkdir(dstDir + QDir::separator() + fileInfo.fileName());
            copyDirectory(srcDir + QDir::separator() + fileInfo.fileName(), dstDir + QDir::separator() + fileInfo.fileName());

        } else {

            QFile file(fileInfo.absoluteFilePath());
            file.copy(dstDir + QDir::separator() + fileInfo.fileName());
        }
    }
}

void MainWindow::copySelectedItems() {

    QStringList selectedItems;
    foreach (const QModelIndex &index, ui->tableView->selectionModel()->selectedRows()) {

        selectedItems.append(fileSystemModel->filePath(index));
    }
    copiedItems = selectedItems;
}

void MainWindow::pasteFiles() {

    foreach (const QFileInfo &fileInfo, copiedItems) {

        if (fileInfo.isDir()) {

            QDir dir;
            dir.mkdir(fileSystemModel->filePath(currentParent) + QDir::separator() + fileInfo.fileName());
            copyDirectory(fileInfo.absoluteFilePath(), fileSystemModel->filePath(currentParent) + QDir::separator() + fileInfo.fileName());

        } else {

            QFile file(fileInfo.absoluteFilePath());
            file.copy(fileSystemModel->filePath(currentParent) + QDir::separator() + fileInfo.fileName());
        }
    }

}
