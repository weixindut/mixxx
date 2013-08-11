#include <QFileInfo>
#include "controllers/dlgpresetupload.h"
#include "controllers/controllerpresetinfo.h"
#include "controllers/dao/presetobjectdao.h"
#include "controllers/json.h"
using namespace QtJson;
DlgPresetUpload::DlgPresetUpload(QWidget* parent):QDialog(parent) {
    m_ui.setupUi(this);
    m_xmlFile = "";
    getUi().labelXMLName->setText("...");
    getUi().labelImageName->setText("...");
    getUi().labelJSName->setText("...");
    connect(getUi().btnXMLUpload, SIGNAL(clicked()),
            this, SLOT(slotSelectXMLFile()));
    connect(getUi().btnImageUpload, SIGNAL(clicked()),
            this, SLOT(sloSelectPICFile()));
    connect(getUi().btnJSUpload, SIGNAL(clicked()),
            this, SLOT(slotSelectJSFile()));
    connect(getUi().btnSubmit, SIGNAL(clicked()),
                this, SLOT(slotSubmit()));
    connect(getUi().btnCancel, SIGNAL(clicked()),
                this, SLOT(slotCancel()));
}
void DlgPresetUpload::slotSelectXMLFile() {
    QFileDialog dialog(this);
    dialog.setNameFilter( tr("XML (*.xml)") );
    dialog.setFileMode( QFileDialog::ExistingFile );
    dialog.setViewMode( QFileDialog::Detail );
    if(dialog.exec()==QFileDialog::Accepted) {
        QStringList files=dialog.selectedFiles();
        if(!files.isEmpty()) {
            m_xmlFile = files[0];
            qDebug()<< "file name============" + m_xmlFile;
            getUi().labelXMLName->setText(m_xmlFile);
        }
    }

}
void DlgPresetUpload::sloSelectPICFile() {
    QFileDialog dialog(this);
    dialog.setNameFilter( tr("PIC (*.png *.jpg *.gif)") );
    dialog.setFileMode( QFileDialog::ExistingFiles );
    dialog.setViewMode( QFileDialog::Detail );
    if(dialog.exec()==QFileDialog::Accepted) {
        m_picFiles=dialog.selectedFiles();
        QString showContent = "";
        foreach (QString file, m_picFiles) {
            QFileInfo info;
            info.setFile(file);
            showContent.append(info.fileName());
            showContent.append(";");
        }
        getUi().labelImageName->setText(showContent);
    }
}
void DlgPresetUpload::slotSelectJSFile() {
    QFileDialog dialog(this);
    dialog.setNameFilter( tr("JS (*.js)") );
    dialog.setFileMode( QFileDialog::ExistingFiles );
    dialog.setViewMode( QFileDialog::Detail );
    if(dialog.exec()==QFileDialog::Accepted) {
        m_jsFiles=dialog.selectedFiles();
        QString showContent = "";
        foreach (QString file, m_jsFiles) {
            QFileInfo info;
            info.setFile(file);
            showContent.append(info.fileName());
            showContent.append(";");
        }
        getUi().labelJSName->setText(showContent);
    }
}

void DlgPresetUpload::slotSubmit() {
    if(m_xmlFile.isEmpty()||
            m_picFiles.isEmpty()||
            m_jsFiles.isEmpty()) {
    	QString message = "Please make sure you select a file for every item!";
    	QMessageBox::information(this, tr("Info"), message);
    	return;
    } else {
        if(uploadCheck(m_xmlFile,m_picFiles,m_jsFiles)) {
            QString serverURL = "http://127.0.0.1:8000/upload";
            foreach(QString file, m_picFiles) {
                m_client.postFile(serverURL,file);
            }
            foreach(QString file, m_jsFiles) {
                m_client.postFile(serverURL,file);
            }
            QString reply = m_client.postFile(serverURL,m_xmlFile);
            qDebug() << "Print JsonObject:"+reply;
            bool ok;
            QVariantMap result = QtJson::parse(reply,ok).toMap();
            if(!ok) {
            	qDebug()<<"parse failed\n";
            }
            QString status = result["status"].toString();
            QString info = result["info"].toString();
            if (status=="true") {
                QString pid = info;
                PresetObjectDAO pod(m_db);
                bool ok = pod.insertOnePreset(pid,m_xmlFile);
                if (!ok) {
                    qDebug() << "preset local insert failed";
                    return;
                } else {
                // TODO(weixin): transfer files into a specified directory,
                // and save that directory
                    foreach(QString file, m_picFiles) {
                        if(!pod.insertOneFile(pid,file,0)) {
                        	// TODO(weixin): do something when some file is inserted failed
                            return;
                        }
                    }
                    foreach(QString file, m_jsFiles) {
                        if(!pod.insertOneFile(pid,file,2)) {
                        	// TODO(weixin): do something when some file is inserted failed
                            return;
                        }
                    }
                    pod.insertOneFile(pid,m_xmlFile,1);
                }
                QString message = "Success!";
                QMessageBox::information(this, tr("Info"), message);
                close();
                m_xmlFile = "";
                m_picFiles.clear();
                m_jsFiles.clear();

            } else if (status=="false") {
            	QMessageBox::information(this, tr("Info"), info);
            	return;
            } else {
            	QString message = "Something unexpected has happened!";
            	QMessageBox::information(this, tr("Info"), message);
            	return;
            }
    	}
    }
}
bool DlgPresetUpload::uploadCheck(QString xmlFile, QList<QString> picFiles, QList<QString> jsFiles) {
    qDebug()<<"===========uploadCheck()=========";
	PresetInfo presetInfo = PresetInfo(xmlFile);
    QList<QString> pictures = presetInfo.getPicFileNames();
    QList<QString> scripts = presetInfo.getJsFileNames();
    if (pictures.size()!=picFiles.size()||scripts.size()!=jsFiles.size()) {
    	QString message = "Please select correct pictures and scripts!";
    	QMessageBox::information(this, tr("Info"), message);
    	return false;
    }
    foreach(QString pic, picFiles) {
        QFileInfo info(pic);
        QString picName = info.fileName();
        if (!pictures.contains(picName)) {
        	qDebug()<<"===========pic:" + picName;
        	QString message = "Please select correct pictures!";
        	QMessageBox::information(this, tr("Info"), message);
        	return false;
        }
    }
    foreach(QString js, jsFiles) {
        QFileInfo info(js);
        QString jsName = info.fileName();
        if (!scripts.contains(jsName)) {
        	qDebug()<<"===========JS:" + jsName;
        	QString message = "Please select correct scripts!";
        	QMessageBox::information(this, tr("Info"), message);
            return false;
        }
    }
    return true;
}
void DlgPresetUpload::slotCancel() {
    close();
    m_xmlFile = "";
    m_picFiles.clear();
    m_jsFiles.clear();
}
