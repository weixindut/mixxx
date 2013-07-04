#include "controllers/dao/presetobjectdao.h"
#include <QtDebug>
#include <QtCore>
#include <QtSql>
PresetObjectDAO::PresetObjectDAO(QSqlDatabase& database)
        : m_database(database) {

}
QList<PresetObject> PresetObjectDAO::getPresetByName(QString name) {
    QList<PresetObject> presetList;
    ScopedTransaction transaction(m_database);
    QSqlQuery query(m_database);
    QString queryStr = "SELECT * FROM Mapping_preset_object where preset_name like %"+name+"%";
    query.prepare(queryStr);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return -1;
    }
    while (query.next()) {

        QString pid = query.value(query.record().indexOf("pid")).toString();
        QString author_name = query.value(query.record().indexOf("author")).toString();
        QUrl url = query.value(query.record().indexOf("url")).toUrl();
        QString description = query.value(query.record().indexOf("description")).toString();
        QString preset_source = query.value(query.record().indexOf("preset_source")).toString();
        QString preset_status = query.value(query.record().indexOf("preset_status")).toString();
        QString mixxx_version = query.value(query.record().indexOf("mixxx_version")).toString();
        QString preset_name = query.value(query.record().indexOf("preset_name")).toString();
        QString midi_controller_name = query.value(query.record().indexOf("midicontroller_name")).toString();
        QString company_name = query.value(query.record().indexOf("company_name")).toString();
        QDateTime datetime = query.value(query.record().indexOf("datetime_added")).toDateTime();
        presetList.append(PresetObject(pid,author_name,url,
                description, preset_source, preset_status,
                mixxx_version, preset_name,midi_controller_name,
                company_name,datetime));
    }
	transaction.commit();
	return presetList;
}
QList<PresetObject> PresetObjectDAO::getPresetByCompanyName(QString name) {
    QList<PresetObject> presetList;
    ScopedTransaction transaction(m_database);
    QSqlQuery query(m_database);
    QString queryStr = "SELECT * FROM Mapping_preset_object where company_name like %"+name+"%";
    query.prepare(queryStr);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return -1;
    }
    while (query.next()) {

        QString pid = query.value(query.record().indexOf("pid")).toString();
        QString author_name = query.value(query.record().indexOf("author")).toString();
        QUrl url = query.value(query.record().indexOf("url")).toUrl();
        QString description = query.value(query.record().indexOf("description")).toString();
        QString preset_source = query.value(query.record().indexOf("preset_source")).toString();
        QString preset_status = query.value(query.record().indexOf("preset_status")).toString();
        QString mixxx_version = query.value(query.record().indexOf("mixxx_version")).toString();
        QString preset_name = query.value(query.record().indexOf("preset_name")).toString();
        QString midi_controller_name = query.value(query.record().indexOf("midicontroller_name")).toString();
        QString company_name = query.value(query.record().indexOf("company_name")).toString();
        QDateTime datetime = query.value(query.record().indexOf("datetime_added")).toDateTime();
        presetList.append(PresetObject(pid,author_name,url,
                description, preset_source, preset_status,
                mixxx_version, preset_name,midi_controller_name,
                company_name,datetime));
    }
	transaction.commit();
	return presetList;
}

QList<PresetObject> PresetObjectDAO::getPresetByPresetName(QString name) {
    QList<PresetObject> presetList;
    ScopedTransaction transaction(m_database);
    QSqlQuery query(m_database);
    QString queryStr = "SELECT * FROM Mapping_preset_object where midicontroller_name like %"+name+"%";
    query.prepare(queryStr);
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return -1;
    }
    while (query.next()) {

        QString pid = query.value(query.record().indexOf("pid")).toString();
        QString author_name = query.value(query.record().indexOf("author")).toString();
        QUrl url = query.value(query.record().indexOf("url")).toUrl();
        QString description = query.value(query.record().indexOf("description")).toString();
        QString preset_source = query.value(query.record().indexOf("preset_source")).toString();
        QString preset_status = query.value(query.record().indexOf("preset_status")).toString();
        QString mixxx_version = query.value(query.record().indexOf("mixxx_version")).toString();
        QString preset_name = query.value(query.record().indexOf("preset_name")).toString();
        QString midi_controller_name = query.value(query.record().indexOf("midicontroller_name")).toString();
        QString company_name = query.value(query.record().indexOf("company_name")).toString();
        QDateTime datetime = query.value(query.record().indexOf("datetime_added")).toDateTime();
        presetList.append(PresetObject(pid,author_name,url,
                description, preset_source, preset_status,
                mixxx_version, preset_name,midi_controller_name,
                company_name,datetime));
    }
	transaction.commit();
	return presetList;
}
