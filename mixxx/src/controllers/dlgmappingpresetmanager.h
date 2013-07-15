#ifndef DLGMAPPINGPRESETMANAGER_H_
#define DLGMAPPINGPRESETMANAGER_H_
#include <QtGui>
#include <QHash>
#include <QList>
#include <QStackedLayout>
#include <QGridLayout>

#include "controllers/ui_dlgmappingpresetmanagerdlg.h"
#include "controllers/midi/midicontrollerpreset.h"

class DlgMappingPresetManager : public QDialog {
    Q_OBJECT
  public:
    DlgMappingPresetManager(QWidget* parent);
    virtual ~DlgMappingPresetManager();
    void getJsonDataTest();
  protected:
    Ui::DlgPresetManagerDlg& getUi() {
        return m_ui;
    }

  private slots:
    void slotSearch();
    void slotSearchLocal();
    void slotSearchCloud();
    void slotShowLocalSearchResults();
    void slotShowCloudSearchResults();
    void slotShowCloudLastPageResults();
    void slotShowCloudNextPageResults();
    void slotShowLocalLastPageResults();
    void slotShowLocalNextPageResults();

  private:
    Ui::DlgPresetManagerDlg m_ui;
    QList<MidiControllerPreset> m_presetListCloud;
    QList<MidiControllerPreset> m_presetListLocal;
    int m_currentCloudResultsPage;
    int m_currentLocalResultsPage;
    QList<QGridLayout* > m_gridLayoutListCloud;
};
#endif

