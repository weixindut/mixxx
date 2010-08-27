
#include "lights/controlmode.h"

QString controlModeToString(ControlMode mode) {
    switch(mode) {
        case CONTROL_OFF:
            return QObject::tr("Off");
        case CONTROL_CHASER:
            return QObject::tr("Chaser");
        case CONTROL_SHIFTER:
            return QObject::tr("Shifter");
        case CONTROL_CYCLE_SET:
            return QObject::tr("Cycle-Set");
        case CONTROL_CYCLE_FADE:
            return QObject::tr("Cycle-Fade");
        case CONTROL_CYCLE_FLASH:
            return QObject::tr("Cycle-Flash");
        case CONTROL_TWINKLE:
            return QObject::tr("Twinkle");
        case CONTROL_GLOW:
            return QObject::tr("Glow");
        case CONTROL_FLASH:
            return QObject::tr("Flash");
        case CONTROL_FLASH_WHITE:
            return QObject::tr("Flash-White");
    }
    return QObject::tr("Unknown");
}

QList<QString> getControlModeOptions() {
    QList<QString> options;
    options.append(controlModeToString(CONTROL_OFF));
    options.append(controlModeToString(CONTROL_CHASER));
    options.append(controlModeToString(CONTROL_SHIFTER));
    options.append(controlModeToString(CONTROL_CYCLE_SET));
    options.append(controlModeToString(CONTROL_CYCLE_FADE));
    options.append(controlModeToString(CONTROL_CYCLE_FLASH));
    options.append(controlModeToString(CONTROL_TWINKLE));
    options.append(controlModeToString(CONTROL_GLOW));
    options.append(controlModeToString(CONTROL_FLASH));
    options.append(controlModeToString(CONTROL_FLASH_WHITE));
    return options;
}


