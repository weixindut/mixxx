#include <QtSql>
#include <QtDebug>
#include <QStringBuilder>

#include "directorydao.h"
#include "library/queryutil.h"

DirectoryDAO::DirectoryDAO(QSqlDatabase& database)
        : m_database(database) {

}

DirectoryDAO::~DirectoryDAO(){
}

void DirectoryDAO::initialize()
{
    qDebug() << "DirectoryDAO::initialize" << QThread::currentThread() 
             << m_database.connectionName();
}

bool DirectoryDAO::addDirectory(QString dir){
    ScopedTransaction transaction(m_database);
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO directories (directory) "
                  "VALUES (\""% dir %"\")");

    if (!query.exec()) {
        qDebug() << "Adding new dir ("% dir %") failed:"
                 <<query.lastError();
        return false;
    }
    transaction.commit();
    return true;
}

bool DirectoryDAO::purgeDirectory(QString dir){
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM directories WHERE directory=\"" % dir % "\"");

    if (!query.exec()) {
        qDebug() << "purging dir ("%dir%") failed:"<<query.lastError();
        return false;
    }
    return true;
}

bool DirectoryDAO::relocateDirectory(QString oldFolder, QString newFolder){
    ScopedTransaction transaction(m_database);
    QSqlQuery query(m_database);
    // update directory in directories table
    query.prepare("UPDATE "%DIRECTORYDAO_TABLE%" SET "%DIRECTORYDAO_DIR%"="
                  "REPLACE("%DIRECTORYDAO_DIR%",\""%oldFolder%"\",\""%newFolder%"\")");
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << "coud not relocate directory";
        return false;
    }
    // update location and directory in track_locations table
    query.prepare("UPDATE track_locations SET location="
                  "REPLACE(location,\""%oldFolder%"\",\""%newFolder%"\")");
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << "coud not relocate path of tracks";
        return false;
    }
    query.prepare("UPDATE track_locations SET directory="
                  "REPLACE(directory,\""%oldFolder%"\",\""%newFolder%"\")");
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << "coud not relocate path of tracks";
        return false;
    }
    // updating the dir_id column is not necessary because it does not change

    // set all tracks to need verification
    query.prepare("UPDATE track_locations SET needs_verification=1");
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << "coud not relocate path of tracks";
        return false;
    }
    transaction.commit();
    return true;
}

QStringList DirectoryDAO::getDirs(){
    QSqlQuery query(m_database);
    query.prepare("SELECT " % DIRECTORYDAO_DIR % " FROM " % DIRECTORYDAO_TABLE);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << "There are no directories saved in the db";
    }
    QStringList dirs;
    while (query.next()) {
        dirs << query.value(query.record().indexOf(DIRECTORYDAO_DIR)).toString();
    }
    return dirs;
}

int DirectoryDAO::getDirId(QString dir){
    QSqlQuery query(m_database);
    query.prepare("SELECT " % DIRECTORYDAO_ID % " FROM " % DIRECTORYDAO_TABLE %
                  " WHERE " % DIRECTORYDAO_DIR %"=\"" % dir % "\"");
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << "couldn't find directory:"<<dir;
    }
    qDebug() << query.size() << "number of dirs with that name";
    int id =0;
    while (query.next()) {
        id = query.value(query.record().indexOf(DIRECTORYDAO_ID)).toInt();
    }
    return id;
}

bool DirectoryDAO::updateTrackLocations(QString dir){
    QString dirId = QString::number(getDirId(dir));
    ScopedTransaction transaction(m_database);
    QSqlQuery query(m_database);
    query.prepare("UPDATE track_locations SET "%DIRECTORYDAO_ID%" = "%dirId);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query) << " could not update TrackLocations";
        return false;
    }
    transaction.commit();
    return true;
}
