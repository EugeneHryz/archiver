#pragma once

#include <QAbstractItemModel>

class StringListModel : public QAbstractListModel {
    Q_OBJECT

public:
    StringListModel(const QStringList &strings, QObject *parent = nullptr) :
        QAbstractListModel(parent), archivedFileNames(strings), archivePath(QString()) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setAcrhivedFileNames(const QStringList &strings);
    void setArchivePath(const QString &string);

    QString getArchivePath() const;

    void removeFileName(const QString &string);

private:
    QStringList archivedFileNames;
    QString archivePath;
};
