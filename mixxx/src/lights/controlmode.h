#ifndef CONTROLMODE_H
#define CONTROLMODE_H

#include <QString>
#include <QList>
#include <QObject>

enum ControlMode {
    CONTROL_OFF = 0,
    CONTROL_CHASER,
    CONTROL_CYCLE_SET,
    CONTROL_CYCLE_FADE,
    CONTROL_CYCLE_FLASH,
    CONTROL_TWINKLE,
    CONTROL_GLOW,
    CONTROL_FLASH,
    CONTROL_FLASH_WHITE
};

QString controlModeToString(ControlMode mode);
QList<QString> getControlModeOptions();

#endif /* CONTROLMODE_H */
