
#include <QtDebug>

#undef VERSION
#define AUBIO_UNSTABLE 1
#include <aubio/aubio.h>
#include <lo/lo.h>
#include <QMutexLocker>

#include "lights/light.h"
#include "lights/controlgroup.h"
#include "lights/lightmanager.h"
#include "lights/dmxlightmanager.h"
#include "lights/lightbrickmanager.h"

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

    const uint_t channels = 1;
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
    m_features.fft = m_fft_output;

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
    m_lightManagers.append(m_pLightBrickManager);

    ControlGroup* pGroup1 = new ControlGroup(this, "Control Group 1");
    ControlGroup* pGroup2 = new ControlGroup(this, "Control Group 2");

    Light* pLight = m_pLightBrickManager->newLight("192.168.1.2", "12344");
    pLight->setColor(Qt::black);
    m_pLightBrickManager->sync();
    m_lights.append(pLight);
    pGroup2->addLight(pLight);

    m_pDMXManager = new DMXLightManager(this, QString("10.0.170.6"));
    m_lightManagers.append(m_pDMXManager);
    const int numDmxLights = 12;
    for (int i = 0; i < numDmxLights; i++) {
        Light* pLight = m_pDMXManager->newLight(i);
        pLight->setColor(Qt::black);
        m_lights.append(pLight);
        pGroup1->addLight(pLight);
    }

    // Turn off the lights
    m_pDMXManager->sync();

    static RGBCycler cycler(20, 200, 70, 50, 100, 20);
    // You spin me right round.
    static HSVSpinner spinner(1.0, 1.0, 1.0, 0.05);
    m_pColorGenerator = &cycler;

    pGroup1->setColorGenerator(&cycler);
    pGroup1->setTriggerMode(BEAT);
    pGroup1->setControlMode(CONTROL_CYCLE_FADE);

    pGroup2->setColorGenerator(&spinner);
    pGroup2->setTriggerMode(BEAT);
    pGroup2->setControlMode(CONTROL_CYCLE_FLASH);

    m_controlGroups.append(pGroup1);
    m_controlGroups.append(pGroup2);
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

void LightController::process_buffer() {
    //aubio_onset_do(m_aubio_onset, m_input_buf, m_onset_output);
    aubio_tempo_do(m_aubio_tempo, m_input_buf, m_tempo_output);
    aubio_pitch_do(m_aubio_pitch, m_input_buf, m_pitch_output);
    aubio_fft_do(m_aubio_fft, m_input_buf, m_fft_output);

    m_features.is_silence = aubio_silence_detection(m_input_buf, ANALYZER_BEAT_SILENCE);
    m_features.is_beat = fvec_read_sample(m_tempo_output, 0, 0) > 0;
    m_features.is_onset = fvec_read_sample(m_tempo_output, 0, 1) > 0;
    m_features.pitch = fvec_read_sample(m_pitch_output, 0, 0);


    // if (m_features.is_beat) {
    //     QColor color = m_pColorGenerator->nextColor();
    //     foreach (Light* pLight, m_lights) {
    //         // Fade to the color in 20 steps
    //         pLight->fadeTo(color, 50);

    //         // Immediately switch to color
    //         //pLight->setColor(color);

    //         // Fade to black in 100 steps
    //         //pLight->fadeDown(100);
    //     }
    //     // Only enable if you want to lessen the frequency of updates to the
    //     // bricks. (Combine with commenting the sync() call below)
    //     //m_pLightBrickManager->sync();
    // }

    //qDebug() << "beat: " << m_features.is_beat << " onset: " << m_state.is_onset;
    //qDebug() << "pitch:" << m_features.pitch;
    //qDebug() << "fft:" << m_fft_output->norm[0][0] << m_fft_output->norm[0][1];
}

void LightController::setColorGenerator(ColorGenerator* pGenerator) {
    QMutexLocker locker(&m_mutex);
    m_pColorGenerator = pGenerator;
}


void LightController::setColor(QColor color) {
    QMutexLocker locker(&m_mutex);

    foreach (Light* pLight, m_lights) {
        pLight->fadeTo(color, 20);
        //pLight->setColor(color);
    }
}

void LightController::process(SAMPLE* pSample, int iFramesPerBuffer) {
    QMutexLocker locker(&m_mutex);

    Q_ASSERT(iFramesPerBuffer % 2 == 0);
    int monoSamples = iFramesPerBuffer / 2;

    for (int i = 0; i < monoSamples; ++i) {
        float monoSample = 0.5f * (float(pSample[i*2+0]) + float(pSample[i*2+1]));
        fvec_write_sample(m_input_buf, monoSample, 0, m_iCurInput);

        // We filled up the buffer, process it.
        if (m_iCurInput == ANALYZER_BEAT_OLAPSIZE - 1) {
            process_buffer();
            m_iCurInput = -1; // gets inc'd to 0 below
        }
        m_iCurInput++;
    }

    // Process state updates for all the lights in each control group
    foreach (ControlGroup* pGroup, m_controlGroups) {
        pGroup->process(&m_features);
    }

    // Tell all the light managers to synchronize their lights with their
    // physical counterparts.
    foreach (LightManager* pManager, m_lightManagers) {
        pManager->sync();
    }
}


// this method is unused!
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

int LightController::numControlGroups() {
    return m_controlGroups.size();
}

ControlGroup* LightController::getControlGroup(int group) {
    return m_controlGroups.at(group);
}
