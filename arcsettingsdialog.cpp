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

        //printf("%s\n%s\n", fileName.toLocal8Bit().constData(), ui->comboBox->itemText(0).toLocal8Bit().constData());

        FILE *sourceFile = NULL;
        fopen_s(&sourceFile, fileName.toLocal8Bit().constData(), "r");

        FILE *arcFile = NULL;
        fopen_s(&arcFile, (fileName.chopped(4) + ".lzh").toLocal8Bit().constData(), "w");

        archive_file(sourceFile, arcFile);

        fclose(sourceFile);
        fclose(arcFile);
    }

    this->hide();
}

void ArcSettingsDialog::on_cancel_clicked()
{
    this->hide();
}
