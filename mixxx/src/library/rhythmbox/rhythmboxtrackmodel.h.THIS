/***************************************************************************
                           rhythmboxtracksource.h
                              -------------------
     begin                : 8/15/2009
     copyright            : (C) 2011 Tobias Rafreider
     email                : alberts@mixxx.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RHYTHMBOXTRACKMODEL_H
#define RHYTHMBOXTRACKMODEL_H


#include "library/basesqltablemodel.h"

class RhythmboxTrackModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    RhythmboxTrackModel(QObject* parent, TrackCollection* pTrackCollection);
    TrackModel::CapabilitiesFlags getCapabilities() const;
    virtual ~RhythmboxTrackModel();

    TrackPointer getTrack(const QModelIndex& index) const;
    bool isColumnInternal(int column);
    bool isColumnHiddenByDefault(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const;

};

#endif
