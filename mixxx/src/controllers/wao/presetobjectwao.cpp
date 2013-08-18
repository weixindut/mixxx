#include <QDebug>

#include "controllers/wao/presetobjectwao.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"

using namespace QtJson;
PresetObjectWAO::PresetObjectWAO() {}
QList<MidiControllerPreset> PresetObjectWAO::getPresetByPresetName(QString name) {
	QString url=generateQueryStr(name);
    return getPresetByURL(url);
}
QList<MidiControllerPreset> PresetObjectWAO::checkForUpdate(QString presetname, QString controller) {
	QString url="http://127.0.0.1:8000/api/v1/midi/preset/updatecheck?preset_name="
	        +presetname+"&controller="+controller+"&format=json";
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
    	qDebug()<<"There is something wrong with server side\n";
    }
    foreach(QVariant plugin, result["objects"].toList()) {
        QVariantMap res = plugin.toMap();
        QString pid = res["pid"].toString();
        QString author = "";
        foreach(QVariant authorname, res["author"].toList()) {
            if(author.isEmpty()) {
                author.append(authorname.toString());
            } else {
            	author.append(",");
            	author.append(authorname.toString());
            }
        }
        QString url = res["url"].toString();
        QString description = res["description"].toString();
        QString preset_source = res["preset_source"].toString();
        QString preset_status = res["preset_status"].toString();
        QString mixxx_version = res["version"].toString();
        QString preset_name = res["preset_name"].toString();
        QString controller_name = res["controller_name"].toString();
        QString schema_version = res["schema_version"].toString();
        QString picture_file = res["picture_file"].toString();
        float avg_rating = res["avg_ratings"].toFloat();
        QString picture_name;
        if (!picture_file.isEmpty()) {
            picture_name = httpclient.downloadFile(picture_file);
        } else {
            picture_name = "";
        }

        //QString xml_file = res["xml_file"].toString();
        //httpclient.downloadFile(xml_file);

        MidiControllerPreset controllerpreset;
        controllerpreset.setPid(pid);
        controllerpreset.setAuthor(author);
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
        controllerpreset.setDescription(description);
        controllerpreset.setPresetSource(preset_source);
        controllerpreset.setPresetStatus(preset_status);
        controllerpreset.setMixxxVersion(mixxx_version);
        controllerpreset.setName(preset_name);
        controllerpreset.setDeviceId(controller_name);
        controllerpreset.setSchemaVersion(schema_version);
        //controllerpreset.setPicturePath("./tmp/"+picture_name);
        // currently server only return one picture file or none
        if (!picture_name.isEmpty()) {
        	controllerpreset.addPictureFile(picture_name);
        }
        controllerpreset.setRatings(avg_rating);
        controllerpreset.setFilePath("");

        presetList.append(controllerpreset);
    }
    return presetList;
}
QString PresetObjectWAO::generateQueryStr(QString name) {

    name.replace(" ",",");
    name.replace("-",",");
    name.replace("_",",");
    name.replace(".",",");

    QStringList words = name.split(",",QString::SkipEmptyParts);
    QString url="http://127.0.0.1:8000/api/v1/midi/preset/search?";
    for(int i=0; i<words.size(); i++) {
        if(i==0) {
        	url.append("q=");
        	url.append(words[i]);
        } else {
        	url.append(" &q=");
        	url.append(words[i]);
    	}
    }
    url.append("&format=json");
    return url;
}
