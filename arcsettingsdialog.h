#pragma once

#include <QDialog>
#include <QComboBox>

#include "archive.h"

namespace Ui {
class ArcSettingsDialog;
}

class ArcSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArcSettingsDialog(QWidget *parent = nullptr);
    ~ArcSettingsDialog();

    QComboBox* getComboBox() const;

    void setSelectedItems(const QStringList &list);

private slots:
    void on_add_clicked();
    void on_cancel_clicked();

private:
    Ui::ArcSettingsDialog *ui;

    QStringList selectedItems;
};

