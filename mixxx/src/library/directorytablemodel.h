#ifndef DIRECTORYTABLEMODEL_H
#define DIRECTORYTABLEMODEL_H

#include <QtSql>
#include <QtCore>

#include "library/dao/directorydao.h"

// class DirectoryDAO;

class DirectoryTableModel : public QSqlTableModel {
    Q_OBJECT
  public:
    DirectoryTableModel(QObject* parent);
    virtual ~DirectoryTableModel();
    void setDirectory();
    void purgeDirectory(QString);
    bool addDirectory(QString);

  private:
    DirectoryDAO m_directoryDAO;
};

#endif
