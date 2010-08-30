
#include "lights/triggermode.h"

QString triggerModeToString(TriggerMode mode) {
    switch(mode) {
        case CONTINUOUS:
            return QObject::tr("Continuous");
        case BEAT:
            return QObject::tr("Beat");
        case ONSET:
            return QObject::tr("Onset");
        case PITCH:
            return QObject::tr("Pitch");
        case FFTBIN:
            return QObject::tr("FFTBIN");
        case TIMER:
            return QObject::tr("Timer");
        case BEAT_DIV2:
            return QObject::tr("Beat/2");
        case BEAT_DIV4:
            return QObject::tr("Beat/4");
        case BEAT_DIV8:
            return QObject::tr("Beat/8");
        case BEAT_X2:
            return QObject::tr("Beat x2");
    }
    return QObject::tr("Unknown");
}

QList<QString> getTriggerModeOptions() {
    QList<QString> options;
    options.append(triggerModeToString(CONTINUOUS));
    options.append(triggerModeToString(BEAT));
    options.append(triggerModeToString(ONSET));
    options.append(triggerModeToString(PITCH));
    options.append(triggerModeToString(FFTBIN));
    options.append(triggerModeToString(TIMER));
    options.append(triggerModeToString(BEAT_DIV2));
    options.append(triggerModeToString(BEAT_DIV4));
    options.append(triggerModeToString(BEAT_DIV8));
    options.append(triggerModeToString(BEAT_X2));
    return options;
}
