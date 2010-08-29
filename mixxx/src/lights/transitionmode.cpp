
#include "lights/transitionmode.h"

QString transitionModeToString(TransitionMode mode) {
    switch(mode) {
        case TRANSITION_SET:
            return QObject::tr("Set");
        case TRANSITION_FADE_10:
            return QObject::tr("Fade10");
        case TRANSITION_FADE_50:
            return QObject::tr("Fade50");
        case TRANSITION_FADE_100:
            return QObject::tr("Fade100");
        case TRANSITION_FLASH_100:
            return QObject::tr("Flash100");
        case TRANSITION_FADEUP_50:
            return QObject::tr("FadeUp50");
        case TRANSITION_FADEUP_100:
            return QObject::tr("FadeUp100");
    }
    return QObject::tr("Unknown");
}

QList<QString> getTransitionModeOptions() {
    QList<QString> options;
    options.append(transitionModeToString(TRANSITION_SET));
    options.append(transitionModeToString(TRANSITION_FADE_10));
    options.append(transitionModeToString(TRANSITION_FADE_50));
    options.append(transitionModeToString(TRANSITION_FADE_100));
    options.append(transitionModeToString(TRANSITION_FLASH_100));
    options.append(transitionModeToString(TRANSITION_FADEUP_50));
    options.append(transitionModeToString(TRANSITION_FADEUP_100));
    return options;
}

