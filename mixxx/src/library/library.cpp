// library.cpp
// Created 8/23/2009 by RJ Ryan (rryan@mit.edu)

#include <QItemSelectionModel>

#include "library/library.h"
#include "library/libraryfeature.h"
#include "library/librarytablemodel.h"
#include "library/sidebarmodel.h"
#include "library/trackcollection.h"
#include "library/trackmodel.h"
#include "library/browse/browsefeature.h"
#include "library/cratefeature.h"
#include "library/rhythmbox/rhythmboxfeature.h"
#include "library/recording/recordingfeature.h"
#include "library/itunes/itunesfeature.h"
#include "library/mixxxlibraryfeature.h"
#include "library/autodjfeature.h"
#include "library/playlistfeature.h"
#include "library/preparefeature.h"
#include "library/promotracksfeature.h"
#include "library/traktor/traktorfeature.h"
#include "library/librarycontrol.h"
#include "library/setlogfeature.h"

#include "widget/wtracktableview.h"
#include "widget/wlibrary.h"
#include "widget/wlibrarysidebar.h"

#include "mixxxkeyboard.h"

// This is is the name which we use to register the WTrackTableView with the
// WLibrary
const QString Library::m_sTrackViewName = QString("WTrackTableView");

Library::Library(QObject* parent, ConfigObject<ConfigValue>* pConfig, bool firstRun,
                 RecordingManager* pRecordingManager)
    : m_pConfig(pConfig),
      m_pSidebarModel(new SidebarModel(parent)),
      m_pTrackCollection(new TrackCollection(pConfig)),
      m_pLibraryControl(new LibraryControl(this)),
      m_pRecordingManager(pRecordingManager),
      m_ptrackModel(NULL),
      m_directoryDAO(m_pTrackCollection->getDirectoryDAO()),
      m_mountwatcher(parent){
    // check if all directories are accessable 
    QStringList dirs = m_directoryDAO.getDirs();
    qDebug() << "kain88";
    qDebug() << "List of dirs to check" <<dirs;
    foreach( QString dir, dirs) {
        if (QDir(dir).exists()) {
            m_availableDirs << dir;
            qDebug() << "dir found:"<<dir;
        } else {
            m_unavailableDirs << dir;
            qDebug() << "dir not found" << dir;
        }
    }

    QList<int> availableDirIds = m_directoryDAO.getDirIds(m_availableDirs);

    connect(&m_mountwatcher, SIGNAL(foundNewStorage(QStringList)),
            this, SLOT(slotFoundNewStorage(QStringList)));
    connect(&m_mountwatcher, SIGNAL(removedStorage(QStringList)),
            this, SLOT(slotRemovedStorage(QStringList)));

    // TODO(rryan) -- turn this construction / adding of features into a static
    // method or something -- CreateDefaultLibrary
    m_pMixxxLibraryFeature = new MixxxLibraryFeature(this, m_pTrackCollection,pConfig,
                            availableDirIds);
    addFeature(m_pMixxxLibraryFeature,true);

    if (PromoTracksFeature::isSupported(m_pConfig)) {
        m_pPromoTracksFeature = new PromoTracksFeature(this, pConfig,
                                                       m_pTrackCollection,
                                                       firstRun);
        addFeature(m_pPromoTracksFeature);
    } else {
        m_pPromoTracksFeature = NULL;
    }

    addFeature(new AutoDJFeature(this, pConfig, m_pTrackCollection,availableDirIds),true);
    m_pPlaylistFeature = new PlaylistFeature(this, m_pTrackCollection, pConfig, availableDirIds);
    addFeature(m_pPlaylistFeature,true);
    m_pCrateFeature = new CrateFeature(this, m_pTrackCollection, pConfig, availableDirIds);
    addFeature(m_pCrateFeature,true);
    addFeature(new BrowseFeature(this, pConfig, m_pTrackCollection, m_pRecordingManager));
    addFeature(new RecordingFeature(this, pConfig, m_pTrackCollection, m_pRecordingManager));
    // the history should show ALL songs 
    addFeature(new SetlogFeature(this, pConfig, m_pTrackCollection,
                                    m_directoryDAO.getDirIds(dirs)));
    m_pPrepareFeature = new PrepareFeature(this, pConfig, m_pTrackCollection,availableDirIds);
    addFeature(m_pPrepareFeature);
    //iTunes and Rhythmbox should be last until we no longer have an obnoxious
    //messagebox popup when you select them. (This forces you to reach for your
    //mouse or keyboard if you're using MIDI control and you scroll through them...)
    if (RhythmboxFeature::isSupported() &&
        pConfig->getValueString(ConfigKey("[Library]","ShowRhythmboxLibrary"),"1").toInt()) {
        addFeature(new RhythmboxFeature(this, m_pTrackCollection));
    }
    if (ITunesFeature::isSupported() &&
        pConfig->getValueString(ConfigKey("[Library]","ShowITunesLibrary"),"1").toInt()) {
        addFeature(new ITunesFeature(this, m_pTrackCollection));
    }
    if (TraktorFeature::isSupported() &&
        pConfig->getValueString(ConfigKey("[Library]","ShowTraktorLibrary"),"1").toInt()) {
        addFeature(new TraktorFeature(this, m_pTrackCollection));
    }

    //Show the promo tracks view on first run, otherwise show the library
    if (firstRun) {
        //qDebug() << "First Run, switching to PROMO view!";
        //This doesn't trigger onShow()... argh
        //m_pSidebarModel->setDefaultSelection(1);
        //slotSwitchToView(tr("Bundled Songs"));
        //Note the promo tracks item has index=1... hardcoded hack. :/
    }
}

