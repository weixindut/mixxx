#include <QtDebug>
#include <QtCore>
#include <QtSql>
#include "controllers/dao/presetobjectdao.h"
#include "library/queryutil.h"
PresetObjectDAO::PresetObjectDAO(QSqlDatabase& database)
        : m_database(database) {

}

QList<MidiControllerPreset> PresetObjectDAO::getPresetByPresetName(QString name) {
    ScopedTransaction transaction(m_database);
    QList<MidiControllerPreset> presetList;

    QSqlQuery query(m_database);
    QString queryStr = "SELECT * FROM Mapping_preset_object WHERE preset_name = '"+name+"'";
    query.prepare(queryStr);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return QList<MidiControllerPreset>();
    }
    while (query.next()) {

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
        QString queryStr = "SELECT directory FROM Files_storage where presetitem_id = '"+pid+"' AND type = 0";
        picQuery.prepare(queryStr);
        if (!picQuery.exec()) {
            LOG_FAILED_QUERY(picQuery);
            return QList<MidiControllerPreset>();
        }
        QString picPath;
        if(picQuery.next()) {
        	picPath = picQuery.value(query.record().indexOf("directory")).toString();
        } else {
        	qDebug()<<"there is not cover picture\n";
        	picPath ="";
        }
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
        controllerpreset.setPicturePath(picPath);
        controllerpreset.setRatings(ratings);
        controllerpreset.setFilePath("");

        presetList.append(controllerpreset);
    }
    transaction.commit();
    return presetList;
}
