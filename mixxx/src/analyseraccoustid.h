#ifndef ANALYSER_ACCOUSTID_H
#define ANALYSER_ACCOUSTID_H

#include <chromaprint.h>

#include "analyser.h"
#include "defs.h"


class AnalyserAccoustID : public Analyser {
  public:
    AnalyserAccoustID();
    bool initialise(TrackPointer tio, int sampleRate, int totalSamples);
    void process(const CSAMPLE *pIn, const int iLen);
    void cleanup(TrackPointer tio);
    void finalise(TrackPointer tio);

  private:
    int m_totalSamples;
    int m_SamplesSoFar;
    SAMPLE* m_pIn;
    ChromaprintContext* m_pChromaprint;
};

#endif
