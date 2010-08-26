
#ifndef LIGHTSTATE_H
#define LIGHTSTATE_H

#include <QString>
#include <QObject>

enum LightState {
    LIGHT_OFF = 0,
    LIGHT_ON,
    LIGHT_FADE,
    // Unused
    LIGHT_CYCLE,
    LIGHT_RANDOM,
    LIGHT_FFT
};

QString lightStateToString(LightState mode);

#endif /* LIGHTSTATE_H */
