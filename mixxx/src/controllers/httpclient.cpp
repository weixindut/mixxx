#include <QApplication>
#include <QEventLoop>
#include <QRegExp>
#include <QStringList>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkRequest>
#include <QFileInfo>
#include <QDebug>
#include "httpclient.h"

HttpClient::HttpClient(QObject* parent) : QObject(parent) {
    m_manager = new QNetworkAccessManager(this);
    m_manager->setCookieJar(new QNetworkCookieJar(this));
    m_textCodec = NULL;
}

HttpClient::~HttpClient() {
    delete m_manager;
}

QString HttpClient::post(const QString& url, const QByteArray& postData) {
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = m_manager->post(request, postData);
    waitForFinish(reply);

    // check redirect
    QByteArray location = reply->rawHeader(QString("Location").toAscii());
    if (location.size() > 0) {
        return get(QString(location));
    } else {
        QByteArray replyData = reply->readAll();
        if (m_textCodec != NULL) {
            return m_textCodec->toUnicode(replyData);
        } else {
            return QString(replyData);
        }
    }
}

QString HttpClient::post(const QString& url, QMap<QString, QString>& postData) {
    QStringList data;
    foreach (QString key, postData.keys()) {
        data.append(key + "=" + postData[key]);
    }
    return post(url, data.join("&").toAscii());
}
QString HttpClient::postFile(const QString& url, const QString path) {
    if (!QFile::exists(path)) {
        qDebug() << "Filepath does not exist";
        return "";
    }
    QByteArray dataToSend; // byte array to be sent in POST
    dataToSend.append(path+"&");
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return "";
    dataToSend.append(file.readAll());
    file.close();
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply* reply = m_manager->post(request, dataToSend);
    waitForFinish(reply);
    qDebug()<< "**************************";
    QByteArray replyData = reply->readAll();
    return QString(replyData);

}

QString HttpClient::get(const QString& url) {
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply* reply = m_manager->get(request);
    waitForFinish(reply);

    // check redirect
    QByteArray location = reply->rawHeader(QString("Location").toAscii());
    if (location.size() > 0) {
        return get(QString(location));
    } else {
        QByteArray replyData = reply->readAll();
        if (m_textCodec != NULL) {
            return m_textCodec->toUnicode(replyData);
        } else {
            return QString(replyData);
        }
    }
}

void HttpClient::setTextCodec(const QString& encoding) {
    m_textCodec = QTextCodec::codecForName(encoding.toAscii());
}

QMap<QString, QString> HttpClient::allCookies(const QString& url) {
    QMap<QString, QString> map;
    QList<QNetworkCookie> list = m_manager->cookieJar()->cookiesForUrl(QUrl(url));
    foreach (QNetworkCookie cookie, list) {
        map.insert(QString(cookie.name()), QString(cookie.value()));
    }
    return map;
}

QMap<QString, QString> HttpClient::defaultValuesFromName(const QString& html, const QString& name) {
    QMap<QString, QString> map;

    // get form
    QRegExp rx("<form.*name=\"" + name + "\".*>.*</form>");
    rx.setMinimal(true);
    if (rx.indexIn(html, 0) == -1) {
        return map;
    }
    QString form = rx.cap(0);

    // get inputs
    rx.setPattern("<(input|select).*name=\"([^\"]+)\".*(value=\"([^\"]+)\")?.*/?>");
    int pos = 0;
    while ((pos = rx.indexIn(form, pos)) != -1) {
        map.insert(rx.cap(2), rx.cap(4));
        pos += rx.matchedLength();
     }

    return map;
}

void HttpClient::waitForFinish(QNetworkReply* reply) {
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

QString HttpClient::doDownload(QString destDirecotry, const QUrl& url) {
	QString fileName;
	QString filePath;
    QNetworkRequest request(url);
    QNetworkReply* reply = m_manager->get(request);
    m_currentDownloads.append(reply);
    waitForFinish(reply);
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
    	fileName = saveFileName(url);
        if (saveToDisk(destDirecotry, fileName, reply)) {
        	printf("Download of %s succeeded (saved to %s)\n",
        	        url.toEncoded().constData(), qPrintable(fileName));
        	filePath = destDirecotry+"/"+fileName;
        }
    }
    m_currentDownloads.removeAll(reply);
    reply->deleteLater();
    if (m_currentDownloads.isEmpty()) {
        qDebug()<<"all downloads finished";
    }
    return filePath;
}

QString HttpClient::saveFileName(const QUrl& url) {
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "index.html";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i))) {
            ++i;
        }
        basename += QString::number(i);
    }

    return basename;
}

bool HttpClient::saveToDisk(QString destDirecotry, const QString& filename, QIODevice* data) {
    QFile file;
    file.setFileName(destDirecotry+filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

QString HttpClient::downloadFile(QString destDirecotry,const QString path) {
    QUrl url;
    url.setHost("127.0.0.1");
    url.setPort(8000);
    url.setScheme("http");
    url.setPath(path);
    return doDownload(destDirecotry, url);
}
