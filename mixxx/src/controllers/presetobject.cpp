#include "controllers/presetobject.h"
PresetObject::PresetObject(QString pid, QString author_name, QUrl url,
    		     QString description, QString preset_source, QString preset_status,
    		     QString mixxx_version, QString preset_name, QString midi_controller_name,
    		     QString company_name, QDateTime datetime)
        : m_pid(pid),m_author_name(author_name),
          m_url(url),m_description(description),
          m_preset_source(preset_source),
          m_preset_status(preset_status),
          m_mixxx_version(mixxx_version),
          m_preset_name(preset_name),
          m_midi_controller_id(midi_controller_id),
          m_company_name(company_name),
          m_datetime(datetime) {
}
QString PresetObject::getPid() {
    return m_pid;
}
QString PresetObject::getAuthorName() {
    return m_author_name;
}
QUrl PresetObject::getURL() {
    return m_url;
}
QString PresetObject::getDescription() {
    return m_description;
}
QString PresetObject::getPresetSource() {
    return m_preset_source;
}
QString PresetObject::getPresetStatus() {
    return m_preset_status;
}
QString PresetObject::getMixxxVersion() {
    return m_mixxx_version;
}
QString PresetObject::getPresetName() {
    return m_preset_name;
}
QString PresetObject::getMidiControllerID() {
    return m_midi_controller_name;
}
QString PresetObject::getCompanyName() {
    return m_company_name;
}
QDateTime PresetObject::getDateTime() {
	return m_datetime;
}
