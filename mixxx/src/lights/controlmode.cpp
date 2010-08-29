
#include "lights/controlmode.h"

QString controlModeToString(ControlMode mode) {
    switch(mode) {
        case CONTROL_OFF:
            return QObject::tr("Off");
        case CONTROL_CHASER:
            return QObject::tr("Chaser");
        case CONTROL_SHIFTER_RIGHT:
            return QObject::tr("ShiftRight");
        case CONTROL_SHIFTER_LEFT:
            return QObject::tr("ShiftLeft");
        case CONTROL_MIRROR_INWARD:
            return QObject::tr("MirrorIn");
        case CONTROL_MIRROR_OUTWARD:
            return QObject::tr("MirrorOut");
        case CONTROL_CYCLE:
            return QObject::tr("Cycle");
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
    options.append(controlModeToString(CONTROL_SHIFTER_RIGHT));
    options.append(controlModeToString(CONTROL_SHIFTER_LEFT));
    options.append(controlModeToString(CONTROL_MIRROR_INWARD));
    options.append(controlModeToString(CONTROL_MIRROR_OUTWARD));
    options.append(controlModeToString(CONTROL_CYCLE));
    options.append(controlModeToString(CONTROL_TWINKLE));
    options.append(controlModeToString(CONTROL_GLOW));
    options.append(controlModeToString(CONTROL_FLASH));
    options.append(controlModeToString(CONTROL_FLASH_WHITE));
    return options;
}


