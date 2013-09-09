#include <QIcon>
#include <QDebug>

#include "controllers/dlgrating.h"

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
    m_buttonGroup->addButton( m_ui.rbtnScore1, 0 );
    m_buttonGroup->addButton( m_ui.rbtnScore2, 0 );
    m_buttonGroup->addButton( m_ui.rbtnScore3, 0 );
    m_buttonGroup->addButton( m_ui.rbtnScore4, 0 );
    m_buttonGroup->addButton( m_ui.rbtnScore5, 0 );
    connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(slotRating()));
    connect(m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}
void DlgRating::slotRating() {
	int id = m_buttonGroup->checkedId();
    printf("hhhhhhhhhhhhhhh%d", id);
}
