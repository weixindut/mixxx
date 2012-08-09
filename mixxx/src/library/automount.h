#ifndef AUTOMOUNT_H
#define AUTOMOUNT_H

#include <QtCore>

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
  private:
    void removedDevice(QStringList devices);
    void addedDevice(QStringList devices);

    QTimer m_timer;
    QStringList m_devices;
};

#endif // AUTOMOUNT_H
