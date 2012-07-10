#include <QtCore>
#include <chromaprint.h>
#include "analyseraccoustid.h"

// mixxx use 2 channels for audio buffer as default
const int numChannels = 2;

AnalyserAccoustID::AnalyserAccoustID(){
    // m_pChromaprint = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);
    m_totalSamples=0;
    m_SamplesSoFar=0;
    m_pIn=NULL;
}

bool AnalyserAccoustID::initialise(TrackPointer tio, int sampleRate,
                              int totalSamples){
    Q_UNUSED(tio);
    m_pChromaprint = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);
    chromaprint_start(m_pChromaprint, sampleRate, numChannels);
    m_totalSamples = totalSamples;
    m_pIn = new SAMPLE[totalSamples];
    m_SamplesSoFar=0;
    return true;
}

void AnalyserAccoustID::process(const CSAMPLE *pIn, const int iLen){
    //convert 32bit float (CSAMPLE) to 16bit int (SAMPLE)
    for (int i= m_SamplesSoFar; i<iLen+m_SamplesSoFar; ++i) {
        m_pIn[i] = pIn[i-m_SamplesSoFar]*32767.0f;
    }
    m_SamplesSoFar+=iLen;
}

void AnalyserAccoustID::finalise(TrackPointer tio){
    qDebug() << "analyse fingerprint";
    if (m_SamplesSoFar != m_totalSamples) {
        qDebug() << "apprently I have read less samples then I ought to have"
                 << "strange" ;
    }
    int success = chromaprint_feed(m_pChromaprint,m_pIn,m_totalSamples);
    qDebug() << success;
    chromaprint_finish(m_pChromaprint);
    void* fprint = NULL;
    int size = 0;
    int ret = chromaprint_get_raw_fingerprint(m_pChromaprint, &fprint, &size);
    QByteArray fingerprint;
    if (ret == 1) {
        void* encoded = NULL;
        int encoded_size = 0;
        chromaprint_encode_fingerprint(fprint, size,
                                       CHROMAPRINT_ALGORITHM_DEFAULT,
                                       &encoded,
                                       &encoded_size, 1);

        fingerprint.append(reinterpret_cast<char*>(encoded), encoded_size);

        chromaprint_dealloc(fprint);
        chromaprint_dealloc(encoded);
    }
    tio->setAccoustID(fingerprint);
    qDebug() << tio->getAccoustID();
    qDebug() << tio->getDuration();
    cleanup(tio);
}

void AnalyserAccoustID::cleanup(TrackPointer tio){
    Q_UNUSED(tio);
    chromaprint_free(m_pChromaprint);
    delete m_pIn;
    m_pIn=0;
}
