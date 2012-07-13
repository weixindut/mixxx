#ifndef CHROMAPRINTER_H
#define CHROMAPRINTER_H

#include <QObject>

#include "trackinfoobject.h"
#include "soundsourceproxy.h"

class chromaprinter: public QObject {
  Q_OBJECT

  public:
    chromaprinter(QObject* parent=NULL);
    QString getFingerPrint(TrackPointer pTrack);
    QString getFingerPrint(QString location);

  private:

    QString calcFingerPrint(SoundSourceProxy& soundSource);
    int m_NumSamples;
    int m_SampleRate;
};

#endif //CHROMAPRINTER_H
