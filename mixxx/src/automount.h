#ifndef AUTOMOUNT_H
#define AUTOMOUNT_H

#include <QWidget>

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
};

#endif // AUTOMOUNT_H
