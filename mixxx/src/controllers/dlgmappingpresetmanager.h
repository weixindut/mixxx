#ifndef DLGMAPPINGPRESETMANAGER_H_
#define DLGMAPPINGPRESETMANAGER_H_
#include <QtGui>
#include <QHash>

#include "controllers/ui_dlgmappingpresetmanagerdlg.h"
#include "controllers/midi/midicontrollerpreset.h"

class DlgMappingPresetManager : public QDialog {
    Q_OBJECT
  public:
    DlgMappingPresetManager(QWidget* parent);
    virtual ~DlgMappingPresetManager() {};
    void getJsonDataTest();
    void addDlgControllerPreset(QWidget w);
  protected:
    Ui::DlgPresetManagerDlg& getUi() {
        return m_ui;
    }
  //private signals:

  private slots:
    void slotSearch();
    void slotSearchLocal();
    void slotSearchCloud();
    void slotShowLocalSearchResults(QList<MidiControllerPreset> presets);
    void slotShowCloudSearchResults(QList<MidiControllerPreset> presets);
  private:
    Ui::DlgPresetManagerDlg m_ui;
    //QList<ControllerPreset>
};
#endif

