#ifndef PRESETOBJECTWAO_H
#define PRESETOBJECTWAO_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QMap>


#include "controllers/midi/midicontrollerpreset.h"
class PresetObjectWAO : public QObject{
    Q_OBJECT
  public:
    PresetObjectWAO();
    virtual ~PresetObjectWAO(){}
    QList<MidiControllerPreset> getPresetByURL(QString url);
    QList<MidiControllerPreset> getPresetByPresetName(QString name);
};
#endif
