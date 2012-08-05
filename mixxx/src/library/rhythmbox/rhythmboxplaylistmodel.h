/***************************************************************************
                           rhythmboxplaylistmodel.h
                              -------------------
     begin                : 01/09/2011
     copyright            : (C) 2011 Tobias Rafreider

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RHYTHMBOXPLAYLISTMODEL_H
#define RHYTHMBOXPLAYLISTMODEL_H

#include "library/basesqltablemodel.h"

class RhythmboxPlaylistModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    RhythmboxPlaylistModel(QObject* parent, TrackCollection* pTrackCollection);
    virtual ~RhythmboxPlaylistModel();

    TrackPointer getTrack(const QModelIndex& index) const;
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setPlaylist(QString path_name);
    TrackModel::CapabilitiesFlags getCapabilities() const;
};

#endif
