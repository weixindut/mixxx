#include <QtDebug>
#include <QtCore>
#include <QtSql>
#include <QFileInfo>

#include "controllers/dao/presetobjectdao.h"
#include "controllers/controllerpresetinfo.h"
#include "library/queryutil.h"
#include "xmlparse.h"
PresetObjectDAO::PresetObjectDAO(QSqlDatabase& database)
        : m_database(database) {

}

QList<MidiControllerPreset> PresetObjectDAO::getPresetByPresetName(QString name) {
    QList<MidiControllerPreset> presetList;
    QSqlQuery query(m_database);
    QString queryStr = generateQueryStr(name);
    query.prepare(queryStr);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return QList<MidiControllerPreset>();
    }
    while (query.next()) {
    	MidiControllerPreset controllerpreset;
        QString pid = query.value(query.record().indexOf("pid")).toString();
        QString author = query.value(query.record().indexOf("author")).toString();
        QString url = query.value(query.record().indexOf("url")).toString();
        QString description = query.value(query.record().indexOf("description")).toString();
        QString preset_source = query.value(query.record().indexOf("preset_source")).toString();
        QString preset_status = query.value(query.record().indexOf("preset_status")).toString();
        QString mixxx_version = query.value(query.record().indexOf("mixxx_version")).toString();
        QString preset_name = query.value(query.record().indexOf("preset_name")).toString();
        QString controller_name = query.value(query.record().indexOf("controller_name")).toString();
        QString schema_version = query.value(query.record().indexOf("schema_version")).toString();
        float ratings = query.value(query.record().indexOf("ratings")).toFloat();
        QSqlQuery picQuery(m_database);
        QString picStr = "SELECT name FROM Files_storage WHERE presetitem_id = '"+pid+"' AND type = 0";
        picQuery.prepare(picStr);
        if (!picQuery.exec()) {
            LOG_FAILED_QUERY(picQuery);
            return QList<MidiControllerPreset>();
        }
        QString picName;
        while(picQuery.next()) {
        	picName = picQuery.value(picQuery.record().indexOf("name")).toString();
        	controllerpreset.addPictureFile(picName);
        }

        QSqlQuery jsQuery(m_database);
        QString jsStr = "SELECT name FROM Files_storage WHERE presetitem_id = '"+pid+"' AND type = 2";
        jsQuery.prepare(jsStr);
        if (!jsQuery.exec()) {
            LOG_FAILED_QUERY(jsQuery);
            return QList<MidiControllerPreset>();
        }
        QString jsName;
        while(jsQuery.next()) {
        	jsName = jsQuery.value(jsQuery.record().indexOf("name")).toString();
            controllerpreset.addScriptFile(jsName,"");
        }

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
        controllerpreset.addPictureFile(picName);
        controllerpreset.setRatings(ratings);
        QSqlQuery xmlQuery(m_database);
        QString xmlStr = "SELECT directory,name FROM Files_storage WHERE presetitem_id = '"+pid+"' AND type = 1";
        xmlQuery.prepare(xmlStr);
        if (!xmlQuery.exec()) {
            LOG_FAILED_QUERY(xmlQuery);
            return QList<MidiControllerPreset>();
        }

        if(xmlQuery.next()) {
        	QString directory = xmlQuery.value(xmlQuery.record().indexOf("directory")).toString();
        	QString name = xmlQuery.value(xmlQuery.record().indexOf("name")).toString();
            controllerpreset.setFilePath(directory+"/"+name);
            presetList.append(controllerpreset);
        } else {
            qDebug()<< "Database is not complete.";
        }

    }
    return presetList;
}

