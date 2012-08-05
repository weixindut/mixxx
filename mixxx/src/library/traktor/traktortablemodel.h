#ifndef TRAKTOR_TABLE_MODEL_H
#define TRAKTOR_TABLE_MODEL_H

#include "library/basesqltablemodel.h"

class TrackCollection;

class TraktorTableModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    TraktorTableModel(QObject* parent, TrackCollection* pTrackCollection);
    TrackModel::CapabilitiesFlags getCapabilities() const;
    virtual ~TraktorTableModel();

    TrackPointer getTrack(const QModelIndex& index) const;
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const;

};

#endif /* TRAKTOR_TABLE_MODEL_H */
