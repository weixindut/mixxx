#ifndef AUTOMOUNT_H
#define AUTOMOUNT_H

#include <QtCore>
#include <QFileSystemWatcher>

// This class will periodically check if there are new storage devices connected
// and inform the main app if this happens

class Automount : public QObject {
  Q_OBJECT
  public:
    Automount(QObject *parent =0);
    ~Automount();
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

#if defined(__LINUX)
    QTimer m_timer;
#elif defined(__APPLE__)
	QFileSystemWatcher m_watcher;
#endif
    QStringList m_devices;
};

#endif // AUTOMOUNT_H
