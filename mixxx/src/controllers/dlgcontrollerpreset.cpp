#include <QDebug>
#include "controllers/dlgcontrollerpreset.h"

DlgControllerPreset::DlgControllerPreset(QWidget* parent)
        :QWidget(parent) {
	m_ui.setupUi(this);
}
void DlgControllerPreset::setCover(QString path) {
	QPixmap pixmap(path);
	getUi().label_cover->setPixmap(pixmap);
}
void DlgControllerPreset::setPresetName(QString name) {
	getUi().label_name->setText(name);
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

}
void DlgControllerPreset::setRatings(float ratings) {
	int value = qRound(ratings);
	if (value == 1) {
		QPixmap pixmap("./res/images/controllers/1starts.png");
		getUi().label_mark->setPixmap(pixmap);
	} else if (value == 2) {
	    QPixmap pixmap("./res/images/controllers/2starts.png");
	    getUi().label_mark->setPixmap(pixmap);
	} else if (value == 3) {
		QPixmap pixmap("./res/images/controllers/3starts.png");
		getUi().label_mark->setPixmap(pixmap);
	} else if (value == 4) {
		QPixmap pixmap("./res/images/controllers/4starts.png");
		getUi().label_mark->setPixmap(pixmap);
	} else if (value == 5) {
		QPixmap pixmap("./res/images/controllers/5fullstarts.png");
		getUi().label_mark->setPixmap(pixmap);
	} else {
	    qDebug("Not support this rating:%d\n",value);
	}
}
