#ifndef DLGCONTROLLERPRESET_H
#define DLGCONTROLLERPRESET_H
#include <QtGui>
#include <QHash>

#include "controllers/ui_dlgcontrollerpreset.h"
class DlgControllerPreset: public QWidget {
    Q_OBJECT
  public:
    DlgControllerPreset(QWidget* parent);
    //DlgControllerPreset(QWidget* parent, const DlgControllerPreset& controllerpreset);
    virtual ~DlgControllerPreset() {};
    void setCover(QString path);
    void setPresetName(QString name);
    void setSource(QString source);
    void setStatus(QString status);
    void setRatings(float ratings);
    void setPresetID(QString pid);
    bool isSelected();
    QString presetID();

  protected:
    Ui::DlgControllerPreset& getUi() {
        return m_ui;
    }
  private slots:
    void slotToSelect();
  private:
    QString m_pid;
    Ui::DlgControllerPreset m_ui;
};
#endif
