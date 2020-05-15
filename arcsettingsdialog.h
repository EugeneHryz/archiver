#pragma once

#include <QDialog>
#include <QComboBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QProgressDialog>

#include "compressing.h"
#include "coding.h"
#include "decoding.h"

#include "archiveview.h"

namespace Ui {
class ArcSettingsDialog;
}

class ArcSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ArcSettingsDialog(QWidget *parent = nullptr);
    ~ArcSettingsDialog();

    QComboBox* getComboBox() const;

    void setSelectedItems(const QStringList &list);
    void setArchivedFilePath(const QString &path);

    void archiveText(const QString &sourceFileName, FILE *encodedFile, const QString &relativePath);
    void archiveBin(const QString &sourceFileName, FILE *encodedFile, const QString &relativePath);
    void archiveFiles();

private slots:
    void on_add_clicked();
    void on_cancel_clicked();

    void on_toolButton_clicked();

private:
    Ui::ArcSettingsDialog *ui;

    QProgressDialog *progress;

    QStringList selectedItems;
    QString archivedFilePath;
};

bool isArchive(const QString &fileName);

