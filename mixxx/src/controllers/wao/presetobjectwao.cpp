#include <QDebug>

#include "controllers/wao/presetobjectwao.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"

using namespace QtJson;
PresetObjectWAO::PresetObjectWAO() {}
QList<MidiControllerPreset> PresetObjectWAO::getPresetByPresetName(QString destDirecotry, QString name) {
	QString url=generateQueryStr(name);
    return getPresetByURL(destDirecotry, url);
}
QList<MidiControllerPreset> PresetObjectWAO::getPresetByPresetID(QString destDirecotry, QString pid) {
	QString url="http://127.0.0.1:8000/api/v1/midi/preset/?pid="
	        +pid+"&format=json";
    return getPresetByURL(destDirecotry, url);
}
QList<MidiControllerPreset> PresetObjectWAO::checkForUpdate(QString presetname, QString controller) {
	QString url="http://127.0.0.1:8000/api/v1/midi/preset/updatecheck?preset_name="
	        +presetname+"&controller="+controller+"&format=json";
	return getPresetByURL("/tmp/",url);
}
QList<MidiControllerPreset> PresetObjectWAO::getPresetByURL(QString destDirecotry, QString url) {
    QList<MidiControllerPreset> presetList;
    HttpClient httpClient;
    bool ok;
    QString data = httpClient.get(url);
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
        QString presetSource = res["preset_source"].toString();
        QString presetStatus = res["preset_status"].toString();
        QString mixxxVersion = res["version"].toString();
        QString presetName = res["preset_name"].toString();
        QString controllerName = res["controller_name"].toString();
        QString schemaVersion = res["schema_version"].toString();
        QString pictureFile = res["picture_file"].toString();
        QString jsFile = res["js_file"].toString();
        QString xmlFile = res["xml_file"].toString();
        float avgRating = res["avg_ratings"].toFloat();
        QString picturePath;
        QString jsPath;
        QString xmlPath;
        if (!pictureFile.isEmpty()) {
        	picturePath = httpClient.downloadFile(destDirecotry,pictureFile);
        } else {
        	picturePath = "";
        }
        if (!jsFile.isEmpty()) {
        	jsPath = httpClient.downloadFile(destDirecotry,jsFile);
        } else {
        	jsPath = "";
        }
        if (!xmlFile.isEmpty()) {
        	xmlPath = httpClient.downloadFile(destDirecotry,xmlFile);
        	qDebug()<<"xmlpath:========="+xmlPath;
        } else {
        	xmlPath = "";
        	qDebug()<<"xmlpath is empty"+xmlPath;
        }

        MidiControllerPreset controllerPreset;
        controllerPreset.setPid(pid);
        controllerPreset.setAuthor(author);
        if (url.contains("forums")) {
        	controllerPreset.setForumLink(url);
            controllerPreset.setWikiLink("");
        } else if (url.contains("wiki")) {
        	controllerPreset.setForumLink("");
            controllerPreset.setWikiLink(url);
        } else {
        	controllerPreset.setForumLink("");
            controllerPreset.setWikiLink("");
        }
        controllerPreset.setDescription(description);
        controllerPreset.setPresetSource(presetSource);
        controllerPreset.setPresetStatus(presetStatus);
        controllerPreset.setMixxxVersion(mixxxVersion);
        controllerPreset.setName(presetName);
        controllerPreset.setDeviceId(controllerName);
        controllerPreset.setSchemaVersion(schemaVersion);
        // currently server only return one picture file or none
        if (!picturePath.isEmpty()) {
        	controllerPreset.addPictureFile(QFileInfo(picturePath).fileName());
        }
        if (!jsPath.isEmpty()) {
            controllerPreset.addScriptFile(QFileInfo(jsPath).fileName(),"");
        }
        controllerPreset.setRatings(avgRating);
        controllerPreset.setFilePath(xmlPath);

        presetList.append(controllerPreset);
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
