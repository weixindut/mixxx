#ifndef CALLBACKCONTROLMANAGER_H
#define CALLBACKCONTROLMANAGER_H

#include <QSet>
#include <QObject>
#include <QScopedPointer>
#include <QTime>

#include "util/fifo.h"
#include "configobject.h"
#include "controlobject.h"
#include "trackinfoobject.h"

class CallbackControlManager;

struct ControlUpdate {
    ControlUpdate()
            : time(0),
              value(0) {
    }
    ControlUpdate(double time, double value)
            : time(time),
              value(value) {
    }
    double time;
    double value;
};

struct CallbackControlMessage {
    ControlObject* control;
    ControlUpdate update;
};

struct TrackUpdate {
    enum Type {
        BEATS_UPDATED,
        CUES_UPDATED
    };

    Type type;
};

struct CallbackTrackMessage {
    TrackInfoObject* track;
    TrackUpdate update;
};

class ControlWatcher : public QObject {
    Q_OBJECT
  public:
    ControlWatcher(ControlObject* pControl);
    virtual ~ControlWatcher();
  signals:
    void controlUpdated(ControlObject* pControl, double value);
  private slots:
    void slotValueChanged(double v);
  private:
    ControlObject* m_pControl;
};

class CallbackControl : public QObject {
    Q_OBJECT
  public:
    // Create a CallbackControl wrapper around pControl. Takes ownership of
    // pControl.
    CallbackControl(CallbackControlManager* pControlManager,
                    ControlObject* pControl, int bufferLength);
    virtual ~CallbackControl();

    // Get access to the internal control. WARNING: Touching this control is NOT
    // callback-safe.
    ControlObject* getControl() const {
        return m_control.data();
    }

    ConfigKey getKey() const {
        return m_control->getKey();
    }

    inline double get() const {
        return m_value.value;
    }
    void set(double value);
    void add(double value);
    void sub(double value);

    // Receive update from CallbackControlManager
    void receiveUpdate(ControlUpdate update);

  signals:
    // Callback-safe signal that a control value is changed. Always emitted from
    // the callback thread. WARNING: always connect to this signal with
    // Qt::DirectConnection or the slot WILL NOT be callback-safe.
    void valueChanged(double value);
    void valueChanged(double value, double time);
    void valueChangedFromEngine(double value);
    void valueChangedFromEngine(double value, double time);
    void controlUpdatedFromCallback(CallbackControl* pControl, double value);

  private:
    ControlUpdate m_value;
    FIFO<ControlUpdate> m_updates;
    QScopedPointer<ControlObject> m_control;
    ControlWatcher m_controlWatcher;
};

class TrackWatcher : public QObject {
    Q_OBJECT
  public:
    TrackWatcher();
    TrackWatcher(const TrackPointer& pTrack);
    TrackWatcher(const TrackWatcher& trackWatcher);
    virtual ~TrackWatcher();

    TrackWatcher& operator=(const TrackWatcher& trackWatcher) {
        *this = TrackWatcher(trackWatcher);
        return *this;
    }

    const TrackPointer& getTrack() const {
        return m_pTrack;
    }

  signals:
    void trackUpdated(TrackPointer pTrack, TrackUpdate update);

  private slots:
    void slotCuesUpdated();
    void slotBeatsUpdated();

  private:
    TrackPointer m_pTrack;
};

class CallbackTrackWatcher : public QObject {
    Q_OBJECT
  public:
    CallbackTrackWatcher();
    virtual ~CallbackTrackWatcher();

    // Receive update from CallbackTrackManager
    void receiveUpdate(TrackUpdate update);

    void watchTrack(const TrackPointer& pTrack);
    void unwatchTrack(const TrackPointer& pTrack);

  signals:
    void cuesUpdated();
    void beatsUpdated();

    void startWatchingTrack(TrackPointer pTrack, CallbackTrackWatcher* pTrackWatcher);
    void stopWatchingTrack(TrackPointer pTrack, CallbackTrackWatcher* pTrackWatcher);
};

