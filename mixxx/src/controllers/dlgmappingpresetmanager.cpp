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
    // TODO(weixin): the path may adjust
    QPixmap pixmapleft("./res/images/controllers/goleft.png");
    QPixmap pixmapright("./res/images/controllers/goright.png");
    getUi().label_localleft->setPixmap(pixmapleft);
    getUi().label_localright->setPixmap(pixmapright);
    getUi().label_cloudleft->setPixmap(pixmapleft);
    getUi().label_cloudright->setPixmap(pixmapright);
    connect(getUi().btn_search, SIGNAL(clicked()),
    		this, SLOT(slotSearch()));
    connect(getUi().label_localleft,SIGNAL(clicked()),
    		this, SLOT(slotShowLastPageResults()));
    connect(getUi().label_localright,SIGNAL(clicked()),
        		this, SLOT(slotShowNextPageResults()));
    connect(getUi().label_cloudleft,SIGNAL(clicked()),
    		this, SLOT(slotShowLastPageResults()));
    connect(getUi().label_cloudright,SIGNAL(clicked()),
        		this, SLOT(slotShowNextPageResults()));
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
	//presetList=pow.getPresetByPresetName(searchcontent);
	presetList=pow.getPresetByURL("http://127.0.0.1:8000/api/v1/midi/preset/?format=json");
	emit(slotShowCloudSearchResults(presetList));


}
void DlgMappingPresetManager::slotShowLocalSearchResults(QList<MidiControllerPreset> presets) {

}
void DlgMappingPresetManager::slotShowCloudSearchResults(QList<MidiControllerPreset> presets) {
	qDebug("=====slotShowCloudSearchResults()===========");
	int row=0;
	int column=0;
	foreach(MidiControllerPreset preset, presets) {
		qDebug("=====in loop===========");
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        showpreset->setCover(preset.picturePath());
        showpreset->setPresetName(preset.name());
        showpreset->setSource(preset.presetSource());
        showpreset->setStatus(preset.presetStatus());
        showpreset->setRatings(preset.Ratings());
        getUi().gridLayout_cloudresults->addWidget(showpreset,row,++column);
    }
}
void DlgMappingPresetManager::slotShowNextPageResults() {

}
void DlgMappingPresetManager::slotShowLastPageResults() {

}
void DlgMappingPresetManager::getJsonDataTest() {
	PresetObjectWAO pow;
    pow.getPresetByPresetName("Akai LPD8 - RK");
}
