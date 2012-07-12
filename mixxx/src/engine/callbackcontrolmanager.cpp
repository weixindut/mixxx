#include <QtDebug>
#include <QMetaType>

#include "engine/callbackcontrolmanager.h"

ControlWatcher::ControlWatcher(ControlObject* pControl)
        : m_pControl(pControl) {
    connect(pControl, SIGNAL(valueChanged(double)),
            this, SLOT(slotValueChanged(double)),
            Qt::DirectConnection);
}

ControlWatcher::~ControlWatcher() {
}

void ControlWatcher::slotValueChanged(double value) {
    // qDebug() << this << "slotValueChanged"
    //          << m_pControl->getKey().group
    //          << m_pControl->getKey().item
    //          << ":" << value;
    emit(controlUpdated(m_pControl, value));
}

CallbackControl::CallbackControl(CallbackControlManager* pControlManager,
                                 ControlObject* pControl, int bufferLength)
        : m_value(0.0, pControl->get()),
          m_updates(bufferLength),
          m_control(pControl),
          m_controlWatcher(pControl) {
    connect(&m_controlWatcher, SIGNAL(controlUpdated(ControlObject*, double)),
            pControlManager, SLOT(slotControlUpdated(ControlObject*, double)),
            Qt::DirectConnection);
    connect(this, SIGNAL(controlUpdatedFromCallback(CallbackControl*, double)),
            pControlManager, SLOT(slotControlUpdatedFromCallback(CallbackControl*, double)),
            Qt::DirectConnection);
}

CallbackControl::~CallbackControl() {
}

void CallbackControl::set(double value) {
    // TODO(XXX) set time.
    m_value.value = value;
    emit(valueChangedFromEngine(m_value.value));
    emit(valueChangedFromEngine(m_value.value, m_value.time));
    emit(controlUpdatedFromCallback(this, m_value.value));
}

void CallbackControl::add(double value) {
    set(m_value.value + value);
}

void CallbackControl::sub(double value) {
    set(m_value.value - value);
}

void CallbackControl::receiveUpdate(ControlUpdate update) {
    m_value = update;
    m_updates.write(&update, 1);
    emit(valueChanged(m_value.value));
    emit(valueChanged(m_value.value, m_value.time));
}

TrackWatcher::TrackWatcher() {
}

TrackWatcher::TrackWatcher(const TrackPointer& pTrack)
        : QObject(),
          m_pTrack(pTrack) {
    connect(m_pTrack.data(), SIGNAL(cuesUpdated()),
            this, SLOT(slotCuesUpdated()),
            Qt::DirectConnection);
    connect(m_pTrack.data(), SIGNAL(beatsUpdated()),
            this, SLOT(slotBeatsUpdated()),
            Qt::DirectConnection);
}

TrackWatcher::TrackWatcher(const TrackWatcher& watcher)
        : QObject() {
    m_pTrack = watcher.m_pTrack;
    connect(m_pTrack.data(), SIGNAL(cuesUpdated()),
            this, SLOT(slotCuesUpdated()),
            Qt::DirectConnection);
    connect(m_pTrack.data(), SIGNAL(beatsUpdated()),
            this, SLOT(slotBeatsUpdated()),
            Qt::DirectConnection);
}

TrackWatcher::~TrackWatcher() {
}

void TrackWatcher::slotCuesUpdated() {
    //qDebug() << "TrackWatcher::slotCuesUpdated()";
    TrackUpdate update;
    update.type = TrackUpdate::CUES_UPDATED;
    emit(trackUpdated(m_pTrack, update));
}

void TrackWatcher::slotBeatsUpdated() {
    //qDebug() << "TrackWatcher::slotBeatsUpdated()";
    TrackUpdate update;
    update.type = TrackUpdate::BEATS_UPDATED;
    emit(trackUpdated(m_pTrack, update));
}

CallbackTrackWatcher::CallbackTrackWatcher() {
}

CallbackTrackWatcher::~CallbackTrackWatcher() {
}