QString PresetObjectDAO::generateQueryStr(QString name) {

    name.replace(" ",",");
    name.replace("-",",");
    name.replace("_",",");
    name.replace(".",",");

    QStringList words = name.split(",",QString::SkipEmptyParts);

    QString queryStr = "SELECT * FROM Mapping_preset_object ";
    for(int i=0; i<words.size(); i++) {
        if(i==0) {
            queryStr.append(" WHERE LOWER(preset_name) LIKE '%");
            queryStr.append(words[i].toLower());
            queryStr.append("%' ");
        } else {
            queryStr.append(" OR LOWER(preset_name) LIKE '%");
            queryStr.append(words[i].toLower());
            queryStr.append("%' ");
    	}
    }
    return queryStr;
}
bool PresetObjectDAO::isPresetInsertable(QString xmlFilePath) {
	PresetInfo presetInfo = PresetInfo(xmlFilePath);
	QString controllerName = presetInfo.getControllerName();
	QString presetName = presetInfo.getName();
	QString schemaVersion = presetInfo.getSchemaVersion();

    QSqlQuery query(m_database);
    query.prepare(" SELECT controller_name, preset_name, schema_version FROM mapping_preset_object "
    		      " WHERE controller_name = :controller_name AND preset_name = :preset_name AND "
    		      " schema_version = :schema_version");
    query.bindValue(":controller_name", controllerName);
	query.bindValue(":preset_name", presetName);
    query.bindValue(":schema_version", schemaVersion);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return false;
    }
    if(query.next()) {
        qDebug() << "Preset with controller_name:"+controllerName+", preset_name:"+presetName
                +", schema_version:"+schemaVersion+" has already existed in local database.";
        return false;
    } else {
        return true;
    }
}
bool PresetObjectDAO::insertOnePresetRecord(QString pid, QString xmlFilePath, QString status) {
	PresetInfo presetInfo = PresetInfo(xmlFilePath);
	QString controllerName = presetInfo.getControllerName();
	QString presetName = presetInfo.getName();
	QString schemaVersion = presetInfo.getSchemaVersion();
	QString author = presetInfo.getAuthor();
	QString description = presetInfo.getDescription();
	QString mixxxVersion = presetInfo.getMixxxVersion();
	QString presetStatus = status;
	float ratings = 0;
	QString url;
	QString presetSource;
	if(!presetInfo.getForumLink().isEmpty()) {
	    url = presetInfo.getForumLink();
	    presetSource = "forum";
	} else if(!presetInfo.getWikiLink().isEmpty()) {
	    url = presetInfo.getWikiLink();
	    presetSource = "wiki";
	} else {
	    url = "www.mixxx.org";
	    presetSource = "mixxx";
	}
	QSqlQuery query(m_database);
	query.prepare("INSERT INTO mapping_preset_object (pid,author, url, description,"
	               "preset_source, preset_status, mixxx_version, preset_name, ratings,"
	               "controller_name, schema_version) VALUES (:pid, :author, :url, "
	               ":description, :preset_source, :preset_status, :mixxx_version, "
	               ":preset_name, :ratings, :controller_name, :schema_version)");
	query.bindValue(":pid", pid);
	query.bindValue(":author", author);
	query.bindValue(":url", url);
	query.bindValue(":description", description);
	query.bindValue(":preset_source", presetSource);
	query.bindValue(":preset_status", presetStatus);
	query.bindValue(":mixxx_version", mixxxVersion);
	query.bindValue(":preset_name", presetName);
	query.bindValue(":ratings", ratings);
	query.bindValue(":controller_name", controllerName);
	query.bindValue(":schema_version", schemaVersion);
	if (!query.exec()) {
	    LOG_FAILED_QUERY(query);
	    return false;
	}
	return true;
}
bool PresetObjectDAO::insertOneFile(QString pid,QString filePath, int type) {
    // TODO(weixin):give a unified definition of type, currently type:0-pic,1-xml,2-js
    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists()) {
        qDebug() << filePath + "does not exist!";
        return false;
    }
    QString directory = fileInfo.path();
    QString name = fileInfo.fileName();
    // fileInfo.size() return int64;
    int size = fileInfo.size();
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO files_storage (directory,presetitem_id,type,filesize,name) "
                  "VALUES (:directory, :presetitem_id, :type, :filesize, :name)");
    query.bindValue(":directory", directory);
    query.bindValue(":presetitem_id", pid);
    query.bindValue(":type", type);
    query.bindValue(":filesize", size);
    query.bindValue(":name", name);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return false;
    }
    return true;
}
bool PresetObjectDAO::insertOnePreset(QString pid,QString xmlFile,
        QList<QString>& picFiles, QList<QString>& jsFiles, QString status) {
	ScopedTransaction transaction(m_database);
    if (!insertOnePresetRecord(pid,xmlFile,status)) {
    	transaction.rollback();
        return false;
    }
    foreach(QString file, picFiles) {
    	qDebug()<<"insert pictures^^^^^^^^^^^^^^^^^^";
        if(!insertOneFile(pid,file,0)) {
        	transaction.rollback();
            return false;
        }
    }
    foreach(QString file, jsFiles) {
        if(!insertOneFile(pid,file,2)) {
        	transaction.rollback();
            return false;
        }
    }
    if (!insertOneFile(pid,xmlFile,1)) {
    	transaction.rollback();
        return false;
    }
    transaction.commit();
    return true;
}
void PresetObjectDAO::initialize(QString mapFile, QString directory) {
    QDomElement root = XmlParse::openXMLFile(mapFile, "presets");
    if (root.isNull()) {
        qDebug() << "ERROR parsing" << mapFile;
        return;
    }
    QDomNodeList presetList = root.childNodes();
    for(int i=0; i<presetList.count();i++) {
        QDomNode preset = presetList.at(i);
        QString filename = preset.toElement().attribute("filename");
        QString pid = preset.toElement().attribute("pid");
        QString status = preset.toElement().attribute("status");
        if(!doesPresetExist(pid)) {
            QString xmlPath = directory + filename;
            QFile xmlFile(xmlPath);
            QList<QString> jsFiles;
            QList<QString> picFiles;
            if(xmlFile.exists()) {
                QDomElement xmlroot = XmlParse::openXMLFile(mapFile, "controller");
                QDomElement scripts = xmlroot.firstChildElement("scriptfiles");
                QDomNodeList scriptList = scripts.childNodes();
                for(int j=0; j<scriptList.count();j++) {
                    QDomNode script = scriptList.at(j);
                    QString scriptName = directory + script.toElement().attribute("filename");
                    jsFiles.append(scriptName);
                }
                QDomElement pictures = xmlroot.firstChildElement("picfiles");
                QDomNodeList picList = pictures.childNodes();
                for(int j=0; j<picList.count();j++) {
                    QDomNode pic = picList.at(j);
                    QString picName = directory + pic.toElement().attribute("name");
                    jsFiles.append(picName);
                }
            }
            insertOnePreset(pid,xmlPath,picFiles,jsFiles,status);
        }
    }
}
bool PresetObjectDAO::doesPresetExist(QString pid) {
    QSqlQuery query(m_database);
    query.prepare(" SELECT pid FROM mapping_preset_object WHERE pid = :pid");
    query.bindValue(":pid", pid);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return false;
    }
    if(query.next()) {
        return true;
    }
    return false;
}
