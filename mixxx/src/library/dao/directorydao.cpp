#include <QtSql>
#include <QString>
#include <QtDebug>
#include <QVariant>
#include <QThread>

#include "directorydao.h"
#include "library/queryutil.h"

DirectoryDAO::DirectoryDAO(QSqlDatabase& database)
        : m_database(database) {

}

DirectoryDAO::DirectoryDAO()
            : m_database(QSqlDatabase::addDatabase("QSQLITE")) {

    const QString MIXXX_DB_PATH = QDir::homePath().append("/").append(
                                SETTINGS_PATH).append("mixxxdb.sqlite");
    m_database.setHostName("localhost");
    m_database.setDatabaseName(MIXXX_DB_PATH);
    m_database.setUserName("mixxx");
    m_database.setPassword("mixxx");
    bool ok = m_database.open();
    qDebug() << __FILE__ << "DB status:" << ok;
    if (m_database.lastError().isValid()) {
        qDebug() << "Error loading database:" << m_database.lastError();
    }
}

DirectoryDAO::~DirectoryDAO(){

}

void DirectoryDAO::initialize()
{
    qDebug() << "DirectoryDAO::initialize" << QThread::currentThread() 
             << m_database.connectionName();
}

bool DirectoryDAO::addDirectory(QString dir){
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO directories (directory) "
                  "VALUES ("+ dir +")");
    
    if (!query.exec()) {
        qDebug() << "Adding new dir ("+ dir +") failed:"
                 <<query.lastError();
        return false;
    }
    return true;
}

bool DirectoryDAO::purgeDirectory(QString dir){
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM directories WHERE directory="+dir);

    if (!query.exec()) {
        qDebug() << "purging dir ("+dir+") failed:"<<query.lastError();
        return false;
    }
    return true;
}
