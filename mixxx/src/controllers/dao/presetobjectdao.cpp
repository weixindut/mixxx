#include <QtDebug>
#include <QtCore>
#include <QtSql>
#include <QFileInfo>

#include "controllers/dao/presetobjectdao.h"
#include "controllers/controllerpresetinfo.h"
#include "library/queryutil.h"
PresetObjectDAO::PresetObjectDAO(QSqlDatabase& database)
        : m_database(database) {

}

QList<MidiControllerPreset> PresetObjectDAO::getPresetByPresetName(QString name) {
    ScopedTransaction transaction(m_database);
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
        QString queryStr = "SELECT name FROM Files_storage WHERE presetitem_id = '"+pid+"' AND type = 0";
        picQuery.prepare(queryStr);
        if (!picQuery.exec()) {
            LOG_FAILED_QUERY(picQuery);
            return QList<MidiControllerPreset>();
        }
        QString picName;
        for(picQuery.next()) {
        	picName = picQuery.value(picQuery.record().indexOf("name")).toString();
        	controllerpreset.addPictureFile(picName);
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
        controllerpreset.setPicturePath(picPath);
        controllerpreset.setRatings(ratings);
        controllerpreset.setFilePath("");
        // here JS Script files haven't been added, currently unnecessary
        presetList.append(controllerpreset);
    }
    transaction.commit();
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
bool PresetObjectDAO::insertOnePreset(QString pid, QString xmlFilePath) {
	PresetInfo presetInfo = PresetInfo(xmlFilePath);
	QString author = presetInfo.getAuthor();
	QString description = presetInfo.getDescription();
	QString presetName = presetInfo.getName();
	QString mixxxVersion = presetInfo.getMixxxVersion();
	QString schemaVersion = presetInfo.getSchemaVersion();
	QString controllerName = presetInfo.getControllerName();
	QString presetStatus = "undefined";
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
    query.prepare("INSERT INTO mapping_preset_object (pid,author,url,description,"
                  "preset_source,preset_status,mixxx_version,preset_name,ratings,"
                  "controller_name,schema_version) VALUES (:pid,:author,:url,"
                  ":description,:preset_source,:preset_status,mixxx_version,"
                  ":preset_name,:ratings,:controller_name,:schema_version)");
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
                  "VALUES (:directory,:presetitem_id,:type,:filesize,name)");
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
