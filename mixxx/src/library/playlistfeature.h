// playlistfeature.h
// Created 8/17/09 by RJ Ryan (rryan@mit.edu)

#ifndef PLAYLISTFEATURE_H
#define PLAYLISTFEATURE_H

#include <QSqlTableModel>

#include "library/baseplaylistfeature.h"
#include "configobject.h"

class TrackCollection;

class PlaylistFeature : public BasePlaylistFeature {
    Q_OBJECT
  public:
    PlaylistFeature(QObject* parent, 
                    TrackCollection* pTrackCollection,
                    ConfigObject<ConfigValue>* pConfig,
                    QStringList availableDirs);
    virtual ~PlaylistFeature();

    QVariant title();
    QIcon getIcon();

    bool dropAcceptChild(const QModelIndex& index, QList<QUrl> urls);
    bool dragMoveAcceptChild(const QModelIndex& index, QUrl url);

  signals:
    void configChanged(QString,QString);

  public slots:
    void onRightClick(const QPoint& globalPos);
    void onRightClickChild(const QPoint& globalPos, QModelIndex index);
    void slotConfigChanged(QString, QString);

  private slots:
    void slotPlaylistTableChanged(int playlistId);

 protected:
    QModelIndex constructChildModel(int selected_id);

  private:
    virtual QString getRootViewHtml() const;
};

#endif /* PLAYLISTFEATURE_H */
