#ifndef CONTROLGROUP_H
#define CONTROLGROUP_H

#include <QtCore>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QLinkedList>

#include "lights/colorgenerator.h"
#include "lights/controlmode.h"
#include "lights/triggermode.h"
#include "lights/light.h"

class FeatureState;

class ControlGroup : public QObject {
    Q_OBJECT
  public:
    ControlGroup(QObject* pParent, QString name);
    virtual ~ControlGroup();

    QString getName() {
        QMutexLocker locker(&m_mutex);
        return m_name;
    }
    void setName(QString name) {
        QMutexLocker locker(&m_mutex);
        m_name = name;
    }

    void setControlMode(ControlMode controlMode);
    ControlMode getControlMode();

    void setTriggerMode(TriggerMode triggerMode);
    TriggerMode getTriggerMode();

    void setColorGenerator(ColorGenerator* pGenerator);
    ColorGenerator* getColorGenerator();

    void addLight(Light* pLight);
    bool removeLight(Light* pLight);

    int numLights() {
        QMutexLocker locker(&m_mutex);
        return m_lights.size();
    }

    void process(FeatureState* pFrame);

  signals:
    void attributeChanged();

  private:
    void trigger(FeatureState* pState);
    void update_cycle(FeatureState* pState);
    void update_chaser(FeatureState* pState);
    void update_shifter(FeatureState* pState);

    QMutex m_mutex;
    QString m_name;
    QList<Light*> m_lights;

    ControlMode m_controlMode;
    TriggerMode m_triggerMode;

    ColorGenerator* m_pColorGenerator;

    QLinkedList<QColor> m_shifterQueue;
};

#endif /* CONTROLGROUP_H */
