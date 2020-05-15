#include "stringlistmodel.h"

int StringListModel::rowCount(const QModelIndex &parent) const {

    return archivedFileNames.count();
}

QVariant StringListModel::data(const QModelIndex &index, int role) const {

    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() >= archivedFileNames.size()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        return archivedFileNames.at(index.row());
    }
    else
        return QVariant();
}

QVariant StringListModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return QVariant(archivePath);
    } else
        return QVariant();
}

void StringListModel::setAcrhivedFileNames(const QStringList &strings) {

    archivedFileNames = strings;
}

void StringListModel::setArchivePath(const QString &path) {

    archivePath = path;
}

QString StringListModel::getArchivePath() const {

    return archivePath;
}

void StringListModel::removeFileName(const QString &string) {

    archivedFileNames.removeOne(string);
}
