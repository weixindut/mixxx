#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QColor>
#include <QMutex>

#include "lights/lightstate.h"
#include "lights/tweener.h"

class ControlGroup;

class Light : public QObject {
    Q_OBJECT
  public:
    Light(QObject* pParent, QString id);
    virtual ~Light();

    virtual QString getName();
    virtual void setName(QString name);

    QString getId();

    void setColor(const QColor& color);
    QColor getColor();
    LightState getState();
    ControlGroup* getControlGroup();
    void setControlGroup(ControlGroup* pGroup);

    virtual bool isSegmented() {
        return false;
    }

    void animate();
    void fadeDown(int steps);
    void fadeTo(const QColor& targetColor, int steps);
    void fadeToHue(qreal hue, int steps);
    void fadeToValue(qreal value, int steps);

  signals:
    void updated();

  private:
    void setState(LightState state);

    QMutex m_mutex;
    QString m_name, m_id;
    LightState m_state;
    QColor m_color;
    ControlGroup* m_pControlGroup;

    // Fade parameters
    qreal m_target_hue, m_target_sat, m_target_val;
    qreal m_hue_parameter, m_sat_parameter, m_val_parameter;
    qreal m_hue_stepsize, m_sat_stepsize, m_val_stepsize;

    // Tweeners for fading and transitions
    Tweener* m_hueTweener;
    Tweener* m_satTweener;
    Tweener* m_valTweener;
};


#endif /* LIGHT_H */
