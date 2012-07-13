#include <QtCore>
#include <QtNetwork>
#include <chromaprint.h>

#include "library/chromaprinter.h"
// #include "soundsourceproxy.h"
#include "defs.h"

chromaprinter::chromaprinter(QObject* parent)
             : QObject(parent){
}

QString chromaprinter::getFingerPrint(TrackPointer pTrack){
    SoundSourceProxy soundSource(pTrack);
    return calcFingerPrint(soundSource);
}

QString chromaprinter::getFingerPrint(QString location){
    SoundSourceProxy soundSource(location);
    return calcFingerPrint(soundSource);
}

QString chromaprinter::calcFingerPrint(SoundSourceProxy& soundSource){
    soundSource.open();
    m_SampleRate = soundSource.getSampleRate();
    // this is worth 2min of audio, multiply by 2 because we have 2 chanels 
    // --kain88
    m_NumSamples = 120*2*m_SampleRate;
    if (m_NumSamples > soundSource.length()) {
        m_NumSamples = soundSource.length();
    }

    if (m_SampleRate == 0 ){
        qDebug() << "Skipping invalid file:" << soundSource.getFilename();
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
    // qDebug() << "success ? = "<<success;
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
    }
    chromaprint_free(ctx);
    delete pData;

    return fingerprint;
}
