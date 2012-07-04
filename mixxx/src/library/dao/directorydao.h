#ifndef DIRECTORYDAO_H
#define DIRECTORYDAO_H

#include <QSqlDatabase>
#include "library/dao/dao.h"

const QString DIRECTORYDAO_DIR = "directory";
const QString DIRECTORYDAO_ID  = "dir_id";
const QString DIRECTORYDAO_TABLE = "directories";

class DirectoryDAO : public DAO {
  public:
    // normal method
    DirectoryDAO(QSqlDatabase& database);
    // starts it's own database connection. This is needed to create a
    // tablemodel,... in the library preferences view
    DirectoryDAO();
    // disconnect from db!!!
    virtual ~DirectoryDAO();
    // void setDatabase(QSqlDatabase& database);

    void initialize();
    bool addDirectory(QString dir);
    bool purgeDirectory(QString dir);
    bool updateTrackLocations(QString dir);
    int getDirId(QString dir);
    QStringList getDirs();
    
  private:
    QSqlDatabase m_database;

};

#endif //DIRECTORYDAO_H
