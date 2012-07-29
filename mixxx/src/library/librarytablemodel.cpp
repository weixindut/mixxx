#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QApplication>

#include "library/trackcollection.h"
#include "library/librarytablemodel.h"
#include "library/queryutil.h"
#include "controlobjectthread.h"
#include "controlobject.h"


#include "mixxxutils.cpp"

const QString LibraryTableModel::DEFAULT_LIBRARYFILTER =
        "mixxx_deleted=0 AND fs_deleted=0"; 

LibraryTableModel::LibraryTableModel(QObject* parent,
                                     TrackCollection* pTrackCollection,
                                     ConfigObject<ConfigValue>* pConfig,
                                     QStringList availableDirs,
                                     QString settingsNamespace)
        : BaseSqlTableModel(parent, pTrackCollection,
                            pTrackCollection->getDatabase(),
                            settingsNamespace),
          m_trackDao(pTrackCollection->getTrackDAO()),
          m_availableDirs(availableDirs) {
    m_pConfig=pConfig;
    m_pTrackCollection = pTrackCollection;
    connect(this, SIGNAL(doSearch(const QString&)),
            this, SLOT(slotSearch(const QString&)));
    connect(parent, SIGNAL(availableDirsChanged(QStringList,QString)),
            this, SLOT(slotAvailableDirsChanged(QStringList,QString)));
    setLibrary(QString());
}

LibraryTableModel::~LibraryTableModel() {
}

void LibraryTableModel::setLibrary(QString name){
    QStringList columns;
    columns << "library."+LIBRARYTABLE_ID;

    //prepareLibrary give a NULL to the constructor so check for it
    bool showMissing;
    if (m_pConfig) {
        showMissing = m_pConfig->getValueString(ConfigKey("[Library]","ShowMissingSongs")).toInt();
    } else {
        showMissing = false;
    }
    QString tableName = "library_view";
    QString libraryFilter;
    if (showMissing) {
        libraryFilter = "mixxx_deleted=0";
        tableName.append("_missing");
    } else {
        libraryFilter = "mixxx_deleted=0 AND fs_deleted=0";
    }
    tableName.append("_"+name);

    qDebug() << "kani88kain88kain88kin88kain88aksjdaksjdakdakdj";
    qDebug() << m_availableDirs;
    QSqlQuery query(m_pTrackCollection->getDatabase());
    QString queryString = "CREATE TEMPORARY VIEW IF NOT EXISTS "+tableName+" AS "
    // QString queryString = "CREATE TEMPORARY "+tableName+" AS "
            "SELECT " + columns.join(", ") +
            " FROM library INNER JOIN track_locations "
            "ON library.location = track_locations.id "
            "WHERE (" + libraryFilter + ") AND track_locations.dir in (\""+m_availableDirs.join("\",\"")+"\")" ;
    query.prepare(queryString);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
    }

    QStringList tableColumns;
    tableColumns << LIBRARYTABLE_ID;
    setTable(tableName, LIBRARYTABLE_ID, tableColumns,
             m_pTrackCollection->getTrackSource("default"));

    // BaseSqlTabelModel will setup the header info
    initHeaderData();

    setSearch("");
    setDefaultSort(fieldIndex("artist"), Qt::AscendingOrder);
}

bool LibraryTableModel::addTrack(const QModelIndex& index, QString location) {
    Q_UNUSED(index);
    QFileInfo fileInfo(location);

    // Adds track, does not insert duplicates, handles unremoving logic.
    int trackId = m_trackDao.addTrack(fileInfo, true);
    if (trackId >= 0) {
        // TODO(rryan) do not select since we will get a signal. instead, do
        // something nice UI wise and select the track they dropped.
        select(); //Repopulate the data model.
        return true;
    }
    return false;
}

int LibraryTableModel::addTracks(const QModelIndex& index, QList<QString> locations) {
    Q_UNUSED(index);
    QList<QFileInfo> fileInfoList;
    foreach (QString fileLocation, locations) {
        fileInfoList.append(QFileInfo(fileLocation));
    }
    QList<int> trackIds = m_trackDao.addTracks(fileInfoList, true);
    return trackIds.size();
}

TrackPointer LibraryTableModel::getTrack(const QModelIndex& index) const {
    int trackId = getTrackId(index);
    return m_trackDao.getTrack(trackId);
}

