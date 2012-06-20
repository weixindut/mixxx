#ifndef LIBRARYTABLEMODEL_H
#define LIBRARYTABLEMODEL_H

#include <QtSql>
#include <QtCore>

#include "library/basesqltablemodel.h"
#include "library/trackmodel.h"
#include "library/trackcollection.h"
#include "library/dao/trackdao.h"

class TrackCollection;

class LibraryTableModel : public BaseSqlTableModel {
    Q_OBJECT
  public:
    LibraryTableModel(QObject* parent, TrackCollection* pTrackCollection,
					  ConfigObject<ConfigValue>* pConfig,
                      QString settingsNamespace="mixxx.db.model.library");
    virtual ~LibraryTableModel();
    
    void setLibrary();

    virtual TrackPointer getTrack(const QModelIndex& index) const;
    virtual void search(const QString& searchText);
    virtual bool isColumnInternal(int column);
    virtual bool isColumnHiddenByDefault(int column);
    virtual bool addTrack(const QModelIndex& index, QString location);
    // Takes a list of locations and add the tracks to the library. Returns the
    // number of successful additions.
    virtual int addTracks(const QModelIndex& index, QList<QString> locations);
    virtual void moveTrack(const QModelIndex& sourceIndex,
                           const QModelIndex& destIndex);
    TrackModel::CapabilitiesFlags getCapabilities() const;
    static const QString DEFAULT_LIBRARYFILTER;

  public slots:
    void slotConfigChanged(QString, QString);


  private slots:
    void slotSearch(const QString& searchText);

  signals:
    void doSearch(const QString& searchText);
    
  private:
    TrackDAO& m_trackDao;
    ConfigObject<ConfigValue>* m_pConfig;
    TrackCollection* m_pTrackCollection;

};

#endif
