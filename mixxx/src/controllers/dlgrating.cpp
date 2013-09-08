#include <QIcon>
#include "controllers/dlgrating.h"
DlgRating::DlgRating(QWidget* parent, MidiControllerPreset& preset)
        :QDialog(parent),
         m_preset(preset) {
    m_ui.rbtnScore1->setIcon(QIcon("./res/images/controllers/1stars.png"));
    qDebug()<<preset.Pid();
}
