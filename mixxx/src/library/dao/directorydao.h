#ifndef DIRECTORYDAO_H
#define DIRECTORYDAO_H

#include <QObject>
#include <QSqlDatabase>
#include "library/dao/dao.h"

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
    
  private:
    QSqlDatabase m_database;

};

#endif //DIRECTORYDAO_H
