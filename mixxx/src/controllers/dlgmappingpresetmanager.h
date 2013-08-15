#ifndef DLGMAPPINGPRESETMANAGER_H_
#define DLGMAPPINGPRESETMANAGER_H_
#include <QtGui>
#include <QHash>
#include <QList>
#include <QStackedLayout>
#include <QGridLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include "controllers/ui_dlgmappingpresetmanagerdlg.h"
#include "controllers/midi/midicontrollerpreset.h"
#include "configobject.h"

class DlgMappingPresetManager : public QDialog {
    Q_OBJECT
  public:
    DlgMappingPresetManager(QWidget* parent,ConfigObject<ConfigValue>* pConfig);
    virtual ~DlgMappingPresetManager();
    bool checkForTables();
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
    QSqlDatabase m_db;
    Ui::DlgPresetManagerDlg m_ui;
    QList<MidiControllerPreset> m_presetListCloud;
    QList<MidiControllerPreset> m_presetListLocal;
    int m_currentCloudResultsPage;
    int m_currentLocalResultsPage;
    QList<QGridLayout* > m_gridLayoutListCloud;
    QList<QGridLayout* > m_gridLayoutListLocal;
};
#endif
