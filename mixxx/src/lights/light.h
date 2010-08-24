

#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QColor>

#include "lights/lightstate.h"
#include "lights/tweener.h"

class Light : public QObject {

    Q_OBJECT
  public:
    Light(QObject* pParent = NULL);
    virtual ~Light();

    void setColor(const QColor& color);
    QColor getColor();

    virtual bool isSegmented() {
        return false;
    }

    void animate();
    void fadeDown();
    void fadeTo(const QColor& targetColor);
    void fadeToHue(qreal hue);

  private:
    void setState(LightState state);

    LightState m_state;
    QColor m_color;
    QColor m_animation_arg1;
    qreal m_target_hue, m_target_sat, m_target_val;

    Tweener* m_hueTweener;
    Tweener* m_satTweener;
    Tweener* m_valTweener;
};


#endif /* LIGHT_H */
