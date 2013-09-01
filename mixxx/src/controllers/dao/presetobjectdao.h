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
    virtual ~PresetObjectDAO() {}
    QList<MidiControllerPreset> getPresetByPresetName(QString name);
    // insert preset data into table mapping_preset_object
    bool insertOnePresetRecord(QString pid, QString xmlFilePath);
    // insert info of a file into table files_storage
    bool insertOneFile(QString pid,QString filePath, int type);
    // check whether preset with same controller name, preset name and
    // schema version before one preset is inserted
    bool isPresetInsertable(QString xmlFilePath);
    // insert preset data and relative file data into database
    bool insertOnePreset(QString pid,QString xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles);
  private:
    QString generateQueryStr(QString name);

    QSqlDatabase& m_database;
};
#endif
