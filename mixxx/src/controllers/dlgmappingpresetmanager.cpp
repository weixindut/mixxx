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
    m_currentCloudResultsPage=0;
    m_currentLocalResultsPage=0;
    // TODO(weixin): the path may adjust
    QIcon icon;
    QPixmap pixmapleft("./res/images/controllers/goleft.png");
    QPixmap pixmapright("./res/images/controllers/goright.png");
    icon.addPixmap(pixmapleft);
    getUi().btn_localleft->setIcon(icon);
    getUi().btn_localleft->setIconSize(QSize(32,32));
    getUi().btn_localleft->setFixedSize(pixmapleft.size());
    getUi().btn_localleft->setMask(pixmapleft.mask());
    getUi().btn_localleft->setEnabled(false);

    getUi().btn_cloudleft->setIcon(icon);
    getUi().btn_cloudleft->setIconSize(QSize(32,32));
    getUi().btn_cloudleft->setFixedSize(pixmapleft.size());
    getUi().btn_cloudleft->setMask(pixmapleft.mask());
    getUi().btn_cloudleft->setEnabled(false);

    icon.addPixmap(pixmapright);
    getUi().btn_localright->setIcon(icon);
    getUi().btn_localright->setIconSize(QSize(32,32));
    getUi().btn_localright->setFixedSize(pixmapright.size());
    getUi().btn_localright->setMask(pixmapright.mask());
    getUi().btn_localright->setEnabled(false);

    getUi().btn_cloudright->setIcon(icon);
    getUi().btn_cloudright->setIconSize(QSize(32,32));
    getUi().btn_cloudright->setFixedSize(pixmapright.size());
    getUi().btn_cloudright->setMask(pixmapright.mask());
    getUi().btn_cloudright->setEnabled(false);

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
	// TODO(wexin):fuzzy query
	//presetList=pow.getPresetByPresetName(searchcontent);
	m_presetListCloud=pow.getPresetByURL("http://127.0.0.1:8000/api/v1/midi/preset/?format=json");
	emit(slotShowCloudSearchResults());


}
void DlgMappingPresetManager::slotShowLocalSearchResults() {

}
void DlgMappingPresetManager::slotShowCloudSearchResults() {
    qDebug("=====slotShowCloudSearchResults()===========");
    connect(getUi().btn_cloudleft,SIGNAL(clicked()),
            this, SLOT(slotShowCloudLastPageResults()));
    connect(getUi().btn_cloudright,SIGNAL(clicked()),
            this, SLOT(slotShowCloudNextPageResults()));

    for(int i=0; i<getUi().gridLayout_cloudresults->count();i++) {
    	qDebug("=====removeItem()%d===========",i);
        getUi().gridLayout_cloudresults->removeItem(getUi().gridLayout_cloudresults->itemAt(i));
    }
    int num;
    if(m_presetListCloud.size()>8) {
        num=8;
        getUi().btn_cloudright->setEnabled(true);
    } else {
        num=m_presetListCloud.size();
        getUi().btn_cloudright->setEnabled(false);
    }
    int row=0;
    int column=0;
    for(int i=0;i<num;i++) {
    	if(i==4) {
    		row++;
    		column=0;
    	}
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        showpreset->setCover(m_presetListCloud[i].picturePath());
        showpreset->setPresetName(m_presetListCloud[i].name());
        showpreset->setSource(m_presetListCloud[i].presetSource());
        showpreset->setStatus(m_presetListCloud[i].presetStatus());
        showpreset->setRatings(m_presetListCloud[i].Ratings());
        getUi().gridLayout_cloudresults->addWidget(showpreset,row,++column);
    }
}
void DlgMappingPresetManager::slotShowCloudNextPageResults() {
	qDebug()<<"============slotShowCloudNextPageResults===========";
    m_currentCloudResultsPage++;
    if(m_currentCloudResultsPage <= 0) {
        getUi().btn_cloudleft->setEnabled(false);
    } else {
    	getUi().btn_cloudleft->setEnabled(true);
    }
    if((m_currentCloudResultsPage+1)*8 >= m_presetListCloud.size()) {
        getUi().btn_cloudright->setEnabled(false);
    } else {
        getUi().btn_cloudright->setEnabled(true);
    }
    int num;
    if((m_currentCloudResultsPage+1)*8>m_presetListCloud.size()) {
        num=m_presetListCloud.size();
    } else {
        num=(m_currentCloudResultsPage+1)*8;
    }
    for(int i=0; i<getUi().gridLayout_cloudresults->count();i++) {
    	qDebug("=====removeItem()%d===========",i);
        getUi().gridLayout_cloudresults->removeItem(getUi().gridLayout_cloudresults->itemAt(i));
    }
    int row=0;
    int column=0;
    for(int i=m_currentCloudResultsPage*8;i<num;i++) {
        if(i==m_currentCloudResultsPage*8+4) {
            row++;
            column=0;
        }
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        showpreset->setCover(m_presetListCloud[i].picturePath());
        showpreset->setPresetName(m_presetListCloud[i].name());
        showpreset->setSource(m_presetListCloud[i].presetSource());
        showpreset->setStatus(m_presetListCloud[i].presetStatus());
        showpreset->setRatings(m_presetListCloud[i].Ratings());
        getUi().gridLayout_cloudresults->addWidget(showpreset,row,++column);
    }
}
void DlgMappingPresetManager::slotShowCloudLastPageResults() {
	qDebug()<<"============slotShowCloudLastPageResults===========";
    m_currentCloudResultsPage--;
    if(m_currentCloudResultsPage <= 0) {
        getUi().btn_cloudleft->setEnabled(false);
    } else {
    	getUi().btn_cloudleft->setEnabled(true);
    }
    if((m_currentCloudResultsPage+1)*8 >= m_presetListCloud.size()) {
        getUi().btn_cloudright->setEnabled(false);
    } else {
        getUi().btn_cloudright->setEnabled(true);
    }

    for(int i=0; i<getUi().gridLayout_cloudresults->count();i++) {
    	qDebug("=====removeItem()%d===========",i);
        getUi().gridLayout_cloudresults->removeItem(getUi().gridLayout_cloudresults->itemAt(i));
    }
    int row=0;
    int column=0;
    for(int i=m_currentCloudResultsPage*8;i<(m_currentCloudResultsPage+1)*8;i++) {
        if(i==m_currentCloudResultsPage*8+4) {
            row++;
            column=0;
        }
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        showpreset->setCover(m_presetListCloud[i].picturePath());
        showpreset->setPresetName(m_presetListCloud[i].name());
        showpreset->setSource(m_presetListCloud[i].presetSource());
        showpreset->setStatus(m_presetListCloud[i].presetStatus());
        showpreset->setRatings(m_presetListCloud[i].Ratings());
        getUi().gridLayout_cloudresults->addWidget(showpreset,row,++column);
    }
}
void DlgMappingPresetManager::slotShowLocalLastPageResults() {

}
void DlgMappingPresetManager::slotShowLocalNextPageResults() {

}
void DlgMappingPresetManager::getJsonDataTest() {
	PresetObjectWAO pow;
    pow.getPresetByPresetName("Akai LPD8 - RK");
}
