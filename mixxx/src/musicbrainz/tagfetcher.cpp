#include <QFuture>
#include <QFutureWatcher>
#include <QUrl>
#include <QtConcurrentMap>

#include "musicbrainz/tagfetcher.h"
#include "musicbrainz/acoustidclient.h"
#include "library/chromaprinter.h"
#include "musicbrainz/musicbrainzclient.h"

TagFetcher::TagFetcher(QObject* parent)
          : QObject(parent),
            m_pFingerprint_watcher(NULL),
            m_pAcoustid_client(new AcoustidClient(this)),
            m_pMusicbrainz_client(new MusicBrainzClient(this)) {
    connect(m_pAcoustid_client, SIGNAL(Finished(int,QString)),
            this, SLOT(PuidFound(int,QString)));
    connect(m_pMusicbrainz_client, SIGNAL(Finished(int,MusicBrainzClient::ResultList)),
            this, SLOT(TagsFetched(int,MusicBrainzClient::ResultList)));
}

QString TagFetcher::GetFingerprint(const TrackPointer tio) {
    return chromaprinter(NULL).getFingerPrint(tio);
}

void TagFetcher::StartFetch(const QList<TrackPointer>& tracks) {
    Cancel();

    m_tracks = tracks;

    QFuture<QString> future = QtConcurrent::mapped(m_tracks, GetFingerprint);
    m_pFingerprint_watcher = new QFutureWatcher<QString>(this);
    m_pFingerprint_watcher->setFuture(future);
    connect(m_pFingerprint_watcher, SIGNAL(resultReadyAt(int)), SLOT(FingerprintFound(int)));

    foreach (const TrackPointer ptrack, m_tracks) {
        emit Progress(ptrack, tr("Fingerprinting track"));
    }
}

void TagFetcher::Cancel() {
    if (m_pFingerprint_watcher) {
        m_pFingerprint_watcher->cancel();

        delete m_pFingerprint_watcher;
        m_pFingerprint_watcher = NULL;
    }

    m_pAcoustid_client->CancelAll();
    m_pMusicbrainz_client->CancelAll();
    m_tracks.clear();
}

void TagFetcher::FingerprintFound(int index) {
    QFutureWatcher<QString>* watcher = reinterpret_cast<QFutureWatcher<QString>*>(sender());
    if (!watcher || index >= m_tracks.count()) {
        return;
    }

    const QString fingerprint = watcher->resultAt(index);
    const TrackPointer ptrack = m_tracks[index];

    if (fingerprint.isEmpty()) {
        emit ResultAvailable(ptrack, QList<TrackPointer>());
        return;
    }

    emit Progress(ptrack, tr("Identifying track"));
    qDebug() << "start to look it up on musicbrainz";
    m_pAcoustid_client->Start(index, fingerprint, ptrack->getDuration());
}

void TagFetcher::PuidFound(int index, const QString& puid) {
    if (index >= m_tracks.count()) {
        return;
    }

    const TrackPointer pTrack = m_tracks[index];

    if (puid.isEmpty()) {
        emit ResultAvailable(pTrack, QList<TrackPointer>());
        return;
    }

    emit Progress(pTrack, tr("Downloading metadata"));
    m_pMusicbrainz_client->Start(index, puid);
}

void TagFetcher::TagsFetched(int index, const MusicBrainzClient::ResultList& results) {
    if (index >= m_tracks.count()) {
        return;
    }
    qDebug() << "Tagfetcher got musicbrainz results and now refurbrishs them";
    const TrackPointer original_track = m_tracks[index];
    QList<TrackPointer> tracksGuessed;

    foreach (const MusicBrainzClient::Result& result, results) {
        TrackPointer track(new TrackInfoObject(original_track->getLocation(),false),
                           &QObject::deleteLater);
        track->setTitle(result.title_);
        track->setArtist(result.artist_);
        track->setAlbum(result.album_);
        track->setDuration(result.duration_msec_*1000000000);
        track->setTrackNumber(QString::number(result.track_));
        track->setYear(QString::number(result.year_));
        tracksGuessed << track;
    }
    qDebug() << "send this to the world";
    emit foobar();
    emit ResultAvailable(original_track, tracksGuessed);
}
