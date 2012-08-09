#ifndef ITUNES_TABLE_MODEL_H
#define ITUNES_TABLE_MODEL_H

#include "library/basesqltablemodel.h"

class ITunesTrackModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    ITunesTrackModel(QObject* parent, TrackCollection* pTrackCollection);
    TrackModel::CapabilitiesFlags getCapabilities() const;
    virtual ~ITunesTrackModel();

    TrackPointer getTrack(const QModelIndex& index) const;
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const;

};

#endif /* ITUNES_TABLE_MODEL_H */
