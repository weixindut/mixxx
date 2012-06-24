#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "library/trackcollection.h"
#include "library/directorytablemodel.h"
#include "library/dao/directorydao.h"
#include "mixxxutils.cpp"


DirectoryTableModel::DirectoryTableModel(QObject* parent)
                   : m_directoryDAO() {
    setDirectory();
}

DirectoryTableModel::~DirectoryTableModel() {
}

void DirectoryTableModel::setDirectory(){
/*
    QSqlQuery query;
    QString tableName("directory");

    QStringList columns;
    columns << "directory.directory";


    query.prepare("CREATE TEMPORARY VIEW IF NOT EXISTS " + tableName + " AS "
                  "SELECT "
                  + columns.join(",") +
                  " FROM directory ");
    if (!query.exec()) {
        qDebug() << query.executedQuery() << query.lastError();
    }

    //Print out any SQL error, if there was one.
    if (query.lastError().isValid()) {
     	qDebug() << __FILE__ << __LINE__ << query.lastError();
    }

    QStringList tableColumns;
    tableColumns << "directory";
    setTable(tableName, "directory" , tableColumns);

    // initHeaderData();
    setDefaultSort(fieldIndex("directory"), Qt::AscendingOrder);
    setSearch("");
*/

}

bool DirectoryTableModel::addDirectory(QString dir) {
    return m_directoryDAO.addDirectory(dir);
}

void DirectoryTableModel::purgeDirectory(QString dir) {
    m_directoryDAO.purgeDirectory(dir);
}
