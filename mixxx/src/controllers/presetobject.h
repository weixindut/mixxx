#ifndef PRESETOBJECT_H
#define PRESETOBJECT_H
#include <QUrl>
#include <QDateTime>
class PresetObject {
  public:
    PresetObject(QString pid, QString author_name, QUrl url,
    		     QString description, QString preset_source, QString preset_status,
    		     QString mixxx_version, QString preset_name, QString midi_controller_id,
    		     QDateTime datetime);
    virtual ~PresetObject(){}

    QString getPid();
    QString getAuthorName();
    QUrl getURL();
    QString getDescription();
    QString getPresetSource();
    QString getPresetStatus();
    QString getMixxxVersion();
    QString getPresetName();
    QString getMidiControllerID();
    QDateTime getDateTime();

  private:
    QString m_pid;
    QString m_author_name;
    QUrl m_url;
    QString m_description;
    QString m_preset_source;
    QString m_preset_status;
    QString m_mixxx_version;
    QString m_preset_name;
    QString m_midi_controller_id;
    QDateTime m_datetime;

};
#endif
