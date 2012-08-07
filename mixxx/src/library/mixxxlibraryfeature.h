// mixxxlibraryfeature.h
// Created 8/23/2009 by RJ Ryan (rryan@mit.edu)

#ifndef MIXXXLIBRARYFEATURE_H
#define MIXXXLIBRARYFEATURE_H

#include <QStringListModel>

#include "library/dao/directorydao.h"
#include "library/libraryfeature.h"
#include "configobject.h"

class BaseTrackCache;
class LibraryTableModel;
class HiddenTableModel;
class TrackCollection;

class MixxxLibraryFeature : public LibraryFeature {
    Q_OBJECT
    public:
    MixxxLibraryFeature(QObject* parent,
                        TrackCollection* pTrackCollection,
                        ConfigObject<ConfigValue>* pConfig,
                        QList<int> availableDirIds);
    virtual ~MixxxLibraryFeature();

    QVariant title();
    QIcon getIcon();
    bool dropAccept(QList<QUrl> urls);
    bool dropAcceptChild(const QModelIndex& index, QList<QUrl> urls);
    bool dragMoveAccept(QUrl url);
    bool dragMoveAcceptChild(const QModelIndex& index, QUrl url);
    TreeItemModel* getChildModel();

  signals:
    void configChanged(QString, QString);
    void dirsChanged(QString,QString);
    void loadTrackFailed(TrackPointer);

  public slots:
    void activate();
    void activateChild(const QModelIndex& index);
    void onRightClick(const QPoint& globalPos);
    void onRightClickChild(const QPoint& globalPos, QModelIndex index);
    void onLazyChildExpandation(const QModelIndex& index);
    void refreshLibraryModels();
    void slotDirsChanged(QString,QString);

  private:
    TrackCollection* m_pTrackCollection;
    const QString kHiddenTitle;
    QSharedPointer<BaseTrackCache> m_pBaseTrackCache;
    LibraryTableModel* m_pLibraryTableModel;
    HiddenTableModel* m_pHiddenTableModel;
    TreeItemModel m_childModel;
    DirectoryDAO m_directoryDAO;
};

#endif /* MIXXXLIBRARYFEATURE_H */
