
#include <QtDebug>

#undef VERSION
#define AUBIO_UNSTABLE 1
#include <aubio/aubio.h>
#include <lo/lo.h>
#include <QMutexLocker>


#include "lights/lightcontroller.h"
#include "lights/rgbcycler.h"
#include "lights/hsvspinner.h"
#include "lights/colorgenerator.h"

#define ANALYZER_BEAT_BUFSIZE 1024
#define ANALYZER_BEAT_OLAPSIZE 512

#define ANALYZER_BEAT_WINSIZE 1024
#define ANALYZER_BEAT_HOPSIZE 256
#define ANALYZER_BEAT_THRESHOLD 0.2
#define ANALYZER_BEAT_DCTHRESHOLD 0.8
#define ANALYZER_BEAT_SILENCE -60.

LightController* LightController::sInstance = NULL;

LightController::LightController() {
    const uint_t channels = 2;
    const uint_t samplerate = 48000;

    sInstance = this;

    QString host = "localhost";
    QString port = "2447";

    m_osc_destination = lo_address_new(host.toAscii().data(),
                                       port.toAscii().data());

    m_iCurInput = 0;
    m_input_buf = new_fvec(ANALYZER_BEAT_BUFSIZE, channels);
    // onset
    char* onset_method = "default";

    m_aubio_onset = new_aubio_onset (onset_method,
                                     ANALYZER_BEAT_BUFSIZE,
                                     ANALYZER_BEAT_OLAPSIZE,
                                     channels, samplerate);
    m_onset_output = new_fvec(ANALYZER_BEAT_OLAPSIZE, channels);


    m_aubio_fft = new_aubio_fft (ANALYZER_BEAT_WINSIZE, channels);
    m_fft_output = new_cvec(ANALYZER_BEAT_WINSIZE/2, channels);

    // beattracking
    m_aubio_tempo = new_aubio_tempo(onset_method,
                                    ANALYZER_BEAT_BUFSIZE,
                                    ANALYZER_BEAT_OLAPSIZE,
                                    channels, samplerate);
    m_tempo_output = new_fvec(2, channels);

    m_aubio_pitch = new_aubio_pitch(onset_method,
                                    ANALYZER_BEAT_BUFSIZE,
                                    ANALYZER_BEAT_OLAPSIZE,
                                    channels, samplerate);
    aubio_pitch_set_unit(m_aubio_pitch, "midi");
    m_pitch_output = new_fvec(1, channels);


    m_pLightBrickManager = new LightBrickManager(this);
    Light* pLight = m_pLightBrickManager->newLight("192.168.1.2", "12344");
    //Light* pLight = m_pLightBrickManager->newLight("192.168.1.125", "12344");
    pLight->setColor(Qt::black);
    //m_pLightBrickManager->sync();
    m_lights.append(pLight);

    m_pDMXManager = new DMXLightManager(this, QString("10.0.170.6"));
    const int numDmxLights = 12;
    for (int i = 0; i < numDmxLights; i++) {
        Light* pLight = m_pDMXManager->newLight(i);
        pLight->setColor(Qt::black);
        m_lights.append(pLight);
    }

    // Turn off the lights
    m_pDMXManager->sync();

    is_beat = is_onset = false;

    static RGBCycler cycler(20, 200, 70, 50, 100, 20);
    // You spin me right round.
    static HSVSpinner spinner(1.0, 1.0, 1.0, 0.05);
    m_pColorGenerator = &cycler;

}

LightController::~LightController() {
    del_aubio_onset(m_aubio_onset);
    m_aubio_onset = NULL;
    del_aubio_tempo(m_aubio_tempo);
    m_aubio_tempo = NULL;
    del_aubio_pitch(m_aubio_pitch);
    m_aubio_pitch = NULL;
    del_aubio_fft(m_aubio_fft);
    m_aubio_fft = NULL;
}

void LightController::process_onset() {
    //aubio_onset_do(m_aubio_onset, m_input_buf, m_onset_output);
    aubio_tempo_do(m_aubio_tempo, m_input_buf, m_tempo_output);
    aubio_pitch_do(m_aubio_pitch, m_input_buf, m_pitch_output);
    aubio_fft_do(m_aubio_fft, m_input_buf, m_fft_output);

    is_beat = fvec_read_sample(m_tempo_output, 0, 0) > 0;
    is_onset = fvec_read_sample(m_tempo_output, 0, 1) > 0;
    m_currentPitch = fvec_read_sample(m_pitch_output, 0, 0);

    if (is_beat) {
        QColor color = m_pColorGenerator->nextColor();
        foreach (Light* pLight, m_lights) {
            //pLight->fadeTo(color);
            pLight->setColor(color);
            //pLight->fadeDown();
            //pLight->animate();
        }

        m_pLightBrickManager->sync();
    }

    //qDebug() << "beat: " << is_beat << " onset: " << is_onset;
    //qDebug() << "pitch:" << m_currentPitch;

    //qDebug() << "fft:" << m_fft_output->norm[0][0] << m_fft_output->norm[0][1];
    // if (fvec_read_sample(m_onset_output, 0, 0) == 1) {
    //     // processed frame has an onset
    // } else {
    //     // no onset detected in processed frame
    // }

}

void LightController::setColorGenerator(ColorGenerator* pGenerator) {
    QMutexLocker locker(&m_mutex);
    m_pColorGenerator = pGenerator;
}


void LightController::setColor(QColor color) {
    QMutexLocker locker(&m_mutex);

    foreach (Light* pLight, m_lights) {
        pLight->fadeTo(color);
        //pLight->setColor(color);
    }
    //m_pDMXManager->sync();
}

void LightController::process(SAMPLE* pSample, int iFramesPerBuffer) {
    QMutexLocker locker(&m_mutex);

    for (int i = 0; i < iFramesPerBuffer; i += 2) {
        for (int j = 0; j < 2; ++j) {
            fvec_write_sample(m_input_buf, pSample[i+j], j, m_iCurInput);
            //m_input_buf[j][m_iCurInput] = pSample[i+j];;
        }

        // We filled up the buffer, process it.
        if (m_iCurInput == ANALYZER_BEAT_OLAPSIZE - 1) {
            process_onset();
            m_iCurInput = -1; // gets inc'd to 0 below
        }
        m_iCurInput++;
    }


    // Animate the lights and synchronize them with their hardware.
    static int count = 0;
    if (count == 0) {
        foreach (Light* pLight, m_lights) {
            pLight->animate();
            // if (pLight == m_lights[0])
            //     qDebug() << pLight->getColor();
        }
        m_pDMXManager->sync();
        m_pLightBrickManager->sync();
        count = 0;
    } else {
        count--;
    }
}

// unused
bool LightController::send_light_update(char light_number, char red, char green, char blue) {
    QString locationFormat = "/simulator/lights/set";
    if (lo_send(m_osc_destination, locationFormat.toAscii().data(),
                "cccc", light_number, red, green, blue) < 0) {
        qDebug() << "OSC error " << lo_address_errno(m_osc_destination)
                 << ": " << lo_address_errstr(m_osc_destination);
        return false;
    }
    return true;
}