class CallbackTrackManager : public QObject {
    Q_OBJECT
  public:
    explicit CallbackTrackManager();
    virtual ~CallbackTrackManager();

    CallbackTrackWatcher* createTrackWatcher();

    void callbackProcessIncomingUpdates();

  public slots:
    // Must only invoke from the callback.
    void addTrackWatcher(const TrackPointer& pTrack, CallbackTrackWatcher* pTrackWatcher);
    void removeTrackWatcher(const TrackPointer& pTrack, CallbackTrackWatcher* pTrackWatcher);

    // Thread safe but NOT callback-safe. Indicates that the track 'pTrack' was
    // updated with the update 'update'.
    void slotTrackUpdated(const TrackPointer& pTrack, TrackUpdate update);

  private:
    ////////////////////////////////////////////////////////////////////////////
    // The following may only be called/touched while m_incomingUpdateMutex is
    // held.
    ////////////////////////////////////////////////////////////////////////////

    QMutex m_incomingUpdateMutex;
    FIFO<CallbackTrackMessage> m_incomingUpdateFifo;

    ////////////////////////////////////////////////////////////////////////////
    // The following may only be touched from within the callback
    ////////////////////////////////////////////////////////////////////////////

    struct TrackWatchers {
        TrackWatchers(const TrackPointer& pTrack)
                : watcher(pTrack) {
        }

        TrackWatchers(const TrackWatchers& other_watchers)
                : watcher(other_watchers.watcher),
                  callback_watchers(other_watchers.callback_watchers) {
        }

        TrackWatcher watcher;
        QList<CallbackTrackWatcher*> callback_watchers;
    };

    QHash<TrackInfoObject*, TrackWatchers> m_trackWatchers;
};

class CallbackControlManager : public QObject {
    Q_OBJECT
  public:
    explicit CallbackControlManager();
    virtual ~CallbackControlManager();

    // Add a control to be managed by CallbackControlManager. Returns a
    // CallbackControl. MUST ONLY BE CALLED DURING MIXXX INITIALIZATION.
    CallbackControl* addControl(ControlObject* pControl, int bufferLength);

    // Get a CallbackControl for the given ControlObject
    CallbackControl* getControl(ConfigKey key);

    void callbackProcessIncomingUpdates();
    void callbackProcessOutgoingUpdates();
    void processOutgoingUpdates();

  public slots:
    // Thread Safe but NOT callback-safe. Indicates that the control 'pControl'
    // was just updated to value 'value'
    void slotControlUpdated(ControlObject* pControl, double value);

    // Callback-safe.
    void slotControlUpdatedFromCallback(CallbackControl* pControl, double value);

  private:
    ////////////////////////////////////////////////////////////////////////////
    // The following may only be called/touched while m_incomingUpdateMutex is
    // held.
    ////////////////////////////////////////////////////////////////////////////

    double getTime() {
        // So stupid, but we can't trust QTime to give us accurate long-term
        // times since if somebody changes the system clock while running Mixxx,
        // the results is basically undefined. Restart it every time we get the
        // time and keep a tally.
        double elapsed = m_timer.restart();
        m_lastTimeValue += elapsed;
        return m_lastTimeValue;
    }
    QMutex m_incomingUpdateMutex;
    FIFO<CallbackControlMessage> m_incomingUpdateFifo;


    // TODO(XXX) Switch to QElapsedTimer once we require Qt >=4.7
    QTime m_timer;
    double m_lastTimeValue;

    ////////////////////////////////////////////////////////////////////////////
    // The following may only be touched from within the callback
    ////////////////////////////////////////////////////////////////////////////

    QHash<ConfigKey, CallbackControl*> m_callbackControls;
    QSet<ConfigKey> m_updatedControls;
    FIFO<CallbackControlMessage> m_outgoingUpdateFifo;
};

#endif /* CALLBACKCONTROLMANAGER_H */
