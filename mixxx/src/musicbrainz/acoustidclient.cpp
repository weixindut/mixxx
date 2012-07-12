#include <QtNetwork>
#include <QCoreApplication>
#include <QNetworkReply>
#include <QXmlStreamReader>

#include "acoustidclient.h"
#include "network.h"

const char* AcoustidClient::m_pClientId = "ErlAvPUB";
const char* AcoustidClient::m_pUrl = "http://api.acoustid.org/v2/lookup";
const int AcoustidClient::m_DefaultTimeout = 5000; // msec

AcoustidClient::AcoustidClient(QObject* parent)
              : QObject(parent),
                m_pnetwork(new QNetworkAccessManager(this)),
                m_ptimeouts(new NetworkTimeouts(m_DefaultTimeout, this)){
}

void AcoustidClient::SetTimeout(int msec) {
    m_ptimeouts->SetTimeout(msec);
}

void AcoustidClient::Start(int id, const QString& fingerprint, int duration) {
    typedef QPair<QString, QString> Param;
    
    QList<Param> parameters;
    parameters << Param("format", "xml")
                << Param("client", m_pClientId)
                << Param("duration", QString::number(duration))
                << Param("meta", "recordingids")
                << Param("fingerprint", fingerprint);
    
    QUrl url(m_pUrl);
    url.setQueryItems(parameters);
    QNetworkRequest req(url);
    
    QNetworkReply* reply = m_pnetwork->get(req);
    connect(reply, SIGNAL(finished()), SLOT(RequestFinished()));
    m_requests[reply] = id;
    
    m_ptimeouts->AddReply(reply);
}

void AcoustidClient::Cancel(int id) {
    QNetworkReply* reply = m_requests.key(id);
    m_requests.remove(reply);
    delete reply;
}

void AcoustidClient::CancelAll() {
    qDeleteAll(m_requests.keys());
    m_requests.clear();
}

void AcoustidClient::RequestFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    
    reply->deleteLater();
    if (!m_requests.contains(reply))
        return;
    
    int id = m_requests.take(reply);
    
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        emit Finished(id, QString());
        return;
    }

    QXmlStreamReader reader(reply);
    QString ID;
    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement 
            && reader.name()== "results") {
                ID = ParseResult(reader);
            }
    }
    qDebug() << "found something " << ID;
    emit Finished(id, ID);
}

QString AcoustidClient::ParseResult(QXmlStreamReader& reader){

    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType type = reader.readNext();

        if (type== QXmlStreamReader::StartElement) {
            QStringRef name = reader.name();

            if (name == "id") {
                return reader.readElementText();
            }
        }

        if (type == QXmlStreamReader::EndElement && reader.name()=="result") {
            break;
        }

    }
    return QString();
}
