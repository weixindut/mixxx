#include <QFileInfo>
#include <QFile>
#include "controllers/dlgpresetupload.h"
#include "controllers/controllerpresetinfo.h"
#include "controllers/dao/presetobjectdao.h"
#include "controllers/wao/presetobjectwao.h"
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
                this, SLOT(slotClose()));
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
            QFileInfo info(m_xmlFile);
            qDebug()<< "file name============" + m_xmlFile;
            getUi().labelXMLName->setText(info.fileName());
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
                QString message = "Sorry!";
                QMessageBox::information(this, tr("Notice"), message);
                return;
            }
            QString status = result["status"].toString();
            QString info = result["info"].toString();
            if (status=="true") {
                PresetObjectDAO pod(m_db);
                if(pod.insertOnePreset(info,m_xmlFile,m_picFiles,m_jsFiles,"undefined")) {
                    transferPresetFiles(m_xmlFile,m_picFiles,m_jsFiles);
                    QString message = "Success!";
                    QMessageBox::information(this, tr("Info"), message);
                    slotClose();
                } else {
            	    QString message = "Sorry, maybe there's something wrong with database!";
            	    QMessageBox::information(this, tr("Info"), message);
            	    return;
            	}
            } else if (status=="false") {
                QMessageBox::information(this, tr("Info"), info);
                return;
            } else {
                QString message = "Sorry, something unexpected has happened in Server! Please try again later!";
                QMessageBox::information(this, tr("Info"), message);
                return;
            }
    	}
    }
}

bool DlgPresetUpload::uploadCheck(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles) {
    QString message;
    bool ok = true;
    PresetObjectWAO pow;
    QFileInfo fileinfo(xmlFile);
    QString xmlName = fileinfo.fileName();
    PresetInfo presetInfo = PresetInfo(xmlFile);
    QList<QString> pictures = presetInfo.getPicFileNames();
    QList<QString> scripts = presetInfo.getJsFileNames();
    if (pictures.size()!=picFiles.size()||scripts.size()!=jsFiles.size()) {
        message.append("You have selected more files or missed some files according"
                       " your mapping preset file"+xmlName+"!");
        ok = false;
    }
    if (!pow.checkAvailableFileName(xmlName)) {
        message.append("File with name "+xmlName+" has already existed in cloud server!");
        ok = false;
    }
    // check whether selected file name is same with info inserted in
    // xml file by the preset writer
    foreach(QString pic, picFiles) {
        QFileInfo info(pic);
        QString picName = info.fileName();
        if (!pictures.contains(picName)) {
            message.append("File "+picName+" does not match your mapping preset file!");
            ok = false;
        }
        if (!pow.checkAvailableFileName(picName)) {
            message.append("File with name "+picName+" has already existed in cloud server!");
            ok = false;
        }
    }
    foreach(QString js, jsFiles) {
        QFileInfo info(js);
        QString jsName = info.fileName();
        if (!scripts.contains(jsName)) {
            message.append("File "+jsName+" does not match your mapping preset file!");
            ok = false;
        }
        if (!pow.checkAvailableFileName(jsName)) {
            message.append("File with name "+jsName+" has already existed in cloud server!");
            ok = false;
        }
    }
    PresetObjectDAO pod(m_db);
    if(!pod.isPresetInsertable(xmlFile)) {
        message.append("Preset with same schema version has already existed in local database!");
        ok = false;
    }
    if(!ableToTransferPresetFiles(xmlFile,picFiles,jsFiles)) {
        return false;
    }
    if (ok == false) {
        QMessageBox::information(this, tr("Info"), message);
        return ok;
    }
    return ok;
}
bool DlgPresetUpload::ableToTransferPresetFiles(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles) {
    QString destDir = "./res/controllers/";
    QFileInfo xmlInfo(xmlFile);
    QString destXML = destDir+xmlInfo.fileName();
    QFile xml(destXML);
    if(xml.exists()) {
        QString message = "There already exists file "+xmlInfo.fileName()+" in directory "
                +destDir+", please change your preset name and try again:(";
        QMessageBox::information(this, tr("Info"), message);
        return false;
    }
    foreach(QString pic, picFiles) {
        QFileInfo picInfo(pic);
        QString destPic = destDir+picInfo.fileName();
        QFile pic(destPic);
        if (pic.exists()) {
            QString message = "There already exists file "+picInfo.fileName()+" in directory "
                    +destDir+", please change your preset name and try again:(";
            QMessageBox::information(this, tr("Info"), message);
            return false;
        }
    }
    foreach(QString js, jsFiles) {
        QFileInfo jsInfo(js);
        QString destJS = destDir+jsInfo.fileName();
        QFile js(destJS);
        if (js.exists()) {
            QString message = "There already exists file "+jsInfo.fileName()+" in directory "
                    +destDir+", please change your preset name and try again:(";
            QMessageBox::information(this, tr("Info"), message);
            return false;
        }
    }
    return true;
}

void DlgPresetUpload::transferPresetFiles(QString& xmlFile, QList<QString>& picFiles, QList<QString>& jsFiles) {
	QString destDir = "./res/controllers/";
    QFileInfo xml(xmlFile);
    QString destXML = destDir+xml.fileName();
    copyFile(xmlFile,destXML);
    xmlFile = destXML;
    for(int i=0; i<picFiles.size(); i++) {
        QFileInfo info(picFiles[i]);
        QString destPic = destDir+info.fileName();
        copyFile(picFiles[i],destPic);
        picFiles[i] = destPic;
    }
    for(int i=0; i<jsFiles.size(); i++) {
        QFileInfo info(jsFiles[i]);
        QString destJS = destDir+info.fileName();
        copyFile(jsFiles[i],destJS);
        jsFiles[i] = destJS;
    }
}

void DlgPresetUpload::slotClose() {
    close();
    m_xmlFile = "";
    m_picFiles.clear();
    m_jsFiles.clear();
    getUi().labelXMLName->setText("...");
    getUi().labelImageName->setText("...");
    getUi().labelJSName->setText("...");
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
    	QString message = "File "+destination+" has already existed, please rename your file";
    	QMessageBox::information(this, tr("Notice"), message);
        return false;
    }
    QFile::copy(source, destination);
    return true;
}

void DlgPresetUpload::closeEvent(QCloseEvent *event) {
	slotClose();
}
