#include "controllers/dlgpresetupload.h"
DlgPresetUpload::DlgPresetUpload(QWidget* parent):QDialog(parent) {
    m_ui.setupUi(this);
    m_xmlFile = "";
    m_picFile = "";
    m_jsFile = "";
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
    dialog.setFileMode( QFileDialog::ExistingFile );
    dialog.setViewMode( QFileDialog::Detail );
    if(dialog.exec()==QFileDialog::Accepted) {
        QStringList files=dialog.selectedFiles();
        if(!files.isEmpty()) {
        	m_picFile = files[0];
            qDebug()<< "file name============" + m_picFile;
            getUi().labelImageName->setText(m_picFile);
        }
    }
}
void DlgPresetUpload::slotSelectJSFile() {
    QFileDialog dialog(this);
    dialog.setNameFilter( tr("JS (*.js)") );
    dialog.setFileMode( QFileDialog::ExistingFile );
    dialog.setViewMode( QFileDialog::Detail );
    if(dialog.exec()==QFileDialog::Accepted) {
        QStringList files=dialog.selectedFiles();
        if(!files.isEmpty()) {
        	m_jsFile = files[0];
            qDebug()<< "file name============" + m_jsFile;
            getUi().labelJSName->setText(m_jsFile);
        }
    }
}

void DlgPresetUpload::slotSubmit() {
    if(m_xmlFile.isEmpty()||
            m_picFile.isEmpty()||
            m_jsFile.isEmpty()) {
    	QString message = "Please make sure you select a file for every item!";
    	//QMessageBox::StandardButton reply;
    	QMessageBox::information(this, tr("Info"), message);
    	return;
    } else {
        // TODO(weixin):xml check!
    	if(uploadCheck(m_xmlFile,m_picFile,m_jsFile)) {
        	QString serverURL = "http://127.0.0.1:8000/upload";
        	m_client.postFile(serverURL,m_picFile);
        	m_client.postFile(serverURL,m_jsFile);
        	QString reply = m_client.postFile(serverURL,m_xmlFile);
        	// TODO(weixin): check reply data
        	// {status:true,info:pidvalue}
        	// {status:false, info:reason}
        	// reason one: xml file is not according the rules
        	// reason two: preset name already exists

    	}

    }
}

void DlgPresetUpload::slotCancel() {
    close();
    m_xmlFile = "";
    m_picFile = "";
    m_jsFile = "";
}
