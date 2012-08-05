#ifndef HIDDENTABLEMODEL_H
#define HIDDENTABLEMODEL_H

#include "library/basesqltablemodel.h"

class HiddenTableModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    HiddenTableModel(QObject* parent, TrackCollection* pTrackCollection,
                     QStringList availableDirs);
    virtual ~HiddenTableModel();
    void setTableModel(int,QString);
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    void purgeTracks(const QModelIndexList& indices);
    //TODO (kain88) check why this function is here
    void purgeTracks(const int dirId);
    void unhideTracks(const QModelIndexList& indices);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    TrackModel::CapabilitiesFlags getCapabilities() const;
};

#endif
