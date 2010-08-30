#ifndef FEATURESTATE_H
#define FEATURESTATE_H

#include <QTime>

#include <aubio/aubio.h>

class FeatureState {
  public:
    FeatureState()
            : beat_count(-1), // the first beat will be beat 0 this way
              is_fresh(false),
              is_beat(false),
              is_onset(false),
              pitch(0),
              fft(NULL) {
    }

    ~FeatureState() { }

    QTime current_time;
    QTime previous_time;

    qreal previous_beat_length;
    qreal current_beat_length;

    QTime current_beat_time;
    QTime previous_beat_time;

    long beat_count;

    bool is_fresh;
    bool is_beat;
    bool is_onset;
    bool is_silence;
    float pitch;
    cvec_t* fft;
};


#endif /* FEATURESTATE_H */
