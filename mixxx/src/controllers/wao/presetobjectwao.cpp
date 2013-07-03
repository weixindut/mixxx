#include <QDebug>
#include "controllers/wao/presetobjectwao.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"

using namespace QtJson;
PresetObjectWAO::PresetObjectWAO() {}
QList<PresetObject> PresetObjectWAO::getPresetByName(QString name) {
	QList<PresetObject> presetList;
	HttpClient httpclient;
    bool ok;
    QString url="http://127.0.0.1:8000/api/v1/midi/preset/?midi_controller__controller_name="+name+"&format=json";
    QString data = httpclient.get(url);
    QString tmpdata;
    QVariantMap tmpresult;
    qDebug() << "Print JsonObject:"+data;
    QVariantMap result = QtJson::parse(data,ok).toMap();
    if(!ok) {
    	exit(1);
    }
    foreach(QVariant plugin, result["objects"].toList()) {
    	QVariantMap res = plugin.toMap();
    	QString m_pid = res["pid"].toString();
    	QString m_author_name = res["autor"].toString();
    	QUrl m_url = res["url"].toUrl();
    	QString m_description = res["description"].toString();
    	QString m_preset_source = res["preset_source"].toString();
    	QString m_preset_status = res["preset_status"].toString();
    	QString m_mixxx_version = res["version"].toString();
    	QString m_preset_name = res["preset_name"].toString();
    	QString m_midi_controller_id = res["controller"].toString();
    	QDateTime m_datetime = res["datetime"].toDateTime();
    	presetList.append(PresetObject(m_pid,m_author_name,m_url,
    			m_description,m_preset_source, m_preset_status,
    			m_mixxx_version, m_preset_name,m_midi_controller_id,
    			m_datetime));
    }
    return presetList;
}

void PresetObjectWAO::getPresetByCompanyName(QString name) {

}
