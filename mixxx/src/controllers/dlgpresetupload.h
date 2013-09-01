#ifndef DLGPRESETUPLOAD_H_
#define DLGPRESETUPLOAD_H_
#include <QtGui>
#include <QSqlDatabase>
#include <QSqlError>
#include <QList>
#include <QCloseEvent>
#include "controllers/httpclient.h"
#include "controllers/ui_dlgpresetupload.h"
class DlgPresetUpload : public QDialog {
    Q_OBJECT
  public:
    DlgPresetUpload(QWidget* parent);
    virtual ~DlgPresetUpload(){}
    // To check whether the names of picure files and script files in the xml file are
    // same with its real file names. To check whether new preset data can insert into
    // database. To check whether new preset files can be transfered to local preset lib
    // directory. This is used for a full check before the files are uploaded
    bool uploadCheck(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles);
    // Used for copying one file from one directory to another.
    bool copyFile(QString path, QString destination);
    // Used for copying new preset files to local preset lib directory.
    void transferPresetFiles(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles);
    // To check whether preset files can be transfered to preset lib directory.
    bool ableToTransferPresetFiles(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles);
    void closeEvent(QCloseEvent *event);
  protected:
    Ui::DlgPresetUpload& getUi() {
        return m_ui;
    }
  private slots:
    void slotSelectXMLFile();
    void sloSelectPICFile();
    void slotSelectJSFile();
    void slotSubmit();
    void slotClose();
  private:
    Ui::DlgPresetUpload m_ui;
    HttpClient m_client;
    QString m_xmlFile;
    QList<QString> m_picFiles;
    QList<QString> m_jsFiles;
    QSqlDatabase m_db;
};
#endif
