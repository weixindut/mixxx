#ifndef PRESETOBJECTDAO_H
#define PRESETOBJECTDAO_H
#include <QSqlDatabase>
#define PRESETOBJECT_TABLE "mapping_preset_object"
class PresetObjectDAO {
  public:
    PresetObjectDAO(QSqlDatabase& database);
    virtual ~ PresetObjectDAO(){}
	QList<PresetObject> getPresetByName(QString name);
	QList<PresetObject> getPresetByCompanyName(QString name);
	QList<PresetObject> getPresetByPresetName(QString name);
  private:
    QSqlDatabase& m_database;
    DISALLOW_COPY_AND_ASSIGN(PresetObjectDAO);
};
#endif
