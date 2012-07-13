
#ifndef TAGFETCHER_H
#define TAGFETCHER_H

#include <QFutureWatcher>
#include <QObject>

#include "musicbrainz/musicbrainzclient.h"
#include "musicbrainz/acoustidclient.h"
#include "trackinfoobject.h"


class TagFetcher : public QObject {
  Q_OBJECT

  // High level interface to Fingerprinter, AcoustidClient and
  // MusicBrainzClient.

  public:
    TagFetcher(QObject* parent = 0);

    void StartFetch(const TrackPointer track);

  public slots:
    void Cancel();

    signals:
    void Progress(const TrackPointer originalTrack, const QString& stage);
    void ResultAvailable(const TrackPointer originalTrack,
                         const QList<TrackPointer>& tracksGuessed);

  private slots:
    void FingerprintFound(int index);
    void MbidFound(int index, const QString& mbid);
    void TagsFetched(int index, const MusicBrainzClient::ResultList& result);

  private:
    static QString GetFingerprint(const TrackPointer tio);

    QFutureWatcher<QString>* m_pFingerprintWatcher;
    AcoustidClient m_AcoustidClient;
    MusicBrainzClient m_MusicbrainzClient;

    QList<TrackPointer> m_tracks;
};

#endif // TAGFETCHER_H
