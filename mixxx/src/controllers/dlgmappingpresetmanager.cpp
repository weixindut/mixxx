/*
 * dlgmappingpresetmanager.cpp
 *
 *  Created on: 2013-6-20
 *      Author: Xin Wei
 */

#include <QtGui>
#include <QHash>
#include <QVariantMap>
#include <QVariantList>
#include <QMap>
#include "controllers/dlgmappingpresetmanager.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"
#include "controllers/wao/presetobjectwao.h"
#include "controllers/dlgcontrollerpreset.h"
using namespace QtJson;

DlgMappingPresetManager::DlgMappingPresetManager(QWidget* parent)
        : QDialog(parent) {
    m_ui.setupUi(this);
    getUi().tabWidget_results->setCurrentIndex(0);
    //getUi().label_localleft
    connect(getUi().btn_search, SIGNAL(clicked()),
               this, SLOT(slotSearch()));

}
void DlgMappingPresetManager::addDlgControllerPreset(QWidget w) {

}
void DlgMappingPresetManager::slotSearch() {
    int index=getUi().tabWidget_results->currentIndex();
    if (index==0) {
        emit(slotSearchLocal());
    } else {
        getUi().tabWidget_results->setCurrentIndex(1);
        emit(slotSearchCloud());
    }
	qDebug("=====slotSearch()===========%d",index);
}
void DlgMappingPresetManager::slotSearchLocal() {
	QString searchcontent=getUi().lineEdit_search->text();
}
void DlgMappingPresetManager::slotSearchCloud() {
	qDebug("=====slotSearchCloud()===========");
	QString searchcontent=getUi().lineEdit_search->text();
	PresetObjectWAO pow;
	QList<MidiControllerPreset> presetList;
	// TODO(wexin):fuzzy query
	presetList=pow.getPresetByPresetName(searchcontent);
	emit(slotShowCloudSearchResults(presetList));


}
void DlgMappingPresetManager::slotShowLocalSearchResults(QList<MidiControllerPreset> presets) {

}
void DlgMappingPresetManager::slotShowCloudSearchResults(QList<MidiControllerPreset> presets) {
	qDebug("=====slotShowCloudSearchResults()===========");
	foreach(MidiControllerPreset preset, presets) {
		qDebug("=====in loop===========");
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        getUi().gridLayout_cloudresults->addWidget(showpreset);
    }
}
void DlgMappingPresetManager::getJsonDataTest() {
	PresetObjectWAO pow;
    pow.getPresetByPresetName("Akai LPD8 - RK");
}
