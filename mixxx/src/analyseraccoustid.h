#ifndef ANALYSER_ACCOUSTID_H
#define ANALYSER_ACCOUSTID_H

#include <chromaprint.h>
#include <QObject>

#include "analyser.h"
#include "defs.h"

class QNetworkAccessManager;
class QNetworkReply;

class AnalyserAccoustID : public QObject ,public Analyser {
  Q_OBJECT;
  public:
    AnalyserAccoustID(QObject* parent=NULL);
    bool initialise(TrackPointer tio, int sampleRate, int totalSamples);
    void process(const CSAMPLE *pIn, const int iLen);
    void cleanup(TrackPointer tio);
    void finalise(TrackPointer tio);

  public slots:
    void replyFinished(QNetworkReply* reply);

  private:
    int m_totalSamples;
    int m_SamplesSoFar;
    SAMPLE* m_pIn;
    ChromaprintContext* m_pChromaprint;

    QNetworkAccessManager* m_pNetwork;
};

#endif
