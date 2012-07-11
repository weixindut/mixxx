#ifndef CHROMAPRINTER_H
#define CHROMAPRINTER_H

#include <QObject>

#include "trackinfoobject.h"

class chromaprinter: public QObject {
  Q_OBJECT

  public:
    chromaprinter(QObject* parent=NULL);
    QString getFingerPrint(TrackPointer pTrack);

  private:
    TrackPointer m_pTrack;
    int m_NumSamples;
    int m_SampleRate;
};

#endif //CHROMAPRINTER_H
