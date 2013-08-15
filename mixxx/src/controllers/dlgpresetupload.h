#ifndef DLGPRESETUPLOAD_H_
#define DLGPRESETUPLOAD_H_
#include <QtGui>
#include <QSqlDatabase>
#include <QSqlError>
#include <QList>
#include "controllers/httpclient.h"
#include "controllers/ui_dlgpresetupload.h"
class DlgPresetUpload : public QDialog {
    Q_OBJECT
  public:
    DlgPresetUpload(QWidget* parent);
    virtual ~DlgPresetUpload(){}
    // check whether the names of picure files and script files in the xml file are
    // same with its real file names. this is used for a based check before the files
    // are uploaded
    bool uploadCheck(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles);
    bool copyFile(QString path, QString destination);
    bool removeFile(QString path);
    bool transferPresetFiles(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles);
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
    QList<QString> m_picFiles;
    QList<QString> m_jsFiles;
    QSqlDatabase m_db;
};
#endif
