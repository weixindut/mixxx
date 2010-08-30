#ifndef TRANSITIONMODE_H
#define TRANSITIONMODE_H

#include <QObject>
#include <QString>
#include <QList>

// WARNING: If you add a new mode you must update transitionmode.cpp to provide
// a string version of the mode and add it to the available mode list.

enum TransitionMode {
    TRANSITION_SET,
    TRANSITION_FADE_10,
    TRANSITION_FADE_50,
    TRANSITION_FADE_100,
    TRANSITION_FLASH_100,
    TRANSITION_FADEUP_50,
    TRANSITION_FADEUP_100,
    TRANSITION_WHITEFLASH_100
};

QString transitionModeToString(TransitionMode mode);
QList<QString> getTransitionModeOptions();

#endif /* TRANSITIONMODE_H */
