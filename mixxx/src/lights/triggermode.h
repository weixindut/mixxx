#ifndef TRIGGERMODE_H
#define TRIGGERMODE_H

#include <QObject>
#include <QString>

enum TriggerMode {
    CONTINUOUS = 0,
    BEAT,
    ONSET,
    PITCH,
    FFTBIN,
    TIMER,
    BEAT_DIV_4
};

#endif /* TRIGGERMODE_H */
