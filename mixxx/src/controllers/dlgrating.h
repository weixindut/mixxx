#ifndef DLGRATING_H_
#define DLGRATING_H_
#include <QtGui>
#include <QButtonGroup>
#include "controllers/ui_dlgrating.h"

class DlgRating: public QDialog {
    Q_OBJECT
  public:
    DlgRating(QWidget* parent, QString pid);
    virtual ~DlgRating(){delete m_buttonGroup;};
  public slots:
    void slotRating();
  private:
    Ui::DlgRatingDlg m_ui;
    QString m_pid;
    QButtonGroup* m_buttonGroup;
};
#endif