void CallbackTrackWatcher::receiveUpdate(TrackUpdate update) {
    switch (update.type) {
        case TrackUpdate::CUES_UPDATED:
            emit(cuesUpdated());
            break;
        case TrackUpdate::BEATS_UPDATED:
            emit(beatsUpdated());
            break;
        default:
            //qDebug() << "Unhandled TrackUpdate type in CallbackTrackWatcher::receiveUpdate";
            break;
    }
}

void CallbackTrackWatcher::watchTrack(const TrackPointer& pTrack) {
    emit(startWatchingTrack(pTrack, this));
}

void CallbackTrackWatcher::unwatchTrack(const TrackPointer& pTrack) {
    emit(stopWatchingTrack(pTrack, this));
}

CallbackTrackManager::CallbackTrackManager()
        : m_incomingUpdateFifo(4096) {
    qRegisterMetaType<TrackUpdate>("TrackUpdate");
}

CallbackTrackManager::~CallbackTrackManager() {
}

CallbackTrackWatcher* CallbackTrackManager::createTrackWatcher() {
    CallbackTrackWatcher* pWatcher = new CallbackTrackWatcher();
    connect(pWatcher, SIGNAL(startWatchingTrack(TrackPointer, CallbackTrackWatcher*)),
            this, SLOT(addTrackWatcher(TrackPointer, CallbackTrackWatcher*)),
            Qt::DirectConnection);
    connect(pWatcher, SIGNAL(stopWatchingTrack(TrackPointer, CallbackTrackWatcher*)),
            this, SLOT(removeTrackWatcher(TrackPointer, CallbackTrackWatcher*)),
            Qt::DirectConnection);
    return pWatcher;
}

void CallbackTrackManager::addTrackWatcher(const TrackPointer& pTrack,
                                           CallbackTrackWatcher* pTrackWatcher) {
    //qDebug() << "addTrackWatcher()" << pTrack << pTrackWatcher;
    QHash<TrackInfoObject*, TrackWatchers>::iterator it = m_trackWatchers.find(pTrack.data());
    if (it == m_trackWatchers.end()) {
        TrackWatchers watchers(pTrack);
        it = m_trackWatchers.insert(pTrack.data(), watchers);
        connect(&it.value().watcher, SIGNAL(trackUpdated(TrackPointer, TrackUpdate)),
                this, SLOT(slotTrackUpdated(TrackPointer, TrackUpdate)),
                Qt::DirectConnection);
    }
    it.value().callback_watchers.append(pTrackWatcher);
}

void CallbackTrackManager::removeTrackWatcher(const TrackPointer& pTrack,
                                              CallbackTrackWatcher* pTrackWatcher) {
    //qDebug() << "removeTrackWatcher()" << pTrack << pTrackWatcher;
    QHash<TrackInfoObject*, TrackWatchers>::iterator it =
            m_trackWatchers.find(pTrack.data());
    if (it != m_trackWatchers.end()) {
        it.value().callback_watchers.removeAll(pTrackWatcher);
    }
}

void CallbackTrackManager::slotTrackUpdated(const TrackPointer& pTrack,
                                            TrackUpdate update) {
    //qDebug() << "CallbackTrackManager::slotTrackUpdated()" << pTrack;
    if (!pTrack) {
        return;
    }

    QMutexLocker locker(&m_incomingUpdateMutex);
    CallbackTrackMessage message;
    message.track = pTrack.data();
    message.update = update;
    m_incomingUpdateFifo.writeBlocking(&message, 1);
}

void CallbackTrackManager::callbackProcessIncomingUpdates() {
    CallbackTrackMessage message;
    while (m_incomingUpdateFifo.read(&message, 1) == 1) {
        if (!message.track) {
            continue;
        }
        QHash<TrackInfoObject*, TrackWatchers>::iterator it =
                m_trackWatchers.find(message.track);
        if (it != m_trackWatchers.end()) {
            TrackWatchers& watchers = it.value();
            for (QList<CallbackTrackWatcher*>::iterator it2 = watchers.callback_watchers.begin();
                 it2 != watchers.callback_watchers.end(); ++it2) {
                (*it2)->receiveUpdate(message.update);
            }
        }
    }
}

