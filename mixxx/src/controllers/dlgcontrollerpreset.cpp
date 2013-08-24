#include <QDebug>
#include <QFont>
#include "controllers/dlgcontrollerpreset.h"

DlgControllerPreset::DlgControllerPreset(QWidget* parent)
        :QWidget(parent) {
    m_ui.setupUi(this);
    connect(getUi().checkBox,SIGNAL(clicked()),this,SLOT(slotToSelect()));
}
void DlgControllerPreset::setPresetID(QString pid){
    m_pid = pid;
}
void DlgControllerPreset::setCover(QString path) {
    QPixmap pixmap(path);
    getUi().label_cover->setPixmap(pixmap);
}

void DlgControllerPreset::setPresetName(QString name) {
    getUi().label_name->setText(name);
    QFont ft;
    ft.setBold(true);
    getUi().label_name->setFont(ft);
    getUi().label_name->setGeometry(QRect(10, 100, 140, 20*2));
    getUi().label_name->setWordWrap(true);
    getUi().label_name->setAlignment(Qt::AlignTop|Qt::AlignVCenter);
}

void DlgControllerPreset::setSource(QString source) {
    if(source == "forum") {
        QPixmap pixmap("./res/images/controllers/forum.png");
        getUi().label_source->setPixmap(pixmap);
    } else if(source == "mixxx") {
        QPixmap pixmap("./res/images/controllers/logo_mixxx.png");
        getUi().label_source->setPixmap(pixmap);
    } else {
        qDebug() <<"Source string is neither forum nor mixxx, but"+source +"\n";
        QPixmap pixmap("./res/images/controllers/forum.png");
        getUi().label_source->setPixmap(pixmap);
    }
}

void DlgControllerPreset::setStatus(QString status) {
    if(status == "pass") {
        QPixmap pixmap("./res/images/controllers/right.png");
        getUi().label_certificationsign->setPixmap(pixmap);
        getUi().label_certification->setText("certificated");
    } else if(status == "failed") {
        QPixmap pixmap("./res/images/controllers/alert.png");
        getUi().label_certificationsign->setPixmap(pixmap);
        getUi().label_certification->setText("uncertificated");
    } else if(status == "undefined") {
        QPixmap pixmap("./res/images/controllers/question.png");
        getUi().label_certificationsign->setPixmap(pixmap);
        getUi().label_certification->setText("certificating");
    } else {
        qDebug() <<"Wrong status string:"+status +"\n";
    }
    QFont ft;
    ft.setPointSize(10);
    getUi().label_certification->setFont(ft);
    getUi().label_certification->adjustSize();
}

void DlgControllerPreset::setRatings(float ratings) {
    int value = qRound(ratings);
    if (value == 1) {
        QPixmap pixmap("./res/images/controllers/1stars.png");
        getUi().label_mark->setPixmap(pixmap);
    } else if (value == 2) {
        QPixmap pixmap("./res/images/controllers/2stars.png");
        getUi().label_mark->setPixmap(pixmap);
    } else if (value == 3) {
        QPixmap pixmap("./res/images/controllers/3stars.png");
        getUi().label_mark->setPixmap(pixmap);
    } else if (value == 4) {
        QPixmap pixmap("./res/images/controllers/4stars.png");
        getUi().label_mark->setPixmap(pixmap);
    } else if (value == 5) {
        QPixmap pixmap("./res/images/controllers/5stars.png");
        getUi().label_mark->setPixmap(pixmap);
    } else {
        qDebug("Not support this rating:%d\n",value);
    }
}
void DlgControllerPreset::slotToSelect(){
	if(getUi().checkBox->isChecked()) {
		getUi().frame->setAutoFillBackground(true);
	} else {
		getUi().frame->setAutoFillBackground(false);
	}

}
bool DlgControllerPreset::checkBoxStatus() {
	return getUi().checkBox->isChecked();
}
QString DlgControllerPreset::presetID() {
    return m_pid;
}
