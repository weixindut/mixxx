#include <QApplication>

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
                                     QList<int> availableDirIds,
                                     QString settingsNamespace)
        : BaseSqlTableModel(parent, pTrackCollection, pConfig,
                            availableDirIds, settingsNamespace){
    connect(parent, SIGNAL(availableDirsChanged(QList<int>,QString)),
            this, SLOT(slotAvailableDirsChanged(QList<int>,QString)));
    setTableModel(0,QString());
}

LibraryTableModel::~LibraryTableModel() {
}

void LibraryTableModel::setTableModel(int id, QString name){
    Q_UNUSED(id);
    QStringList columns;
    columns << "library."+LIBRARYTABLE_ID;

    //prepareLibrary give a NULL to the constructor so check for it
    bool showMissing;
    if (m_pConfig) {
        showMissing = m_pConfig->getValueString(ConfigKey("[Library]","ShowMissingSongs"),"1").toInt();
    } else {
        showMissing = false;
    }
    QString tableName = "library_view";
    QString libraryFilter;
    if (showMissing) {
        libraryFilter = "mixxx_deleted=0";
        tableName.append("_missing");
        qDebug() << "hei show also missing songs";
    } else {
        libraryFilter = "mixxx_deleted=0 AND fs_deleted=0";
    }
    tableName.append("_"+name);
    qDebug() <<"kain88 tablename="<<tableName;

    qDebug() << m_availableDirIds;
    QStringList ids;
    foreach (int id, m_availableDirIds) {
        ids << QString::number(id);
    }
    qDebug() << ids;

    QSqlQuery query(m_pTrackCollection->getDatabase());
    QString queryString = "CREATE TEMPORARY VIEW IF NOT EXISTS "+tableName+" AS "
    // QString queryString = "CREATE TEMPORARY "+tableName+" AS "
            "SELECT " + columns.join(", ") +
            " FROM library INNER JOIN track_locations "
            "ON library.location = track_locations.id "
            "WHERE (" + libraryFilter + ") AND track_locations.maindir_id in ("+ids.join(",")+",0)" ;
    query.prepare(queryString);
    qDebug() << "kain88 LTM query string";
    qDebug() << queryString;
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


int LibraryTableModel::addTracks(const QModelIndex& index, QList<QString> locations) {
    Q_UNUSED(index);
    QList<QFileInfo> fileInfoList;
    foreach (QString fileLocation, locations) {
        fileInfoList.append(QFileInfo(fileLocation));
    }
    QList<int> trackIds = m_trackDAO.addTracks(fileInfoList, true);
    select();
    return trackIds.size();
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

void LibraryTableModel::slotLoadTrackFailed(TrackPointer pTrack){
    m_trackDAO.markTrackAsDeleted(pTrack);
}
