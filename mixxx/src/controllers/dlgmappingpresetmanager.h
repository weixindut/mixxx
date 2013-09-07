#ifndef DLGMAPPINGPRESETMANAGER_H_
#define DLGMAPPINGPRESETMANAGER_H_
#include <QtGui>
#include <QHash>
#include <QList>
#include <QStackedLayout>
#include <QGridLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include "controllers/controllerpreset.h"
#include "controllers/ui_dlgmappingpresetmanagerdlg.h"
#include "controllers/midi/midicontrollerpreset.h"
#include "controllers/dao/presetobjectdao.h"
#include "configobject.h"

class DlgMappingPresetManager : public QDialog {
    Q_OBJECT
  public:
    DlgMappingPresetManager(QWidget* parent,ConfigObject<ConfigValue>* pConfig);
    virtual ~DlgMappingPresetManager();
    void getJsonDataTest();
    void searchLocal();
    void searchCloud();
    void showLocalSearchResults();
    void showCloudSearchResults();
    bool getSelectedPreset(QList<QGridLayout* > layoutList,
            QList<MidiControllerPreset> presetList);
  signals:
    void presetReturned(QString);
  protected:
    Ui::DlgPresetManagerDlg& getUi() {
        return m_ui;
    }

  private slots:
    void slotOk();
    void slotSearch();
    void slotSetApplyText(int index);
    void slotShowCloudLastPageResults();
    void slotShowCloudNextPageResults();
    void slotShowLocalLastPageResults();
    void slotShowLocalNextPageResults();

  private:
    Ui::DlgPresetManagerDlg m_ui;
    ConfigObject<ConfigValue>* m_pConfig;
    QSqlDatabase m_db;
    QList<MidiControllerPreset> m_presetListCloud;
    QList<MidiControllerPreset> m_presetListLocal;
    int m_currentCloudResultsPage;
    int m_currentLocalResultsPage;
    QList<QGridLayout* > m_gridLayoutListCloud;
    QList<QGridLayout* > m_gridLayoutListLocal;
    MidiControllerPreset m_selectedPreset;
    PresetObjectDAO m_presetObjectDAO;

};
#endif

