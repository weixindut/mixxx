
#include "lights/controlgroup.h"

#include "lights/featurestate.h"

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

}

void ControlGroup::update_shifter(FeatureState* pState) {
    QColor nextColor = m_pColorGenerator->nextColor();
    m_shifterQueue.push_front(nextColor);

    // Keep only the last N colors where N is the number of lights
    while (m_shifterQueue.size() > m_lights.size()) {
        m_shifterQueue.pop_back();
    }

    int i = 0;
    foreach(QColor color, m_shifterQueue) {
        if (i >= m_lights.size()) {
            break;
        }
        Light* pLight = m_lights[i];
        pLight->setColor(color);
        i++;
    }
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
        case CONTROL_OFF:
            foreach(Light* pLight, m_lights) {
                pLight->fadeDown(20);
            }
            break;
        case CONTROL_CHASER:
            update_chaser(pState);
            break;
        case CONTROL_SHIFTER:
            update_shifter(pState);
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