void LibraryTableModel::moveTrack(const QModelIndex& sourceIndex,
                                  const QModelIndex& destIndex) {
    Q_UNUSED(sourceIndex);
    Q_UNUSED(destIndex);
    // Does nothing because we don't support reordering tracks in the library,
    // and getCapabilities() reports that.
}

void LibraryTableModel::search(const QString& searchText) {
    // qDebug() << "LibraryTableModel::search()" << searchText
    //          << QThread::currentThread();
    emit(doSearch(searchText));
}

void LibraryTableModel::slotSearch(const QString& searchText) {
    // qDebug() << "slotSearch()" << searchText << QThread::currentThread();
    BaseSqlTableModel::search(searchText);
}

void LibraryTableModel::slotAvailableDirsChanged(QStringList availableDirs,
                                            QString name){
    m_availableDirs = availableDirs;
    setLibrary(name);
    select();
}


bool LibraryTableModel::isColumnInternal(int column) {
    if ((column == fieldIndex(LIBRARYTABLE_ID)) ||
        (column == fieldIndex(LIBRARYTABLE_URL)) ||
        (column == fieldIndex(LIBRARYTABLE_CUEPOINT)) ||
        (column == fieldIndex(LIBRARYTABLE_REPLAYGAIN)) ||
        (column == fieldIndex(LIBRARYTABLE_WAVESUMMARYHEX)) ||
        (column == fieldIndex(LIBRARYTABLE_SAMPLERATE)) ||
        (column == fieldIndex(LIBRARYTABLE_MIXXXDELETED)) ||
        (column == fieldIndex(LIBRARYTABLE_HEADERPARSED)) ||
        (column == fieldIndex(LIBRARYTABLE_PLAYED)) ||
        (column == fieldIndex(LIBRARYTABLE_BPM_LOCK)) ||
        (column == fieldIndex(LIBRARYTABLE_CHANNELS)) ||
        (column == fieldIndex(TRACKLOCATIONSTABLE_FSDELETED))) {
        return true;
    }
    return false;
}

bool LibraryTableModel::isColumnHiddenByDefault(int column) {
    if (column == fieldIndex(LIBRARYTABLE_KEY))
        return true;
    return false;
}



TrackModel::CapabilitiesFlags LibraryTableModel::getCapabilities() const {
    return TRACKMODELCAPS_NONE
            | TRACKMODELCAPS_RECEIVEDROPS
            | TRACKMODELCAPS_ADDTOPLAYLIST
            | TRACKMODELCAPS_ADDTOCRATE
            | TRACKMODELCAPS_ADDTOAUTODJ
            | TRACKMODELCAPS_RELOADMETADATA
            | TRACKMODELCAPS_LOADTODECK
            | TRACKMODELCAPS_LOADTOSAMPLER
            | TRACKMODELCAPS_HIDE
            | TRACKMODELCAPS_BPMLOCK
            | TRACKMODELCAPS_CLEAR_BEATS
            | TRACKMODELCAPS_RESETPLAYED
            | TRACKMODELCAPS_RELOCATE;
}

void LibraryTableModel::slotConfigChanged(QString identifier, QString key){
    Q_UNUSED(identifier);
    if (key=="ShowMissingSongs"){
        setLibrary(QString());
        select();
    }
}

void LibraryTableModel::relocateTracks(const QModelIndexList& indices) {
    foreach (QModelIndex index, indices) {
        int trackId = getTrackId(index);

        QString oldLocation = m_trackDao.getTrackLocation(trackId);
        TrackPointer track = m_trackDao.getTrack(trackId);
        qDebug() << oldLocation;
        QString newLocation = QFileDialog::getOpenFileName(QApplication::desktop(),
                                        QString(tr("Relocate track")),
                                        oldLocation);
        qDebug() << newLocation;

        if (!m_trackDao.relocateTrack(oldLocation, newLocation)) {
            QMessageBox::warning(QApplication::desktop(), tr("Mixxx"),
                                           tr("The tracks do not match"));
        }

    }
}

void LibraryTableModel::slotLoadTrackFailed(TrackPointer pTrack){
    m_trackDao.markTrackAsDeleted(pTrack);
}
