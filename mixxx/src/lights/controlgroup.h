#ifndef CONTROLGROUP_H
#define CONTROLGROUP_H

#include <QtCore>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>

#include "lights/colorgenerator.h"
#include "lights/controlmode.h"
#include "lights/triggermode.h"
#include "lights/featurestate.h"
#include "lights/light.h"

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

    int numLights() {
        QMutexLocker locker(&m_mutex);
        return m_lights.size();
    }

    void process(FeatureState* pFrame);

  signals:
    void attributeChanged();

  private:
    QMutex m_mutex;
    QString m_name;
    QList<Light*> m_lights;

    ControlMode m_controlMode;
    TriggerMode m_triggerMode;

    ColorGenerator* m_pColorGenerator;
};

#endif /* CONTROLGROUP_H */
