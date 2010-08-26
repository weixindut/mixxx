#ifndef TRIGGERMODE_H
#define TRIGGERMODE_H

#include <QObject>
#include <QString>
#include <QList>

enum TriggerMode {
    CONTINUOUS = 0,
    BEAT,
    ONSET,
    PITCH,
    FFTBIN,
    TIMER,
    BEAT_DIV_4
};

QString triggerModeToString(TriggerMode mode);
QList<QString> getTriggerModeOptions();

#endif /* TRIGGERMODE_H */
