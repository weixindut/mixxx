#ifndef DLGCONTROLLERPRESET_H
#define DLGCONTROLLERPRESET_H
#include <QtGui>
#include <QHash>

#include "controllers/ui_dlgcontrollerpreset.h"
class DlgControllerPreset: public QWidget {
    Q_OBJECT
  public:
    DlgControllerPreset(QWidget* parent);
    virtual ~DlgControllerPreset() {};
    void setCover(QString path);
    void setPresetName(QString name);
    void setSource(QString source);
    void setStatus(QString status);
    void setRatings(float ratings);

  protected:
    Ui::DlgControllerPreset& getUi() {
        return m_ui;
    }
  private:
    Ui::DlgControllerPreset m_ui;
};
#endif
