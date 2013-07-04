#ifndef PRESETOBJECTWAO_H
#define PRESETOBJECTWAO_H

#include <QVariantMap>
#include <QVariantList>
#include <QMap>


#include "controllers/presetobject.h"
class PresetObjectWAO {
  public:
	PresetObjectWAO();
	virtual ~PresetObjectWAO(){}
	QList<PresetObject> getPresetByURL(QString url);
	QList<PresetObject> getPresetByName(QString name);
	QList<PresetObject> getPresetByCompanyName(QString name);
	QList<PresetObject> getPresetByPresetName(QString name);
};
#endif
