#include <QDebug>
#include "controllers/wao/presetobjectwao.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"

using namespace QtJson;
PresetObjectWAO::PresetObjectWAO() {}
QList<PresetObject> PresetObjectWAO::getPresetByName(QString name) {

    QString url="http://127.0.0.1:8000/api/v1/midi/preset/?controller_name="+name+"&format=json";
    return getPresetByURL(url);
}

QList<PresetObject>  PresetObjectWAO::getPresetByCompanyName(QString name) {
	QString url="http://127.0.0.1:8000/api/v1/midi/preset/?company_name="+name+"&format=json";
	return getPresetByURL(url);
}
QList<PresetObject> PresetObjectWAO::getPresetByPresetName(QString name) {
	QString url="http://127.0.0.1:8000/api/v1/midi/preset/?preset_name="+name+"&format=json";
	return getPresetByURL(url);
}
QList<PresetObject> PresetObjectWAO::getPresetByURL(QString url) {
	QList<PresetObject> presetList;
	HttpClient httpclient;
    bool ok;
    QString data = httpclient.get(url);
    qDebug() << "Print JsonObject:"+data;
    QVariantMap result = QtJson::parse(data,ok).toMap();
    if(!ok) {
        exit(1);
    }
    foreach(QVariant plugin, result["objects"].toList()) {
        QVariantMap res = plugin.toMap();
        QString pid = res["pid"].toString();
        QString author_name = res["autor"].toString();
        QUrl url = res["url"].toUrl();
        QString description = res["description"].toString();
        QString preset_source = res["preset_source"].toString();
        QString preset_status = res["preset_status"].toString();
        QString mixxx_version = res["version"].toString();
        QString preset_name = res["preset_name"].toString();
        QString midi_controller_name = res["controller_name"].toString();
        QString company_name = res["company_name"].toString();
        QDateTime datetime = res["datetime"].toDateTime();
        presetList.append(PresetObject(pid,author_name,url,
                description, preset_source, preset_status,
                mixxx_version, preset_name,midi_controller_name,
                company_name,datetime));
        }
    return presetList;
}
