
#include "lights/controlgroup.h"

#include "lights/featurestate.h"
#include "lights/lightcontroller.h"

ControlGroup::ControlGroup(QObject* pParent, QString name)
        : QObject(pParent),
          m_name(name),
          m_controlMode(CONTROL_OFF),
          m_triggerMode(CONTINUOUS),
          m_pColorGenerator(NULL) {
}

ControlGroup::~ControlGroup() {

}

void ControlGroup::setControlMode(ControlMode controlMode) {
    QMutexLocker locker(&m_mutex);
    m_controlMode = controlMode;
    initialize_mode();
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

void ControlGroup::init_mirror() {
    m_mirrorQueue.clear();
}

void ControlGroup::update_mirror(FeatureState* pState) {
    QColor nextColor = m_pColorGenerator->nextColor();
    m_mirrorQueue.push_front(nextColor);

    int numLights = m_lights.size();
    bool odd = numLights % 2 != 0;

    // If it is odd, then there is a center node
    int mirrorLength = (numLights >> 1) + (odd ? 1 : 0);

    // Trim the mirror queue to the length of the mirror
    while (m_mirrorQueue.size() > mirrorLength) {
        m_mirrorQueue.pop_back();
    }

    // Now set the colors in the mirror queue depending on whether its an
    // outwards or inwards moving mirror. If its outwards moving then the queue
    // needs to be written from front to back starting in the center of the
    // lights. Otherwise it needs to start from back to front starting in the
    // center. We have to handle even or odd groups of lights.

    bool outward = m_controlMode == CONTROL_MIRROR_OUTWARD;
    int i, j;

    // for outward
    // 6 mirror i,j start at 2 and 3
    // 5 mirror i,j start at 2
    // 4 mirror i,j start at 1 and 2
    // 3 mirror i,j start at 1
    // 2 mirror i,j start at 0 and 1
    // 1 mirror i,j start at 0

    if (outward) {
        if (odd) {
            i = j = mirrorLength - 1;
        } else {
            i = mirrorLength - 1;
            j = mirrorLength;
        }
    } else {
        // It's really simple if it's inward.
        i = 0;
        j = numLights - 1;
    }

    foreach (QColor color, m_mirrorQueue) {
        // Check that i and j are in bounds. If i > j then we are doing an
        // inward mirror and we're done.
        if (i < 0 || i >= m_lights.size() ||
            j < 0 || j >= m_lights.size() ||
            i > j) {
            break;
        }

        // Set i's light color
        Light* pLight = m_lights[i];
        pLight->setColor(color);

        // If it's not redundant, set j's light color
        if (i != j) {
            pLight = m_lights[j];
            pLight->setColor(color);
        }

        if (outward) {
            i--; j++;
        } else {
            i++; j--;
        }
    }
}

void ControlGroup::init_shifter() {
    m_shifterQueue.clear();
}

void ControlGroup::update_shifter(FeatureState* pState) {
    QColor nextColor = m_pColorGenerator->nextColor();

    m_shifterQueue.push_front(nextColor);

    // Keep only the last N colors where N is the number of lights
    while (m_shifterQueue.size() > m_lights.size()) {
        m_shifterQueue.pop_back();
    }

    // If right-shifting, start at 0, else start at the end
    int i = (m_controlMode == CONTROL_SHIFTER_RIGHT) ? 0 : m_lights.size() - 1;
    int delta = (m_controlMode == CONTROL_SHIFTER_RIGHT) ? 1 : -1;

    foreach(QColor color, m_shifterQueue) {
        // Check for out of bounds
        if (i < 0 || i >= m_lights.size()) {
            break;
        }
        Light* pLight = m_lights[i];
        pLight->setColor(color);
        i = i + delta;
    }
}

void ControlGroup::addLight(Light* pLight) {
    QMutexLocker locker(&m_mutex);
    m_lights.append(pLight);
    pLight->setControlGroup(this);
}

bool ControlGroup::removeLight(Light* pLight) {
    QMutexLocker locker(&m_mutex);
    int removed = m_lights.removeAll(pLight);
    Q_ASSERT(removed <= 1);

    if (removed == 1) {
        pLight->setControlGroup(NULL);
    }
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
        case CONTROL_SHIFTER_RIGHT:
        case CONTROL_SHIFTER_LEFT:
            update_shifter(pState);
            break;
        case CONTROL_MIRROR_INWARD:
        case CONTROL_MIRROR_OUTWARD:
            update_mirror(pState);
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

void ControlGroup::initialize_mode() {
    switch (m_controlMode) {
        case CONTROL_SHIFTER_RIGHT:
        case CONTROL_SHIFTER_LEFT:
            init_shifter();
            break;
        case CONTROL_MIRROR_INWARD:
        case CONTROL_MIRROR_OUTWARD:
            init_mirror();
            break;
        default:
            // Do nothing
            break;
    }
}

void ControlGroup::process(FeatureState* pState) {
    switch (m_triggerMode) {
        case CONTINUOUS:
            trigger(pState);
            break;
        case BEAT:
            if (pState->is_fresh && pState->is_beat && !pState->is_silence) {
                trigger(pState);
            }
            break;
        case ONSET:
            if (pState->is_fresh && pState->is_onset && !pState->is_silence) {
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

// static
ControlGroup* ControlGroup::fromXml(LightController* pController, QDomNode node) {
    Q_ASSERT(node.nodeName() == "ControlGroup");

    QDomElement element = node.toElement();

    QString groupName = element.attribute("name", "");
    ControlGroup* pGroup = new ControlGroup(pController, groupName);

    QDomNodeList children = node.childNodes();

    for (int i = 0; i < children.count(); ++i) {
        QDomNode child = children.at(i);
        QString tagName = child.nodeName();

        if (tagName == "Lights") {
            QDomNodeList lights = child.childNodes();
            for (int j = 0; j < lights.count(); ++j) {
                QDomElement light = lights.at(j).toElement();

                QString lightId = light.attribute("id");
                Light* pLight = pController->getLightById(lightId);

                if (pLight != NULL) {
                    pGroup->addLight(pLight);
                }
            }
        } else if (tagName == "TriggerState") {

        } else if (tagName == "ControlMode") {

        }
    }
    return pGroup;
}
