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
    QString queryStr = "SELECT * FROM Mapping_preset_object where midicontroller_name like %"+name+"%";
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
	transaction.commit();
	return presetList;
}
