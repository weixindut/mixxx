#ifndef VINYLCONTROLCONTROL_H
#define VINYLCONTROLCONTROL_H

#include "engine/enginecontrol.h"
#include "trackinfoobject.h"
#include "configobject.h"
#include "controlobject.h"
#include "controlpushbutton.h"

class CallbackControl;
class CallbackControlManager;

class VinylControlControl : public EngineControl {
    Q_OBJECT
  public:
    VinylControlControl(const char* pGroup, ConfigObject<ConfigValue>* pConfig,
                        CallbackControlManager* pCallbackControlManager);
    virtual ~VinylControlControl();

    void trackLoaded(TrackPointer pTrack);
    void trackUnloaded(TrackPointer pTrack);

  private slots:
    void slotControlVinylSeek(double value);

  private:
    CallbackControl* m_pControlVinylEnabled;
    CallbackControl* m_pControlVinylMode;
    CallbackControl* m_pControlVinylCueing;
    TrackPointer m_pCurrentTrack;
};

#endif /* VINYLCONTROLCONTROL_H */
