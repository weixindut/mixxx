#include <QDebug>
#include "controllers/wao/presetobjectwao.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"

using namespace QtJson;
PresetObjectWAO::PresetObjectWAO() {}
QList<MidiControllerPreset> PresetObjectWAO::getPresetByPresetName(QString name) {
	QString url="http://127.0.0.1:8000/api/v1/midi/preset/?preset_name="+name+"&format=json";
	return getPresetByURL(url);
}
QList<MidiControllerPreset> PresetObjectWAO::getPresetByURL(QString url) {
	QList<MidiControllerPreset> presetList;
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
        QString author = res["author"].toString();
        QString url = res["url"].toString();
        QString description = res["description"].toString();
        QString preset_source = res["preset_source"].toString();
        QString preset_status = res["preset_status"].toString();
        QString mixxx_version = res["version"].toString();
        QString preset_name = res["preset_name"].toString();
        QString controller_name = res["controller_name"].toString();
        QString schema_version = res["schema_version"].toString();

        MidiControllerPreset controllerpreset;
        controllerpreset.setPid(pid);
        controllerpreset.setPresetStatus(preset_status);
        controllerpreset.setPresetSource(preset_source);
        controllerpreset.setMixxxVersion(mixxx_version);
        controllerpreset.setSchemaVersion(schema_version);
        controllerpreset.setDescription(description);
        controllerpreset.setAuthor(author);
        controllerpreset.setName(preset_name);
        controllerpreset.setFilePath("");
        controllerpreset.setDeviceId(controller_name);
        if (url.contains("forums")) {
            controllerpreset.setForumLink(url);
            controllerpreset.setWikiLink("");
        } else if (url.contains("wiki")) {
        	controllerpreset.setForumLink("");
        	controllerpreset.setWikiLink(url);
        } else {
        	controllerpreset.setForumLink("");
        	controllerpreset.setWikiLink("");
        }

        presetList.append(controllerpreset);
    }
    return presetList;
}
