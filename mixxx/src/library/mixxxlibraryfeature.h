// mixxxlibraryfeature.h
// Created 8/23/2009 by RJ Ryan (rryan@mit.edu)

#ifndef MIXXXLIBRARYFEATURE_H
#define MIXXXLIBRARYFEATURE_H

#include <QStringListModel>

#include "library/libraryfeature.h"
#include "configobject.h"
#include "library/dao/trackdao.h"
#include "treeitemmodel.h"

class BaseTrackCache;
class LibraryTableModel;
class MissingTableModel;
class HiddenTableModel;
class TrackCollection;

class MixxxLibraryFeature : public LibraryFeature {
    Q_OBJECT
    public:
    MixxxLibraryFeature(QObject* parent,
                        TrackCollection* pTrackCollection,
                        ConfigObject<ConfigValue>* pConfig);
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

  public slots:
    void activate();
    void activateChild(const QModelIndex& index);
    void onRightClick(const QPoint& globalPos);
    void onRightClickChild(const QPoint& globalPos, QModelIndex index);
    void onLazyChildExpandation(const QModelIndex& index);
    void refreshLibraryModels();

  private:
    TrackCollection* m_pTrackCollection;
    const QString kMissingTitle;
    const QString kHiddenTitle;
    QSharedPointer<BaseTrackCache> m_pBaseTrackCache;
    LibraryTableModel* m_pLibraryTableModel;
    MissingTableModel* m_pMissingTableModel;
    HiddenTableModel* m_pHiddenTableModel;
    TreeItemModel m_childModel;
    TrackDAO& m_trackDao;
};

#endif /* MIXXXLIBRARYFEATURE_H */
