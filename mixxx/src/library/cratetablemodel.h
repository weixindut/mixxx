// cratetablemodel.h
// Created 10/25/2009 by RJ Ryan (rryan@mit.edu)

#ifndef CRATETABLEMODEL_H
#define CRATETABLEMODEL_H

#include "library/basesqltablemodel.h"
#include "library/dao/cratedao.h"

class CrateTableModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    CrateTableModel(QObject* parent, TrackCollection* pTrackCollection,
                    ConfigObject<ConfigValue>* pConfig,
                    QStringList availableDirs);
    virtual ~CrateTableModel();

    void setTableModel(int crateId,QString name);
    int getCrate() const {
        return m_iCrateId;
    }

    // From TrackModel
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    void removeTracks(const QModelIndexList& indices);
    // Returns the number of unsuccessful track additions
    int addTracks(const QModelIndex& index, QList <QString> locations);
    TrackModel::CapabilitiesFlags getCapabilities() const;

  private:
    int m_iCrateId;
    CrateDAO& m_crateDAO;
};

#endif /* CRATETABLEMODEL_H */
