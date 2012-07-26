#include <QTimer>
#include <QDebug>
// #include <QFile>

#include "automount.h"

Automount::Automount(QWidget* parent)
         : QWidget(parent) {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this , SLOT(slotReadMtab()));
    timer->start(5000);//call every 5 seconds
}

Automount::~Automount(){
    
}

void Automount::slotReadMtab(){
    // QFile
    qDebug() << "called function";
}
