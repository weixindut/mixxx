#ifndef FEATURESTATE_H
#define FEATURESTATE_H

#include <QTime>

#include <aubio/aubio.h>

class FeatureState {
  public:
    FeatureState() {
        is_fresh = false;
        is_beat = false;
        is_onset = false;
        pitch = 0;
        fft = NULL;
    }
    ~FeatureState() { }

    QTime current_time;
    QTime previous_time;

    bool is_fresh;
    bool is_beat;
    bool is_onset;
    bool is_silence;
    float pitch;
    cvec_t* fft;
};


#endif /* FEATURESTATE_H */
