
#include "lights/controlgroup.h"

ControlGroup::ControlGroup(QObject* pParent, QString name)
        : QObject(pParent),
          m_name(name) {
}

ControlGroup::~ControlGroup() {

}

void ControlGroup::setControlMode(ControlMode controlMode) {
    QMutexLocker locker(&m_mutex);
    m_controlMode = controlMode;
}

ControlMode ControlGroup::getControlMode() {
    QMutexLocker locker(&m_mutex);
    return m_controlMode;
}

void ControlGroup::setTriggerMode(TriggerMode triggerMode) {
    QMutexLocker locker(&m_mutex);
    m_triggerMode = triggerMode;
}

TriggerMode ControlGroup::getTriggerMode() {
    QMutexLocker locker(&m_mutex);
    return m_triggerMode;
}

void ControlGroup::setColorGenerator(ColorGenerator* pGenerator) {
    QMutexLocker locker(&m_mutex);
    m_pColorGenerator = pGenerator;
}

ColorGenerator* ControlGroup::getColorGenerator() {
    QMutexLocker locker(&m_mutex);
    return m_pColorGenerator;
}

void ControlGroup::update_cycle(FeatureState* pState) {
    QColor nextColor = m_pColorGenerator->nextColor();

    foreach (Light* pLight, m_lights) {
        switch (m_controlMode) {
            case CONTROL_CYCLE_SET:
                pLight->setColor(nextColor);
                break;
            case CONTROL_CYCLE_FADE:
                pLight->fadeTo(nextColor, 50);
                break;
            case CONTROL_CYCLE_FLASH:
                pLight->setColor(nextColor);
                pLight->fadeDown(100);
                break;
            default:
                qDebug() << "update_cycle called while not in a cycle!";
                break;
        }
    }
}

void ControlGroup::update_chaser(FeatureState* pState) {
    // TODO(XXX) um, this
}

void ControlGroup::addLight(Light* pLight) {
    QMutexLocker locker(&m_mutex);
    m_lights.append(pLight);
}

bool ControlGroup::removeLight(Light* pLight) {
    QMutexLocker locker(&m_mutex);
    int removed = m_lights.removeAll(pLight);
    Q_ASSERT(removed <= 1);
    return removed == 1;
}

void ControlGroup::trigger(FeatureState* pState) {
    switch (m_controlMode) {
        case CONTROL_CHASER:
            update_chaser(pState);
            break;
        case CONTROL_CYCLE_SET:
        case CONTROL_CYCLE_FADE:
        case CONTROL_CYCLE_FLASH:
            update_cycle(pState);
            break;
        case CONTROL_TWINKLE:
        case CONTROL_GLOW:
        case CONTROL_FLASH:
        case CONTROL_FLASH_WHITE:
            break;
    }
}

void ControlGroup::process(FeatureState* pState) {
    switch (m_triggerMode) {
        case CONTINUOUS:
            trigger(pState);
            break;
        case BEAT:
            if (pState->is_beat && !pState->is_silence) {
                trigger(pState);
            }
            break;
        case ONSET:
            if (pState->is_onset && !pState->is_silence) {
                trigger(pState);
            }
            break;
        case PITCH:
        case FFTBIN:
        case TIMER:
        case BEAT_DIV_4:
            // ????
            break;
    }

    // Animate all our light, regardless of we triggered a state update. They
    // may need to complete a fade or something
    foreach (Light* pLight, m_lights) {
        pLight->animate();
    }
}

QString triggerModeToString(TriggerMode mode) {
    switch(mode) {
        case CONTINUOUS:
            return QObject::tr("Continuous");
            break;
        case BEAT:
            return QObject::tr("Beat");
            break;
        case ONSET:
            return QObject::tr("Onset");
            break;
        case PITCH:
            return QObject::tr("Pitch");
            break;
        case FFTBIN:
            return QObject::tr("FFTBIN");
            break;
        case TIMER:
            return QObject::tr("Timer");
            break;
        case BEAT_DIV_4:
            return QObject::tr("Beat/4");
            break;
    }
    return QObject::tr("Unknown");
}

QString controlModeToString(ControlMode mode) {
    switch(mode) {
        case CONTROL_CHASER:
            return QObject::tr("Chaser");
            break;
        case CONTROL_CYCLE_SET:
            return QObject::tr("Cycle-Set");
            break;

        case CONTROL_CYCLE_FADE:
            return QObject::tr("Cycle-Fade");
            break;

        case CONTROL_CYCLE_FLASH:
            return QObject::tr("Cycle-Flash");
            break;

        case CONTROL_TWINKLE:
            return QObject::tr("Twinkle");
            break;
        case CONTROL_GLOW:
            return QObject::tr("Glow");
            break;
        case CONTROL_FLASH:
            return QObject::tr("Flash");
            break;
        case CONTROL_FLASH_WHITE:
            return QObject::tr("Flash-White");
            break;
    }
    return QObject::tr("Unknown");
}