CallbackControlManager::CallbackControlManager()
        : m_incomingUpdateFifo(4096),
          m_lastTimeValue(0.0),
          m_outgoingUpdateFifo(4096) {
    m_timer.start();
}

CallbackControlManager::~CallbackControlManager() {
}

CallbackControl* CallbackControlManager::addControl(ControlObject* pControl,
                                                    int bufferLength) {
    if (!pControl) {
        return NULL;
    }
    CallbackControl* pCallbackControl =
            m_callbackControls.value(pControl->getKey(), NULL);
    if (pCallbackControl) {
        return pCallbackControl;
    }

    pCallbackControl = new CallbackControl(
        this, pControl, bufferLength);
    m_callbackControls[pControl->getKey()] = pCallbackControl;
    return pCallbackControl;
}

CallbackControl* CallbackControlManager::getControl(ConfigKey key) {
    return m_callbackControls.value(key, NULL);
}

void CallbackControlManager::slotControlUpdated(ControlObject* pControl,
                                                double value) {
    if (!pControl) {
        return;
    }

    QMutexLocker locker(&m_incomingUpdateMutex);
    CallbackControlMessage message;
    message.control = pControl;
    message.update.value = value;
    message.update.time = getTime();
    // qDebug() << this << "slotControlUpdated()"
    //          << pControl->getKey().group
    //          << pControl->getKey().item
    //          << ":" << message.update.value
    //          << "at" << message.update.time;
    m_incomingUpdateFifo.writeBlocking(&message, 1);
}

void CallbackControlManager::slotControlUpdatedFromCallback(
    CallbackControl* pCallbackControl, double value) {
    m_updatedControls.insert(pCallbackControl->getKey());
}

void CallbackControlManager::callbackProcessIncomingUpdates() {
    CallbackControlMessage message;
    while (m_incomingUpdateFifo.read(&message, 1) == 1) {
        if (!message.control) {
            continue;
        }
        // qDebug() << this << "callbackProcessIncomingUpdates()"
        //          << message.control->getKey().group
        //          << message.control->getKey().item
        //          << ":" << message.update.value
        //          << "at" << message.update.time;
        CallbackControl* pCallbackControl =
                m_callbackControls.value(message.control->getKey(), NULL);
        if (!pCallbackControl) {
            continue;
        }
        pCallbackControl->receiveUpdate(message.update);
    }
}

void CallbackControlManager::callbackProcessOutgoingUpdates() {
    CallbackControlMessage message;
    foreach (ConfigKey key, m_updatedControls) {
        CallbackControl* pCallbackControl = m_callbackControls.value(key, NULL);
        if (!pCallbackControl) {
            continue;
        }
        message.control = pCallbackControl->getControl();
        message.update.value = pCallbackControl->get();
        // TODO(rryan) allow outgoing updates to have a timestamp?
        message.update.time = 0;

        // qDebug() << this << "callbackProcessOutgoingUpdates()"
        //          << message.control->getKey().group
        //          << message.control->getKey().item
        //          << ":" << message.update.value
        //          << "at" << message.update.time;

        if (m_outgoingUpdateFifo.write(&message, 1) != 1) {
            // TODO(XXX) log this condition. If the main Mixxx thread is hung it
            // won't be reading these updates. We can't block here though since
            // that would allow priority inversion.
        }
    }
    m_updatedControls.clear();
}

void CallbackControlManager::processOutgoingUpdates() {
    QMutexLocker locker(&m_incomingUpdateMutex);
    CallbackControlMessage message;
    while (m_outgoingUpdateFifo.read(&message, 1) == 1) {
        // qDebug() << this << "processOutgoingUpdates()"
        //          << message.control->getKey().group
        //          << message.control->getKey().item
        //          << ":" << message.update.value
        //         << "at" << message.update.time;
        message.control->set(message.update.value);
    }
}
