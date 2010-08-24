#include <QtDebug>

#include "lights/light.h"

#include "mathstuff.h"

Light::Light(QObject* pParent) {

}

Light::~Light() {

}

void Light::setColor(const QColor& color) {
    setState(LIGHT_ON);
    m_color = color;
}

QColor Light::getColor() {
    return m_color;
}

void Light::fadeDown() {
    setState(LIGHT_FADE_DOWN);
}

void Light::fadeTo(const QColor& targetColor) {
    setState(LIGHT_FADE_UP);
    m_animation_arg1 = targetColor;
    qreal scratch;
    targetColor.getHsvF(&m_target_hue, &m_target_sat, &m_target_val, &scratch);
}

void Light::fadeToHue(qreal hue) {
    m_target_hue = hue;
    setState(LIGHT_FADE_UP);
}

void Light::animate() {
    double h, s, v, a;
    double th, ts, tv, ta;
    double dh, ds, dv, da;
    double gain;

    m_color.getHsvF(&h, &s, &v, &a);

    switch (m_state) {
        case LIGHT_OFF:
        case LIGHT_ON:
            // Do nothing
            break;
        case LIGHT_FADE_DOWN:
            // Cubic ease out
            // t - time
            // c - change in value
            // b - beginning value
            // d - duration
            // return c*((t=t/d-1)*t*t + 1) + b;

            // gain 0.05 is strobe-like
            gain = 0.01f;
            if (v > 0) {
                v -= gain;
                v = math_max(v, 0.0f);
            }

            if (v == 0.0f) {
                setState(LIGHT_OFF);
            }

            m_color.setHsvF(h, s, v, a);
            break;
        case LIGHT_FADE_UP:
            th = m_target_hue;
            tv = m_target_val;
            ts = m_target_sat;
            //m_animation_arg1.getHsvF(&th, &ts, &tv, &ta);

            dh = (th - h);
            ds = (ts - s);
            dv = (tv - v);
            //da = (ta - a);


            //gain = 0.01f;
            gain = 0.05f;


            h += dh * gain;
            s += ds * gain;
            v += dv * gain;

            // TODO take shortest path in angular space
            // h += math_max(-gain, math_min(gain, dh));
            // s += math_max(-gain, math_min(gain, ds));
            // v += math_max(-gain, math_min(gain, dv));
            //a += min(0.1f, da);

            // Clamp 0..1
            h = math_min(math_max(0.0f, h), 1.0f);
            s = math_min(math_max(0.0f, s), 1.0f);
            v = math_min(math_max(0.0f, v), 1.0f);

            if (dh == 0.0f && ds == 0.0f && dv == 0.0f) {
                setState(LIGHT_ON);
            }

            //qDebug() << dh << ds << dv << h << s << v << a;
            m_color.setHsvF(h, s, v, a);
            break;
        case LIGHT_CYCLE:
        case LIGHT_FFT:
        case LIGHT_RANDOM:
        default:
            break;
    }
}

void Light::setState(LightState state) {
    m_state = state;
}
