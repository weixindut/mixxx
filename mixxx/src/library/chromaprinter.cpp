#include <QtCore>
#include <QtNetwork>
#include <chromaprint.h>

#include "library/chromaprinter.h"
#include "soundsourceproxy.h"
#include "defs.h"

chromaprinter::chromaprinter(QObject* parent)
             : QObject(parent){
}

QString chromaprinter::getFingerPrint(TrackPointer pTrack){
    m_pTrack = pTrack;
    SoundSourceProxy soundSource(pTrack);
    soundSource.open();
    qDebug() << "test something";
    qDebug() << soundSource.length()/soundSource.getSampleRate() ;
    qDebug() << pTrack->getDuration();
    m_SampleRate = soundSource.getSampleRate();
    // this is worth 30sec of audio, multiply by 2 because we have 2 chanels 
    // --kain88
    m_NumSamples = 30*2*m_SampleRate;
    if (m_NumSamples > soundSource.length()) {
        m_NumSamples = soundSource.length();
    }

    qDebug() << "numSamples = " <<m_NumSamples;
    qDebug() << "numSampleRate = " <<m_SampleRate;

    if (m_SampleRate == 0 ){
        qDebug() << "Skipping invalid file:" << m_pTrack->getLocation();
        return QString();
    }

    SAMPLE *pData = new SAMPLE[m_NumSamples];
    int read = soundSource.read(m_NumSamples, pData);

    if (read!=m_NumSamples) {
        qDebug() << "oh that's embarrasing I couldn't read a file";
    }

    ChromaprintContext* ctx = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);
    // we have 2 channels in mixxx always
    chromaprint_start(ctx, m_SampleRate, 2);

    int success = chromaprint_feed(ctx, pData, m_NumSamples);
    qDebug() << "success ? = "<<success;
    chromaprint_finish(ctx);
    
    void* fprint = NULL;
    int size = 0;
    int ret = chromaprint_get_raw_fingerprint(ctx, &fprint, &size);
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
        qDebug() <<"encoded_size=" << encoded_size;
    }
    chromaprint_free(ctx);
    delete pData;

    // qDebug() << QString(fingerprint);
    typedef QPair<QString, QString> Param;
    QList<Param> parameters;
    parameters << Param("format", "xml")
                << Param("client", "ErlAvPUB")
                << Param("duration", QString::number(pTrack->getDuration()))
                << Param("meta", "recordingids")
                << Param("fingerprint", fingerprint);
    QUrl url("http://api.acoustid.org/v2/lookup");
    url.setQueryItems(parameters);
    qDebug() << url.toString();
    
    return fingerprint;
}
