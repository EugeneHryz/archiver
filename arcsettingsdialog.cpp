#include "arcsettingsdialog.h"
#include "ui_arcsettingsdialog.h"

ArcSettingsDialog::ArcSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArcSettingsDialog), selectedItems(QStringList()), archivedFilePath(QString())
{
    ui->setupUi(this);
    this->setWindowTitle("Add to Archive");
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

void ArcSettingsDialog::setArchivedFilePath(const QString &path) {

    archivedFilePath = path;
}

void ArcSettingsDialog::archiveText(const QString &sourceFileName, FILE *encodedFile, const QString &relativePath) {

    FILE *sourceFile = NULL;
    fopen_s(&sourceFile, sourceFileName.toLocal8Bit().constData(), "r");

    FILE *compressedFile = NULL;
    char *tempFileName = (char*)malloc(L_tmpnam_s * sizeof(char));
    tmpnam_s(tempFileName, L_tmpnam_s);
    fopen_s(&compressedFile, tempFileName, "w");

    progress->setLabelText("Compressing " + sourceFileName + "...");
    compress_file(sourceFile, compressedFile);

    fclose(sourceFile);
    fclose(compressedFile);

    fopen_s(&compressedFile, tempFileName, "rb");

    int fileLength = 0;
    int characters[CHARACTERS_NUMBER] = { 0 };

    Character *head = NULL;
    Node *root = NULL;

    char *codes[CHARACTERS_NUMBER] = { NULL };

    fileLength = get_characters_count(compressedFile, characters);
    if (fileLength > 0) {

        head = make_characters_list(characters);
        root = make_tree(&head);
        make_codes(root, 0, codes);

        int pos = get_info_block_pos(encodedFile);

        progress->setLabelText("Encoding " + sourceFileName + "...");
        encode_file(compressedFile, encodedFile, relativePath.toLocal8Bit().constData(), root, codes, fileLength, pos);
        progress->setValue(progress->value() + 1);
    }

    fclose(compressedFile);
    remove(tempFileName);
}

void ArcSettingsDialog::archiveBin(const QString &sourceFileName, FILE *encodedFile, const QString &relativePath) {

    FILE *sourceFile = NULL;
    fopen_s(&sourceFile, sourceFileName.toLocal8Bit().constData(), "rb");

    int fileLength = 0;
    int characters[CHARACTERS_NUMBER] = { 0 };

    Character *head = NULL;
    Node *root = NULL;

    char *codes[CHARACTERS_NUMBER] = { NULL };

    fileLength = get_characters_count(sourceFile, characters);
    if (fileLength > 0) {

        head = make_characters_list(characters);
        root = make_tree(&head);
        make_codes(root, 0, codes);

        int pos = get_info_block_pos(encodedFile);

        progress->setLabelText("Encoding " + sourceFileName + "...");
        encode_file(sourceFile, encodedFile, relativePath.toLocal8Bit().constData(), root, codes, fileLength, pos);
        progress->setValue(progress->value() + 1);

        free_tree(root);
    }

    fclose(sourceFile);
}

FILE* reopenFile(const QString &fileName) {

    FILE *file = NULL;

    fopen_s(&file, fileName.toLocal8Bit().constData(), "rb");

    FILE *tempFile = NULL;
    tmpfile_s(&tempFile);

    copy_file_contents(file, tempFile);

    fclose(file);
    fopen_s(&file, fileName.toLocal8Bit().constData(), "wb+");

    copy_file_contents(tempFile, file);

    fseek(file, 0, SEEK_END);
    fclose(tempFile);

    return file;
}

void ArcSettingsDialog::archiveFiles() {

    QString archiveName = ui->comboBox->currentText();
    QFileInfo fileInfo = QFileInfo(archiveName);

    FILE *encodedFile = NULL;

    if (fileInfo.exists()) {
        encodedFile = reopenFile(archiveName);
    } else {
        fopen_s(&encodedFile, archiveName.toLocal8Bit().constData(), "wb+");
    }
    if (encodedFile == NULL)
        return;

    QString relativePath = QString();

    int steps = 0;
    foreach (const QFileInfo &fileInfo, selectedItems) {

        if (!fileInfo.completeBaseName().endsWith(".") && !fileInfo.isDir()) {
            steps++;
        }
    }

    steps *= 2;
    progress = new QProgressDialog("Archiving...", "Cancel", 0, steps, this, Qt::WindowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    progress->setWindowTitle("Archiving...");
    progress->setWindowModality(Qt::WindowModal);
    progress->setCancelButton(nullptr);
    progress->setMinimumDuration(0);
    progress->setValue(0);
    progress->open();

    QString rootDirName = QString(selectedItems.at(0));
    rootDirName.truncate(rootDirName.lastIndexOf(QChar('/')));
    rootDirName.remove(0, rootDirName.lastIndexOf(QChar('/')) + 1);

    foreach (const QString &fileName, selectedItems) {

        fileInfo.setFile(fileName);

        QString parentDir = fileInfo.absolutePath();
        parentDir.remove(0, parentDir.lastIndexOf(QChar('/')) + 1);

        if (!fileName.endsWith(".")) {

            if (parentDir.compare(rootDirName) == 0) {

                relativePath.clear();
            }

            if (!relativePath.isEmpty()) {

                printf("\nParent folder name: %s\nCurrent folder name: %s\n", parentDir.toLocal8Bit().constData(), relativePath.mid(relativePath.lastIndexOf(QChar('/')) + 1).toLocal8Bit().constData());

                while (parentDir.compare(relativePath.mid(relativePath.lastIndexOf(QChar('/')) + 1)) != 0) {

                    relativePath.truncate(relativePath.lastIndexOf(QChar('/')));
                }
            }

            relativePath += "/" + fileInfo.fileName();

            if (fileInfo.isFile()) {

                progress->setLabelText("Archiving " + fileName + "...");

                if (fileInfo.completeSuffix().endsWith("txt")) {

                    archiveText(fileName, encodedFile, relativePath);

                } else {

                    archiveBin(fileName, encodedFile, relativePath);
                }

                progress->setValue(progress->value() + 1);
                relativePath.truncate(relativePath.lastIndexOf(QChar('/')));
            }
        }
    }
    progress->setValue(steps);

    delete(progress);
    fclose(encodedFile);
}

void ArcSettingsDialog::on_add_clicked()
{
    this->hide();

    archiveFiles();
}

void ArcSettingsDialog::on_cancel_clicked()
{
    this->hide();
}

void ArcSettingsDialog::on_toolButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Archive", ui->comboBox->currentText().chopped(4),
                                                    "Archives (*.lzh)", nullptr, QFileDialog::DontConfirmOverwrite);
    QFileInfo fileInfo = QFileInfo(fileName);

    ui->comboBox->addItem(fileName);
    ui->comboBox->setCurrentText(fileName);
}

bool isArchive(const QString &fileName) {

    bool result = false;

    FILE *file = NULL;
    fopen_s(&file, fileName.toLocal8Bit().constData(), "rb");

    if (file != NULL) {

        result = is_archive(file);
        fclose(file);
    }
    return result;
}

