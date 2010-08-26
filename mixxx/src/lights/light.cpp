#include <QtDebug>

#include "lights/light.h"

#include "mathstuff.h"

#include "lights/lineartweener.h"
#include "lights/cubiceaseouttweener.h"
#include "lights/cubiceaseintweener.h"

Light::Light(QObject* pParent)
        : m_hueTweener(new CubicEaseOutTweener()),
          m_satTweener(new CubicEaseOutTweener()),
          m_valTweener(new CubicEaseOutTweener())
          // m_hueTweener(new LinearTweener()),
          // m_satTweener(new LinearTweener()),
          // m_valTweener(new LinearTweener())

{

}

Light::~Light() {

}

void Light::setColor(const QColor& color) {
    m_color = color;
    setState(LIGHT_ON);
}

QColor Light::getColor() {
    return m_color;
}

void Light::fadeDown(int steps) {
    fadeToValue(0.0f, steps);
}

void Light::fadeTo(const QColor& targetColor, int steps) {
    targetColor.getHsvF(&m_target_hue, &m_target_sat, &m_target_val);

    double h, s, v;
    double th, ts, tv;
    double dh, ds, dv;
    m_color.getHsvF(&h, &s, &v);
    th = m_target_hue;
    tv = m_target_val;
    ts = m_target_sat;

    dh = (th - h);
    ds = (ts - s);
    dv = (tv - v);

    // The parameters all range from 0.0 to 1.0f for now. This could be a
    // time parameter int the future.
    m_hue_parameter = m_sat_parameter = m_val_parameter = 0.0f;
    m_hueTweener->init(h, dh, 1.0f);
    m_satTweener->init(s, ds, 1.0f);
    m_valTweener->init(v, dv, 1.0f);

    // 0.05 is a 20 step animation given a 1.0f timescale
    // 0.01 is a 100 step animation
    double gain = 1.0f / steps;

    m_hue_stepsize = gain;
    m_sat_stepsize = gain;
    m_val_stepsize = gain;

    setState(LIGHT_FADE);
}

void Light::fadeToHue(qreal target_hue, int steps) {
    double h, s, v;
    m_color.getHsvF(&h, &s, &v);
    fadeTo(QColor::fromHsvF(target_hue, s, v), steps);
}

void Light::fadeToValue(qreal target_value, int steps) {
    double h, s, v;
    m_color.getHsvF(&h, &s, &v);
    fadeTo(QColor::fromHsvF(h, s, target_value), steps);
}

void Light::animate() {
    double h, s, v;
    // double dh, ds, dv;
    double gain;

    m_color.getHsvF(&h, &s, &v);

    switch (m_state) {
        case LIGHT_OFF:
        case LIGHT_ON:
            // Do nothing
            break;
        case LIGHT_FADE:
            // Make sure they stay below 1.0f
            m_hue_parameter = math_min(1.0f, m_hue_parameter + m_hue_stepsize);
            m_sat_parameter = math_min(1.0f, m_sat_parameter + m_sat_stepsize);
            m_val_parameter = math_min(1.0f, m_val_parameter + m_val_stepsize);

            h = m_hueTweener->getValue(m_hue_parameter);
            s = m_satTweener->getValue(m_sat_parameter);
            v = m_valTweener->getValue(m_val_parameter);

            // For sanity, clamp 0..1
            h = math_min(math_max(0.0f, h), 1.0f);
            s = math_min(math_max(0.0f, s), 1.0f);
            v = math_min(math_max(0.0f, v), 1.0f);

            // If parameters have all reached their end, state transition.
            if (m_hue_parameter == 1.0f &&
                m_val_parameter == 1.0f &&
                m_sat_parameter == 1.0f) {
                setState(LIGHT_ON);
            }

            m_color.setHsvF(h, s, v);
            break;
        case LIGHT_CYCLE:
        case LIGHT_FFT:
        case LIGHT_RANDOM:
        default:
            break;
    }
}

LightState Light::getState() {
    return m_state;
}

void Light::setState(LightState state) {
    m_state = state;
}
