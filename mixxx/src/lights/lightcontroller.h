#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QString>
#include <QObject>
#include <QList>
#include <QMutex>

#include <aubio/aubio.h>
#include <lo/lo.h>

#include "defs.h"

#include "lights/light.h"
#include "lights/dmxlightmanager.h"
#include "lights/lightbrickmanager.h"

class LightController : public QObject {
    Q_OBJECT
  public:
    LightController();
    virtual ~LightController();

    void process(SAMPLE* pBuffer, int iFramesPerBuffer);

    void setColor(QColor color);


  private:
    bool send_light_update(char light_number, char red, char green, char blue);
    void process_onset();

    // OSC address
    QString m_base_location;
    lo_address m_osc_destination;

    QMutex m_mutex;

    // aubio structures
    aubio_fft_t* m_aubio_fft;
    aubio_onset_t* m_aubio_onset;
    aubio_tempo_t* m_aubio_tempo;
    aubio_pitch_t* m_aubio_pitch;

    fvec_t* m_input_buf;
    fvec_t* m_tempo_output;
    fvec_t* m_onset_output;
    fvec_t* m_pitch_output;
    cvec_t* m_fft_output;

    int m_iCurInput;
    bool is_beat, is_onset;
    float m_currentPitch;

    DMXLightManager* m_pDMXManager;
    LightBrickManager* m_pLightBrickManager;

    QList<Light*> m_lights;

  public:
    // Dirty dirty


    static LightController* sInstance;
};

#endif /* LIGHTCONTROLLER_H */
