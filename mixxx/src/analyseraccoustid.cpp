#include <QtCore>
#include <QtNetwork>

#include <chromaprint.h>
#include "analyseraccoustid.h"

// mixxx use 2 channels for audio buffer as default
const int numChannels = 2;

AnalyserAccoustID::AnalyserAccoustID(QObject* parent)
                 : QObject(parent) {
    // m_pChromaprint = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);
    m_totalSamples=0;
    m_SamplesSoFar=0;
    m_pIn=NULL;
    m_pNetwork = new QNetworkAccessManager(this);
}

bool AnalyserAccoustID::initialise(TrackPointer tio, int sampleRate,
                              int totalSamples){
    Q_UNUSED(tio);
    m_pChromaprint = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);
    chromaprint_start(m_pChromaprint, sampleRate, numChannels);
    // this is worth 30sec of audio, multiply by 2 because we have 2 chanels 
    // --kain88
    m_totalSamples = 30*2*sampleRate;
    if (m_totalSamples > totalSamples) {
        m_totalSamples = totalSamples;
    }
    m_pIn = new SAMPLE[m_totalSamples];
    m_SamplesSoFar=0;
    return true;
}

void AnalyserAccoustID::process(const CSAMPLE *pIn, const int iLen){
    // make sure that samplesSoFar<= totalSamples is true
    if(m_SamplesSoFar+iLen>m_totalSamples){
        return;
    }
    // convert 32bit float (CSAMPLE) to 16bit int (SAMPLE)
    for (int i= m_SamplesSoFar; i<iLen+m_SamplesSoFar; ++i) {
        m_pIn[i] = pIn[i-m_SamplesSoFar]*32767.0f;
    }
    m_SamplesSoFar+=iLen;
}

void AnalyserAccoustID::finalise(TrackPointer tio){
    if (m_SamplesSoFar != m_totalSamples) {
        qDebug() << "apprently I have read less samples then I ought to have"
                 << "strange" ;
    }
    int success = chromaprint_feed(m_pChromaprint,m_pIn,m_SamplesSoFar);
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
    // qDebug() << tio->getAccoustID();
    // qDebug() << tio->getDuration();
    cleanup(tio); 
    
    typedef QPair<QString, QString> Param;
    QList<Param> parameters;
    parameters << Param("format", "xml")
                << Param("client", "ErlAvPUB")
                << Param("duration", QString::number(tio->getDuration()))
                << Param("meta", "recordingids")
                << Param("fingerprint", fingerprint);
    QUrl url("http://api.acoustid.org/v2/lookup");
    url.setQueryItems(parameters);
    qDebug() << url.toString();

    m_pNetwork->get(QNetworkRequest(url));
    connect(m_pNetwork, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void AnalyserAccoustID::cleanup(TrackPointer tio){
    Q_UNUSED(tio);
    chromaprint_free(m_pChromaprint);
    delete m_pIn;
    m_pIn=NULL;
}

void AnalyserAccoustID::replyFinished(QNetworkReply* reply){
    QString data = QString::fromUtf8(reply->readAll());
    qDebug() << data;
}
