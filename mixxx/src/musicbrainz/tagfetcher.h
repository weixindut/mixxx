
#ifndef TAGFETCHER_H
#define TAGFETCHER_H

#include <QFutureWatcher>
#include <QObject>

#include "musicbrainz/musicbrainzclient.h"
#include "trackinfoobject.h"

class AcoustidClient;

class TagFetcher : public QObject {
  Q_OBJECT

  // High level interface to Fingerprinter, AcoustidClient and
  // MusicBrainzClient.

  public:
    TagFetcher(QObject* parent = 0);

    void StartFetch(const QList<TrackPointer>& tracks);

  public slots:
    void Cancel();

    signals:
    void Progress(const TrackPointer original_track, const QString& stage);
    void ResultAvailable(const TrackPointer original_track,
                        const QList<TrackPointer>& tracks_guessed);
    void foobar();

  private slots:
    void FingerprintFound(int index);
    void PuidFound(int index, const QString& puid);
    void TagsFetched(int index, const MusicBrainzClient::ResultList& result);

  private:
    static QString GetFingerprint(const TrackPointer tio);

    QFutureWatcher<QString>* m_pFingerprint_watcher;
    AcoustidClient* m_pAcoustid_client;
    MusicBrainzClient* m_pMusicbrainz_client;

    QList<TrackPointer> m_tracks;
};

#endif // TAGFETCHER_H
