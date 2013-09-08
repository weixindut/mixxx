#ifndef DLGRATING_H
#define DLGRATING_H
#include <QtGui>
#include "controllers/ui_dlgrating.h"
#include "controllers/midi/midicontrollerpreset.h"
#include "controllers/wao/presetobjectwao.h"
class DlgRating: public QDialog {
    Q_OBJECT
  public:
    DlgRating(QWidget* parent, MidiControllerPreset& preset);
    virtual ~DlgRating(){};
  private:
    Ui::DlgRatingDlg m_ui;
    MidiControllerPreset& m_preset;
};
#endif