Library::~Library() {
    QMutableListIterator<LibraryFeature*> features_it(m_features);
    while(features_it.hasNext()) {
        LibraryFeature* feature = features_it.next();
        features_it.remove();
        delete feature;
    }

    delete m_pLibraryControl;
    delete m_pSidebarModel;
    //IMPORTANT: m_pTrackCollection gets destroyed via the QObject hierarchy somehow.
    //           Qt does it for us due to the way RJ wrote all this stuff.
    //Update:  - OR NOT! As of Dec 8, 2009, this pointer must be destroyed manually otherwise
    // we never see the TrackCollection's destructor being called... - Albert
    delete m_pTrackCollection;
}

void Library::bindWidget(WLibrarySidebar* pSidebarWidget,
                         WLibrary* pLibraryWidget,
                         MixxxKeyboard* pKeyboard) {
    WTrackTableView* pTrackTableView =
            new WTrackTableView(pLibraryWidget, m_pConfig, m_pTrackCollection);
    pTrackTableView->installEventFilter(pKeyboard);
    connect(this, SIGNAL(showTrackModel(QAbstractItemModel*)),
            pTrackTableView, SLOT(loadTrackModel(QAbstractItemModel*)));
    connect(pTrackTableView, SIGNAL(loadTrack(TrackPointer)),
            this, SLOT(slotLoadTrack(TrackPointer)));
    connect(pTrackTableView, SIGNAL(loadTrackToPlayer(TrackPointer, QString)),
            this, SLOT(slotLoadTrackToPlayer(TrackPointer, QString)));
    pLibraryWidget->registerView(m_sTrackViewName, pTrackTableView);

    connect(this, SIGNAL(switchToView(const QString&)),
            pLibraryWidget, SLOT(switchToView(const QString&)));

    m_pLibraryControl->bindWidget(pSidebarWidget, pLibraryWidget, pKeyboard);

    // Setup the sources view
    pSidebarWidget->setModel(m_pSidebarModel);
    connect(m_pSidebarModel, SIGNAL(selectIndex(const QModelIndex&)),
            pSidebarWidget, SLOT(selectIndex(const QModelIndex&)));
    connect(pSidebarWidget, SIGNAL(pressed(const QModelIndex&)),
            m_pSidebarModel, SLOT(clicked(const QModelIndex&)));
    // Lazy model: Let triange symbol increment the model
    connect(pSidebarWidget, SIGNAL(expanded(const QModelIndex&)),
            m_pSidebarModel, SLOT(doubleClicked(const QModelIndex&)));

    connect(pSidebarWidget, SIGNAL(rightClicked(const QPoint&, const QModelIndex&)),
            m_pSidebarModel, SLOT(rightClicked(const QPoint&, const QModelIndex&)));

    QListIterator<LibraryFeature*> feature_it(m_features);
    while(feature_it.hasNext()) {
        LibraryFeature* feature = feature_it.next();
        feature->bindWidget(pSidebarWidget, pLibraryWidget, pKeyboard);
    }

    // Enable the default selection
    pSidebarWidget->selectionModel()
        ->select(m_pSidebarModel->getDefaultSelection(),
                 QItemSelectionModel::SelectCurrent);
    m_pSidebarModel->activateDefaultSelection();
}

void Library::addFeature(LibraryFeature* feature, bool config) {
    Q_ASSERT(feature);
    m_features.push_back(feature);
    m_pSidebarModel->addLibraryFeature(feature);
    connect(feature, SIGNAL(showTrackModel(QAbstractItemModel*)),
            this, SLOT(slotShowTrackModel(QAbstractItemModel*)));
    connect(feature, SIGNAL(switchToView(const QString&)),
            this, SLOT(slotSwitchToView(const QString&)));
    connect(feature, SIGNAL(loadTrack(TrackPointer)),
            this, SLOT(slotLoadTrack(TrackPointer)));
    connect(feature, SIGNAL(loadTrackToPlayer(TrackPointer, QString)),
            this, SLOT(slotLoadTrackToPlayer(TrackPointer, QString)));
    connect(feature, SIGNAL(restoreSearch(const QString&)),
            this, SLOT(slotRestoreSearch(const QString&)));
    if (config) {
        connect(this, SIGNAL(configChanged(QString,QString)),
                feature, SIGNAL(configChanged(QString,QString)));
    }
    connect(this, SIGNAL(availableDirsChanged(QList<int>)),
            feature, SIGNAL(availableDirsChanged(QList<int>)));
}

