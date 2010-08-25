
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

void ControlGroup::process(FeatureState* pFrame) {


}
