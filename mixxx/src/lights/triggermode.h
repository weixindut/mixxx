#ifndef TRIGGERMODE_H
#define TRIGGERMODE_H

#include <QObject>
#include <QString>
#include <QList>

// WARNING: If you add a new trigger mode you must update triggermode.cpp to
// provide a string version of the mode and add it to the available mode list.

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
