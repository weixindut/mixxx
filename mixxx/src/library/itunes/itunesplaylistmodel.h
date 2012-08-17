#ifndef ITUNES_PLAYLIST_MODEL_H
#define ITUNES_PLAYLIST_MODEL_H

#include "library/basesqltablemodel.h"

class ITunesPlaylistModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    ITunesPlaylistModel(QObject* parent, TrackCollection* pTrackCollection);
    virtual ~ITunesPlaylistModel();

    TrackPointer getTrack(const QModelIndex& index) const;
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setPlaylist(QString path_name);
    TrackModel::CapabilitiesFlags getCapabilities() const;

};

#endif /* ITUNES_PLAYLIST_MODEL_H */
