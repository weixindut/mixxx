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
    QList<MidiControllerPreset> getPresetByURL(QString destDirecotry, QString url);
    QList<MidiControllerPreset> getPresetByPresetName(QString destDirecotry, QString name);
    QList<MidiControllerPreset> getPresetByPresetID(QString destDirecotry, QString pid);
    QList<MidiControllerPreset> checkForUpdate(QString presetname, QString controller);
    bool checkAvailableFileName(QString filename);

  private:
    QString generateQueryStr(QString name);
};
#endif
