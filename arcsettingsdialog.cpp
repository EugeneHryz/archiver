#include "arcsettingsdialog.h"
#include "ui_arcsettingsdialog.h"

ArcSettingsDialog::ArcSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArcSettingsDialog)
{
    ui->setupUi(this);
}

ArcSettingsDialog::~ArcSettingsDialog()
{
    delete ui;
}

QComboBox* ArcSettingsDialog::getComboBox() const {

    return ui->comboBox;
}

void ArcSettingsDialog::setSelectedItems(const QStringList &list) {

    selectedItems = list;
}

void ArcSettingsDialog::on_add_clicked()
{
    foreach (const QString &fileName, selectedItems) {

        FILE *sourceFile = NULL;
        fopen_s(&sourceFile, fileName.toLocal8Bit().constData(), "r");

        FILE *compressedFile = NULL;
        fopen_s(&compressedFile, (fileName.chopped(4) + ".lzh").toLocal8Bit().constData(), "w");

        compress_file(sourceFile, compressedFile);

        fclose(sourceFile);
        fclose(compressedFile);
    }

    this->hide();
}

void ArcSettingsDialog::decompress() {

    if (!selectedItems.isEmpty()) {

        foreach (const QString &fileName, selectedItems) {

            FILE *compressedFile = NULL;
            fopen_s(&compressedFile, fileName.toLocal8Bit().constData(), "r");

            FILE *outFile = NULL;
            fopen_s(&outFile, (fileName.chopped(4) + ".txt").toLocal8Bit().constData(), "w");

            decompress_file(compressedFile, outFile);

            fclose(compressedFile);
            fclose(outFile);
        }
    }
}

void ArcSettingsDialog::on_cancel_clicked()
{
    this->hide();
}
