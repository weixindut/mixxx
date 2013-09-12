#include <QIcon>
#include <QDebug>
#include <QByteArray>
#include "controllers/dlgrating.h"
#include "httpclient.h"

DlgRating::DlgRating(QWidget* parent, QString pid)
        :QDialog(parent),
         m_pid(pid){
    m_ui.setupUi(this);
    m_buttonGroup = new QButtonGroup;

    m_ui.rbtnScore1->setIcon(QIcon("./res/images/controllers/1stars.png"));
    m_ui.rbtnScore2->setIcon(QIcon("./res/images/controllers/2stars.png"));
    m_ui.rbtnScore3->setIcon(QIcon("./res/images/controllers/3stars.png"));
    m_ui.rbtnScore4->setIcon(QIcon("./res/images/controllers/4stars.png"));
    m_ui.rbtnScore5->setIcon(QIcon("./res/images/controllers/5stars.png"));
    m_buttonGroup->addButton( m_ui.rbtnScore1, 1 );
    m_buttonGroup->addButton( m_ui.rbtnScore2, 2 );
    m_buttonGroup->addButton( m_ui.rbtnScore3, 3 );
    m_buttonGroup->addButton( m_ui.rbtnScore4, 4 );
    m_buttonGroup->addButton( m_ui.rbtnScore5, 5 );
    connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(slotRating()));
    connect(m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}
void DlgRating::slotRating() {
	int id = m_buttonGroup->checkedId();
	if (id == 1) {
		qDebug()<<"one star only!";
	}
	QString message = "Thank you!";
	QMessageBox::information(this, tr("Info"),message);
	HttpClient hc;
	QString url = "http://127.0.0.1:8000/api/v1/comment/rating/?format=json";
	QMap<QString, QString> postData;
	postData.insert("rating",QString::number(id));
	postData.insert("pid",m_pid);
	hc.post(url,postData);
}
