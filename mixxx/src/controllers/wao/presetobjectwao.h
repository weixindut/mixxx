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
	QList<PresetObject> getPresetByName(QString name);
	void getPresetByCompanyName(QString name);
};
#endif
