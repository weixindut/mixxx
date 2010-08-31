
#include <aubio/aubio.h>

#include "lights/lightshowcontroller.h"
#include "lights/controlgroup.h"
#include "lights/lightcontroller.h"
#include "lights/featurestate.h"

LightShowController::LightShowController(LightController* pController)
        : QObject(pController),
          m_pLightController(pController) {
}

LightShowController::~LightShowController() {

}

void LightShowController::addControlGroup(ControlGroup* pGroup) {
    m_controlGroups.append(pGroup);
}

void LightShowController::process(FeatureState* pState) {

    int fft_bins = pState->fft->length;

    int bins_per_group = fft_bins / 3;
    qreal low_sum = 0;
    qreal mid_sum = 0;
    qreal high_sum = 0;

    for (int i = 0; i < bins_per_group; ++i) {
        low_sum += cvec_read_norm(pState->fft, 0, i);
        mid_sum += cvec_read_norm(pState->fft, 0, bins_per_group + i);
        high_sum += cvec_read_norm(pState->fft, 0, 2*bins_per_group + i);
    }

    //qDebug() << "bins per group" << bins_per_group << "fft_bins" << fft_bins;

    low_sum /= bins_per_group;
    mid_sum /= bins_per_group;
    high_sum /= bins_per_group;

    qreal alpha = 0.1f;
    // m_lowFftAverage = m_lowFftAverage * (1-alpha) + alpha * low_sum;
    // m_midFftAverage = m_midFftAverage * (1-alpha) + alpha * mid_sum;
    // m_highFftAverage = m_highFftAverage * (1-alpha) + alpha * high_sum;

    //qDebug() << m_lowFftAverage << m_midFftAverage << m_highFftAverage;
}

