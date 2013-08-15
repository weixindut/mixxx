#include <QFileInfo>
#include <QFile>
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
    if(m_xmlFile.isEmpty()) {
    	QString message = "Please make sure that you have selected a xml mapping preset file!";
    	QMessageBox::information(this, tr("Info"), message);
    	return;
    } else {
        if(uploadCheck(m_xmlFile,m_picFiles,m_jsFiles)) {
        	qDebug()<<"~~~~~~~~~~~"+m_xmlFile+"~~~~~~~~~";
            QString serverURL = "http://127.0.0.1:8000/upload";
            foreach(QString file, m_picFiles) {
            	qDebug()<<"~~~~~~~~~~~"+file+"~~~~~~~~~";
                m_client.postFile(serverURL,file);
            }
            foreach(QString file, m_jsFiles) {
            	qDebug()<<"~~~~~~~~~~~"+file+"~~~~~~~~~";
                m_client.postFile(serverURL,file);
            }
            QString reply = m_client.postFile(serverURL,m_xmlFile);
            qDebug() << "Print JsonObject:"+reply;
            bool ok;
            QVariantMap result = QtJson::parse(reply,ok).toMap();
            if(!ok) {
            	qDebug()<<"parse failed\n";
                QString message = "Sorry!";
                QMessageBox::information(this, tr("Notice"), message);
                return;
            }
            QString status = result["status"].toString();
            QString info = result["info"].toString();
            if (status=="true") {
                QString pid = info;
                PresetObjectDAO pod(m_db);
                bool ok = pod.insertOnePreset(pid,m_xmlFile);
                if (!ok) {
                    qDebug() << "preset local insert failed";
                    QString message = "Sorry!";
                    QMessageBox::information(this, tr("Notice"), message);
                    return;
                } else {
                // TODO(weixin): transfer files into a specified directory,
                // and save that directory
                    foreach(QString file, m_picFiles) {
                        if(!pod.insertOneFile(pid,file,0)) {
                            QString message = "Pictures upload failed, try to rename the relevant "
                                              "picture files' names and preset relevant code, and "
                                              "then try to submit again!";
                            QMessageBox::information(this, tr("Notice"), message);
                            return;
                        }
                    }
                    foreach(QString file, m_jsFiles) {
                        if(!pod.insertOneFile(pid,file,2)) {
                            QString message = "Scripts upload failed, try to rename the relevant "
                                              "JS files' names and preset relevant code, and "
                                              "then try to submit again!";
                            QMessageBox::information(this, tr("Notice"), message);
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
bool DlgPresetUpload::uploadCheck(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles) {
    qDebug()<<"===========uploadCheck()=========";
	PresetInfo presetInfo = PresetInfo(xmlFile);
    QList<QString> pictures = presetInfo.getPicFileNames();
    QList<QString> scripts = presetInfo.getJsFileNames();
    if (pictures.size()!=picFiles.size()||scripts.size()!=jsFiles.size()) {
    	QString message = "Please select correct pictures and scripts!";
    	QMessageBox::information(this, tr("Info"), message);
    	return false;
    }
    // check whether selected file name is same with info inserted in
    // xml file by the preset writer
    foreach(QString pic, picFiles) {
        QFileInfo info(pic);
        QString picName = info.fileName();
        if (!pictures.contains(picName)) {
        	qDebug()<<"===========PIC:" + picName;
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
    return transferPresetFiles(xmlFile,picFiles,jsFiles);
}
bool DlgPresetUpload::transferPresetFiles(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles) {
    bool status = true;
	QString destDir = "./res/controllers/";
    QFileInfo xml(xmlFile);
    QString destXML = destDir+xml.fileName();
    if (!copyFile(xmlFile,destXML)) {
    	status = false;
    	QString message = "There already exists file "+xml.fileName()+" in directory "
    	        +destDir+", please change your preset name and try again:(";
    	QMessageBox::information(this, tr("Info"), message);
    	return status;
    }
    foreach(QString pic, picFiles) {
        QFileInfo info(pic);
        QString destPic = destDir+info.fileName();
        if (!copyFile(pic,destPic)) {
            status = false;
        	QString message = "There already exists file "+info.fileName()+" in directory "
        	        +destDir+", please change your preset name and try again:(";
        	QMessageBox::information(this, tr("Info"), message);
        }
    }
    foreach(QString js, jsFiles) {
        QFileInfo info(js);
        QString destJS = destDir+info.fileName();
        if (!copyFile(js,destJS)) {
        	status = false;
        	QString message = "There already exists file "+info.fileName()+" in directory "
        	        +destDir+", please change your preset name and try again:(";
        	QMessageBox::information(this, tr("Info"), message);
        }
    }
    if(status == false) {
        removeFile(destXML);
        foreach(QString pic, picFiles) {
        	QFileInfo info(pic);
        	QString destPic = destDir+info.fileName();
        	removeFile(destPic);
        }
        foreach(QString js, jsFiles) {
        	QFileInfo info(js);
        	QString destJS = destDir+info.fileName();
        	removeFile(destJS);
        }
        return false;
    } else {
    	xmlFile = destXML;
    	for(int i=0; i<picFiles.size(); i++) {
    		QFileInfo info(picFiles[i]);
    		QString destPic = destDir+info.fileName();
    		picFiles[i] = destPic;
    	}
        for(int i=0; i<jsFiles.size(); i++) {
            QFileInfo info(jsFiles[i]);
            QString destJS = destDir+info.fileName();
            jsFiles[i] = destJS;
        }
        return true;
    }
}
void DlgPresetUpload::slotCancel() {
    close();
    m_xmlFile = "";
    m_picFiles.clear();
    m_jsFiles.clear();
}
bool DlgPresetUpload::copyFile(QString source, QString destination) {
	if (source == destination) {
		return true;
	}
    if (!QFile::exists(source)) {
        qDebug() << "copyFile:Given path does not exist!Path:"+source;
        return false;
    }
    if (QFile::exists(destination)) {
    	QString message = "File "+destination+" has been existed, please rename your file";
    	QMessageBox::information(this, tr("Notice"), message);
        return false;
    }
    QFile::copy(source, destination);
    return true;
}
bool DlgPresetUpload::removeFile(QString path) {
    if(QFile::exists(path)) {
    	QFile::remove(path);
    	return true;
    } else {
    	qDebug() << "removeFile:Given path does not exist!Path:"+path;
        return false;
    }
}