#ifndef PRESETOBJECTDAO_H
#define PRESETOBJECTDAO_H
#include <QObject>
#include <QSqlDatabase>
#include "controllers/midi/midicontrollerpreset.h"
#define PRESETOBJECT_TABLE "mapping_preset_object"
class PresetObjectDAO: public QObject{
	Q_OBJECT
  public:
    PresetObjectDAO(QSqlDatabase& database);
    virtual ~ PresetObjectDAO(){}
	QList<MidiControllerPreset> getPresetByPresetName(QString name);
  private:
    QSqlDatabase& m_database;
    //DISALLOW_COPY_AND_ASSIGN(PresetObjectDAO);
};
#endif
