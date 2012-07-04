// mixxxlibraryfeature.h
// Created 8/23/2009 by RJ Ryan (rryan@mit.edu)

#ifndef MIXXXLIBRARYFEATURE_H
#define MIXXXLIBRARYFEATURE_H

#include <QStringListModel>

#include "library/dao/directorydao.h"
#include "library/libraryfeature.h"
#include "treeitemmodel.h"
#include "configobject.h"

class BaseTrackCache;
class LibraryTableModel;
class HiddenTableModel;
class TrackCollection;
// class DirectoryDAO;

class MixxxLibraryFeature : public LibraryFeature {
    Q_OBJECT
    public:
    MixxxLibraryFeature(QObject* parent,
                        TrackCollection* pTrackCollection,
                        ConfigObject<ConfigValue>* pConfig);
    virtual ~MixxxLibraryFeature();

    QVariant title();
    QIcon getIcon();
    bool dropAccept(QUrl url);
    bool dropAcceptChild(const QModelIndex& index, QUrl url);
    bool dragMoveAccept(QUrl url);
    bool dragMoveAcceptChild(const QModelIndex& index, QUrl url);
    TreeItemModel* getChildModel();
    QStringList getDirs();

  signals:
    void configChanged(QString, QString);

  public slots:
    void activate();
    void activateChild(const QModelIndex& index);
    void onRightClick(const QPoint& globalPos);
    void onRightClickChild(const QPoint& globalPos, QModelIndex index);
    void onLazyChildExpandation(const QModelIndex& index);
    void refreshLibraryModels();
    void slotDirsChanged(QString,QString);

  private:
    const QString kHiddenTitle;
    QSharedPointer<BaseTrackCache> m_pBaseTrackCache;
    LibraryTableModel* m_pLibraryTableModel;
    HiddenTableModel* m_pHiddenTableModel;
    TreeItemModel m_childModel;
    DirectoryDAO m_directoryDAO;
};

#endif /* MIXXXLIBRARYFEATURE_H */
