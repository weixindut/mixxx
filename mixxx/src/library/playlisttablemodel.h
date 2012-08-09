#ifndef PLAYLISTTABLEMODEL_H
#define PLAYLISTTABLEMODEL_H

#include "library/basesqltablemodel.h"
#include "library/dao/playlistdao.h"

class PlaylistTableModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    PlaylistTableModel(QObject* parent, TrackCollection* pTrackCollection,
                    QString settingsNamespace, 
                    ConfigObject<ConfigValue>* pConfig,
                    QList<int> availableDirIds,
                    bool showAll=false);
    virtual ~PlaylistTableModel();
    void setTableModel(int playlistId = -1);

    int getPlaylist() const {
        return m_iPlaylistId;
    }

    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    void removeTrack(const QModelIndex& index);
    void removeTracks(const QModelIndexList& indices);
    // Adding multiple tracks at one to a playlist. Returns the number of
    // successful additions.
    int addTracks(const QModelIndex& index, QList<QString> locations);
    void moveTrack(const QModelIndex& sourceIndex,
                           const QModelIndex& destIndex);
    void shuffleTracks(const QModelIndex& shuffleStartIndex);
    TrackModel::CapabilitiesFlags getCapabilities() const;

  private:
    PlaylistDAO& m_playlistDao;
    int m_iPlaylistId;
    bool m_showAll;
};

#endif
