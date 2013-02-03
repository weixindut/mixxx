#ifndef TRAKTOR_PLAYLIST_MODEL_H
#define TRAKTOR_PLAYLIST_MODEL_H

#include "library/basesqltablemodel.h"

class TraktorPlaylistModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    TraktorPlaylistModel(QObject* parent, TrackCollection* pTrackCollection);
    TrackModel::CapabilitiesFlags getCapabilities() const;
    virtual ~TraktorPlaylistModel();

    TrackPointer getTrack(const QModelIndex& index) const;
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setPlaylist(QString path_name);

};

#endif /* TRAKTOR_TABLE_MODEL_H */
