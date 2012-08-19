#ifndef MOUNTWATCHER_H
#define MOUNTWATCHER_H

#include <QtCore>
#include <QFileSystemWatcher>

// This class will periodically check if there are new storage devices connected
// and inform the main app if this happens

class MountWatcher : public QObject {
  Q_OBJECT
  public:
    MountWatcher(QObject *parent =0);
    virtual ~MountWatcher();
    QStringList attachedDevices();
  signals:
    void foundNewStorage(QStringList);
    void removedStorage(QStringList);
  private slots:
    void slotReadMtab();
    void slotDirectoryChanged(const QString&);
  private:
    void removedDevice(QStringList devices);
    void addedDevice(QStringList devices);

#if defined(__LINUX__)
    QTimer m_timer;
#elif defined(__APPLE__)
    QFileSystemWatcher m_watcher;
#endif
    QStringList m_devices;
};

#endif // MOUNTWATCHER_H
