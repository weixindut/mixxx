#include "musicbrainzclient.h"
#include "network.h"

#include <QCoreApplication>
#include <QNetworkReply>
#include <QtNetwork>
#include <QSet>
#include <QXmlStreamReader>

const char* MusicBrainzClient::m_pTrackUrl = "http://musicbrainz.org/ws/2/recording/";
const char* MusicBrainzClient::m_pDiscUrl = "http://musicbrainz.org/ws/1/release/";
const char* MusicBrainzClient::m_pDateRegex = "^[12]\\d{3}";
const int MusicBrainzClient::m_DefaultTimeout = 5000; // msec

MusicBrainzClient::MusicBrainzClient(QObject* parent)
                 : QObject(parent),
                   m_network(new QNetworkAccessManager(this)),
                   m_ptimeouts(new NetworkTimeouts(m_DefaultTimeout, this)){
}

void MusicBrainzClient::Start(int id, const QString& mbid) {
  typedef QPair<QString, QString> Param;

  QList<Param> parameters;
  parameters << Param("inc", "artists+releases+media");

  QUrl url(m_pTrackUrl + mbid);
  url.setQueryItems(parameters);
  QNetworkRequest req(url);

  QNetworkReply* reply = m_network->get(req);
  connect(reply, SIGNAL(finished()), SLOT(RequestFinished()));
  m_requests[reply] = id;

  m_ptimeouts->AddReply(reply);
}

void MusicBrainzClient::StartDiscIdRequest(const QString& discid) {
  typedef QPair<QString, QString> Param;

  QList<Param> parameters;
  parameters << Param("type", "xml")
             << Param("discid", discid);

  QUrl url(m_pDiscUrl);
  url.setQueryItems(parameters);
  QNetworkRequest req(url);

  QNetworkReply* reply = m_network->get(req);
  connect(reply, SIGNAL(finished()), SLOT(DiscIdRequestFinished()));
  //m_requests[reply] = id;

  m_ptimeouts->AddReply(reply);
}

void MusicBrainzClient::Cancel(int id) {
  QNetworkReply* reply = m_requests.key(id);
  m_requests.remove(reply);
  delete reply;
}

void MusicBrainzClient::CancelAll() {
  qDeleteAll(m_requests.keys());
  m_requests.clear();
}

void MusicBrainzClient::DiscIdRequestFinished() {
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;
  reply->deleteLater();

  ResultList ret;
  QString artist;
  QString album;

  if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
    emit Finished(artist, album, ret);
    return;
  }



  // Parse xml result:
  // -get title
  // -get artist
  // -get all the tracks' tags
  QXmlStreamReader reader(reply);
  while (!reader.atEnd()) {
    QXmlStreamReader::TokenType type = reader.readNext();
    if (type == QXmlStreamReader::StartElement) {
      QStringRef name = reader.name();
      if (name == "title") {
        album = reader.readElementText();
      } else if (name == "artist") {
        ParseArtist(reader, &artist);
      } else if (name == "track-list") {
        break;
      }
    }
  }

  while (!reader.atEnd()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement && reader.name() == "track") {
      ResultList tracks = ParseTrack(reader);
      foreach (const Result& track, tracks) {
        if (!track.title_.isEmpty()) {
          ret << track;
        }
      }
    } else if (token == QXmlStreamReader::EndElement && reader.name() == "track-list") {
      break;
    }
  }


  emit Finished(artist, album, UniqueResults(ret));
}


void MusicBrainzClient::RequestFinished() {
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;

  reply->deleteLater();
  if (!m_requests.contains(reply))
    return;

  int id = m_requests.take(reply);
  ResultList ret;

  if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
    emit Finished(id, ret);
    return;
  }
  qDebug() << "received reply from musicbrainz start to parse xml now";
  QXmlStreamReader reader(reply);
  while (!reader.atEnd()) {
    if (reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "recording") {
      ResultList tracks = ParseTrack(reader);
      foreach (const Result& track, tracks) {
        if (!track.title_.isEmpty()) {
          ret << track;
        }
      }
    }
  }
  qDebug() << "emit musicbrainz results";
  emit Finished(id, UniqueResults(ret));
}

MusicBrainzClient::ResultList MusicBrainzClient::ParseTrack(QXmlStreamReader& reader) {
  Result result;
  QList<Release> releases;

  while (!reader.atEnd()) {
    QXmlStreamReader::TokenType type = reader.readNext();

    if (type == QXmlStreamReader::StartElement) {
      QStringRef name = reader.name();

      if (name == "title") {
        result.title_ = reader.readElementText();
      } else if (name == "length") {
        result.duration_msec_ = reader.readElementText().toInt();
      } else if (name == "artist") {
        ParseArtist(reader, &result.artist_);
      } else if (name == "release") {
        releases << ParseRelease(reader);
      }
    }

    if (type == QXmlStreamReader::EndElement && reader.name() == "recording") {
      break;
    }
  }

  ResultList ret;
  foreach (const Release& release, releases) {
    ret << release.CopyAndMergeInto(result);
  }
  return ret;
}

void MusicBrainzClient::ParseArtist(QXmlStreamReader& reader, QString* artist) {
  while (!reader.atEnd()) {
    QXmlStreamReader::TokenType type = reader.readNext();

    if (type == QXmlStreamReader::StartElement && reader.name() == "name") {
      *artist = reader.readElementText();
    }

    if (type == QXmlStreamReader::EndElement && reader.name() == "artist") {
      return;
    }
  }
}

MusicBrainzClient::Release MusicBrainzClient::ParseRelease(QXmlStreamReader& reader) {
  Release ret;

  while (!reader.atEnd()) {
    QXmlStreamReader::TokenType type = reader.readNext();

    if (type == QXmlStreamReader::StartElement) {
      QStringRef name = reader.name();
      if (name == "title") {
        ret.album_ = reader.readElementText();
      } else if (name == "date") {
        QRegExp regex(m_pDateRegex);
        if (regex.indexIn(reader.readElementText()) == 0) {
          ret.year_ = regex.cap(0).toInt();
        }
      } else if (name == "track-list") {
        ret.track_ = reader.attributes().value("offset").toString().toInt() + 1;
        ConsumeCurrentElement(reader);
      }
    }

    if (type == QXmlStreamReader::EndElement && reader.name() == "release") {
      break;
    }
  }

  return ret;
}

MusicBrainzClient::ResultList MusicBrainzClient::UniqueResults(const ResultList& results) {
  ResultList ret = QSet<Result>::fromList(results).toList();
  qSort(ret);
  return ret;
}

void MusicBrainzClient::ConsumeCurrentElement(QXmlStreamReader& reader) {
  int level = 1;
  while (level != 0 && !reader.atEnd()) {
    switch (reader.readNext()) {
      case QXmlStreamReader::StartElement: ++level; break;
      case QXmlStreamReader::EndElement:   --level; break;
      default: break;
    }
  }
}