void Library::slotShowTrackModel(QAbstractItemModel* model) {
    qDebug() << "Library::slotShowTrackModel" << model;
    m_ptrackModel = dynamic_cast<TrackModel*>(model);
    Q_ASSERT(m_ptrackModel);
    emit(showTrackModel(model));
    emit(switchToView(m_sTrackViewName));
    emit(restoreSearch(m_ptrackModel->currentSearch()));
}

void Library::slotSwitchToView(const QString& view) {
    //qDebug() << "Library::slotSwitchToView" << view;
    emit(switchToView(view));
}

void Library::slotLoadTrack(TrackPointer pTrack) {
    emit(loadTrack(pTrack));
}

void Library::slotLoadTrackToPlayer(TrackPointer pTrack, QString group) {
    emit(loadTrackToPlayer(pTrack, group));
}

void Library::slotRestoreSearch(const QString& text) {
    emit(restoreSearch(text));
}

void Library::slotRefreshLibraryModels() {
   m_pMixxxLibraryFeature->refreshLibraryModels();
   m_pPrepareFeature->refreshLibraryModels();
}

void Library::slotCreatePlaylist() {
    m_pPlaylistFeature->slotCreatePlaylist();
}

void Library::slotCreateCrate() {
    m_pCrateFeature->slotCreateCrate();
}


QList<TrackPointer> Library::getTracksToAutoLoad() {
    if (m_pPromoTracksFeature)
        return m_pPromoTracksFeature->getTracksToAutoLoad();
    else
        return QList<TrackPointer>();
}

MixxxLibraryFeature* Library::getpMixxxLibraryFeature(){
    return m_pMixxxLibraryFeature;
}

void Library::slotDirsChanged(QString op, QString dir){
    qDebug() << "kain88 slotDirsChanged";
    qDebug() << op << '\t' << dir;
    if (op=="added") {
        m_directoryDAO.addDirectory(dir);
        m_availableDirs << dir;
        emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
    } else if (op=="removed") {
        purgeTracks(m_directoryDAO.getDirId(dir));
        m_directoryDAO.purgeDirectory(dir);
        m_availableDirs.removeOne(dir);
        emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
    } else if (op=="relocate") {
        // see dlgprefplaylist for this
        QStringList dirs = dir.split("!(~)!");
        QString newFolder = dirs[0];
        QString oldFolder = dirs[1];
        m_directoryDAO.relocateDirectory(oldFolder,newFolder);
        m_availableDirs.removeOne(oldFolder);
        emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
        m_availableDirs << newFolder;
        emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
    } else if (op=="update") {
        // this will be signaled from the library scanner if the db needs to be 
        // updated
        m_directoryDAO.addDirectory(dir);
        m_directoryDAO.updateTrackLocations(dir);
        m_availableDirs << dir;
        emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
    }
}

void Library::purgeTracks(const int dirId) {
    QSqlQuery query;
    QList<int> trackIds;
    query.prepare("SELECT library.id FROM library INNER JOIN track_locations "
                "ON library.location=track_locations.id "
                "WHERE maindir_id="+QString::number(dirId));

    if (!query.exec()) {
        qDebug() << "could not purge tracks from libraryPath "<<dirId;
    }

    while (query.next()) {
        trackIds.append(query.value(query.record().indexOf("id")).toInt());
    }
    qDebug() << "starting to purge Tracks " << trackIds;
    m_pTrackCollection->getTrackDAO().purgeTracks(trackIds); 
}

void Library::slotFoundNewStorage(QStringList newStorage){
    qDebug() << "called new storage";
    foreach (QString dir, m_unavailableDirs) {
        if (dir.contains(QRegExp(newStorage.join("|")))) {
            m_availableDirs << dir;
            m_unavailableDirs.removeOne(dir);
        }
    }
    emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
}

void Library::slotRemovedStorage(QStringList removedStorage){
    qDebug() << "called removed storage";
    foreach (QString dir, m_availableDirs) {
        if (dir.contains(QRegExp(removedStorage.join("|")))) {
            m_unavailableDirs << dir;
            m_availableDirs.removeOne(dir);
        }
    }
    emit availableDirsChanged(m_directoryDAO.getDirIds(m_availableDirs));
}

void Library::slotLoadTrackFailed(TrackPointer pTrack){
    m_pTrackCollection->getTrackDAO().markTrackAsDeleted(pTrack);
}

QStringList Library::getDirs(){
    return m_directoryDAO.getDirs();
}
