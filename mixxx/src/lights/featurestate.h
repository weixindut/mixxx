#ifndef FEATURESTATE_H
#define FEATURESTATE_H

#include <aubio/aubio.h>

class FeatureState {
  public:
    FeatureState() {
        is_beat = false;
        is_onset = false;
        pitch = 0;
        fft = NULL;
    }
    ~FeatureState() { }

    bool is_beat;
    bool is_onset;
    bool is_silence;
    float pitch;
    cvec_t* fft;
};


#endif /* FEATURESTATE_H */
