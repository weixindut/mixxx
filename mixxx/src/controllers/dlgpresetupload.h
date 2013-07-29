#ifndef DLGPRESETUPLOAD_H_
#define DLGPRESETUPLOAD_H_
#include "controllers/ui_dlgpresetupload.h"
class DlgPresetUpload : public QDialog {
    Q_OBJECT
  public:
    DlgPresetUpload(QWidget* parent);
    virtual ~DlgPresetUpload(){}
  private:
    Ui::DlgPresetUpload m_ui;
};
#endif
