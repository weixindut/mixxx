#ifndef AUTOMOUNT_H
#define AUTOMOUNT_H

#include <QWidget>
#include <QTimer>
#include <QList>

// This class will periodically check if there are new storage devices connected
// and inform the main app if this happens

class Automount : public QWidget {
  Q_OBJECT
  public:
    Automount(QWidget *parent =0);
    ~Automount();
  signals:
    void foundNewStorage(QString);
    void removedStorage(QString);
  private slots:
    void slotReadMtab();
  private:
    QList<QString> parseMtab();
    void removedDevice(QList<QString> devices);
    void addedDevice(QList<QString> devices);
  
    QTimer m_timer;
    QList<QString> m_devices;
    // QStringList m_filesystems;
};

#endif // AUTOMOUNT_H
