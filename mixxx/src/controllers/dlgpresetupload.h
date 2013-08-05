#ifndef DLGPRESETUPLOAD_H_
#define DLGPRESETUPLOAD_H_
#include <QtGui>
#include "httpclient.h"
#include "controllers/ui_dlgpresetupload.h"
class DlgPresetUpload : public QDialog {
    Q_OBJECT
  public:
    DlgPresetUpload(QWidget* parent);
    virtual ~DlgPresetUpload(){}
  protected:
    Ui::DlgPresetUpload& getUi() {
        return m_ui;
    }
  private slots:
    void slotSelectXMLFile();
    void sloSelectPICFile();
    void slotSelectJSFile();
    void slotSubmit();
    void slotCancel();
  private:
    Ui::DlgPresetUpload m_ui;
    HttpClient m_client;
    QString m_xmlFile;
    QString m_picFile;
    QString m_jsFile;
};
#endif
