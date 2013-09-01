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
#include "controllers/dao/presetobjectdao.h"
#include "controllers/dlgcontrollerpreset.h"
using namespace QtJson;

DlgMappingPresetManager::DlgMappingPresetManager(QWidget* parent,ConfigObject<ConfigValue>* pConfig)
        :QDialog(parent),
         m_db(QSqlDatabase::addDatabase("QSQLITE","MAPPING_PRESET_MANAGER")) {

    if (!m_db.isOpen()) {
        m_db.setHostName("localhost");
        m_db.setDatabaseName(pConfig->getSettingsPath().append("/mixxxdb.sqlite"));
        m_db.setUserName("mixxx");
        m_db.setPassword("mixxx");
        //Open the database connection in this thread.
        if (!m_db.open()) {
            qDebug()<< "Failed to open database from mapping preset manager thread."
                    << m_db.lastError();
        }
    }

    m_ui.setupUi(this);
    getUi().tabWidget_results->setCurrentIndex(0);
    m_currentCloudResultsPage = 0;
    m_currentLocalResultsPage = 0;
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

    getUi().label_statisticalresult->setText("...");
    connect(getUi().btn_search, SIGNAL(clicked()),
            this, SLOT(slotSearch()));
    connect(getUi().tabWidget_results, SIGNAL(currentChanged(int)),
            this, SLOT(slotSetApplyText(int)));
    connect(getUi().btn_apply, SIGNAL(clicked()),
            this, SLOT(slotOk()));
}
DlgMappingPresetManager::~DlgMappingPresetManager() {
    for(int i=0;i<m_gridLayoutListCloud.size();i++) {
        delete m_gridLayoutListCloud[i];
    }
    m_gridLayoutListCloud.clear();
    m_presetListCloud.clear();
    for(int i=0;i<getUi().stackedWidgetCloud->count();i++) {
        getUi().stackedWidgetCloud->removeWidget(getUi().stackedWidgetCloud->widget(i));
    }
    for(int i=0;i<m_gridLayoutListLocal.size();i++) {
        delete m_gridLayoutListLocal[i];
    }
    m_gridLayoutListLocal.clear();
    m_presetListLocal.clear();
    for(int i=0;i<getUi().stackedWidgetLocal->count();i++) {
        getUi().stackedWidgetLocal->removeWidget(getUi().stackedWidgetLocal->widget(i));
    }
}


void DlgMappingPresetManager::slotSearch() {
    int index=getUi().tabWidget_results->currentIndex();
    if(index == 0) {
        searchLocal();
    } else {
        getUi().tabWidget_results->setCurrentIndex(1);
        searchCloud();
    }
}

void DlgMappingPresetManager::searchLocal() {
    for(int i=0;i<m_gridLayoutListLocal.size();i++) {
        delete m_gridLayoutListLocal[i];
    }
    m_gridLayoutListLocal.clear();
    m_presetListLocal.clear();
    for(int i=0;i<getUi().stackedWidgetLocal->count();i++) {
        getUi().stackedWidgetLocal->removeWidget(getUi().stackedWidgetLocal->widget(i));
    }
    QString searchcontent=getUi().lineEdit_search->text();
    PresetObjectDAO pod(m_db);
    m_presetListLocal=pod.getPresetByPresetName(searchcontent);

    showLocalSearchResults();
}
void DlgMappingPresetManager::searchCloud() {
    for(int i=0;i<m_gridLayoutListCloud.size();i++) {
    	delete m_gridLayoutListCloud[i];
    }
    m_gridLayoutListCloud.clear();
    m_presetListCloud.clear();
    for(int i=0;i<getUi().stackedWidgetCloud->count();i++) {
        getUi().stackedWidgetCloud->removeWidget(getUi().stackedWidgetCloud->widget(i));
    }
    QString searchcontent=getUi().lineEdit_search->text();
    PresetObjectWAO pow;
    m_presetListCloud=pow.getPresetByPresetName("/tmp/",searchcontent);
    //m_presetListCloud=pow.getPresetByURL("http://127.0.0.1:8000/api/v1/midi/preset/?format=json");
    showCloudSearchResults();
}
void DlgMappingPresetManager::showLocalSearchResults() {
    QString result = "WoW, "+QString::number(m_presetListLocal.size())+" presets!";
    getUi().label_statisticalresult->setText(result);
    connect(getUi().btn_localleft,SIGNAL(clicked()),
            this, SLOT(slotShowLocalLastPageResults()));
    connect(getUi().btn_localright,SIGNAL(clicked()),
            this, SLOT(slotShowLocalNextPageResults()));
    if(m_presetListLocal.size()>8) {
        getUi().btn_localright->setEnabled(true);
    } else {
        getUi().btn_localright->setEnabled(false);
    }
    for(int i=0,row=0,column=0;i<m_presetListLocal.size();i++,column++) {
        if(i%4==0) {
            row++;
            column=0;
            if(i%8==0) {
                QWidget* pPageWidget = new QWidget();
                QGridLayout* gridLayout = new QGridLayout();
                pPageWidget->setLayout(gridLayout);
                m_gridLayoutListLocal.append(gridLayout);
                getUi().stackedWidgetLocal->addWidget(pPageWidget);
                row=0;
            }
        }
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        if (!m_presetListLocal[i].PictureFileNames().isEmpty()) {
        	//TODO(weixin): rewrite the path
            showpreset->setCover("./res/controllers/"+m_presetListLocal[i].PictureFileNames()[0]);
        } else {
            showpreset->setCover("./res/images/mixxx-icon.png");
        }
        showpreset->setPresetName(m_presetListLocal[i].name());
        showpreset->setSource(m_presetListLocal[i].presetSource());
        showpreset->setStatus(m_presetListLocal[i].presetStatus());
        showpreset->setRatings(m_presetListLocal[i].Ratings());
        showpreset->setPresetID(m_presetListLocal[i].Pid());
        m_gridLayoutListLocal[i/8]->addWidget(showpreset,row,++column);
    }
    getUi().stackedWidgetLocal->setCurrentIndex(m_currentLocalResultsPage);
}
void DlgMappingPresetManager::showCloudSearchResults() {
	qDebug()<<"================slotShowCloudSearchResults()=============";
    QString result = "WoW, "+QString::number(m_presetListCloud.size())+" presets!";
    getUi().label_statisticalresult->setText(result);
    connect(getUi().btn_cloudleft,SIGNAL(clicked()),
            this, SLOT(slotShowCloudLastPageResults()));
    connect(getUi().btn_cloudright,SIGNAL(clicked()),
            this, SLOT(slotShowCloudNextPageResults()));
    if(m_presetListCloud.size()>8) {
        getUi().btn_cloudright->setEnabled(true);
    } else {
        getUi().btn_cloudright->setEnabled(false);
    }
    for(int i=0,row=0,column=0;i<m_presetListCloud.size();i++,column++) {
        if(i%4==0) {
            row++;
            column=0;
            if(i%8==0) {
                QWidget* pPageWidget = new QWidget();
                QGridLayout* gridLayout = new QGridLayout();
                pPageWidget->setLayout(gridLayout);
                m_gridLayoutListCloud.append(gridLayout);
                getUi().stackedWidgetCloud->addWidget(pPageWidget);
                row=0;
            }
        }
        DlgControllerPreset* showpreset = new DlgControllerPreset(this);
        if (!m_presetListCloud[i].PictureFileNames().isEmpty()) {
        	//TODO(weixin): rewrite the path
        	qDebug()<<"================showpreset->setCover=============";
            showpreset->setCover("./tmp/"+m_presetListCloud[i].PictureFileNames()[0]);

        } else {
            showpreset->setCover("./res/images/mixxx-icon.png");
        }

        showpreset->setPresetName(m_presetListCloud[i].name());
        showpreset->setSource(m_presetListCloud[i].presetSource());
        showpreset->setStatus(m_presetListCloud[i].presetStatus());
        showpreset->setRatings(m_presetListCloud[i].Ratings());
        showpreset->setPresetID(m_presetListCloud[i].Pid());
        m_gridLayoutListCloud[i/8]->addWidget(showpreset,row,++column);
    }
    getUi().stackedWidgetCloud->setCurrentIndex(m_currentCloudResultsPage);
}
void DlgMappingPresetManager::slotShowCloudNextPageResults() {
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
    getUi().stackedWidgetCloud->setCurrentIndex(m_currentCloudResultsPage);
}
void DlgMappingPresetManager::slotShowCloudLastPageResults() {
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
    getUi().stackedWidgetCloud->setCurrentIndex(m_currentCloudResultsPage);
}
void DlgMappingPresetManager::slotShowLocalLastPageResults() {
    m_currentLocalResultsPage--;
    if(m_currentLocalResultsPage <= 0) {
        getUi().btn_localleft->setEnabled(false);
    } else {
        getUi().btn_localleft->setEnabled(true);
    }
    if((m_currentLocalResultsPage+1)*8 >= m_presetListLocal.size()) {
        getUi().btn_localright->setEnabled(false);
    } else {
        getUi().btn_localright->setEnabled(true);
    }
    getUi().stackedWidgetLocal->setCurrentIndex(m_currentLocalResultsPage);
}
void DlgMappingPresetManager::slotShowLocalNextPageResults() {
    m_currentLocalResultsPage++;
    if(m_currentLocalResultsPage <= 0) {
        getUi().btn_localleft->setEnabled(false);
    } else {
        getUi().btn_localleft->setEnabled(true);
    }
    if((m_currentLocalResultsPage+1)*8 >= m_presetListLocal.size()) {
        getUi().btn_localright->setEnabled(false);
    } else {
        getUi().btn_localright->setEnabled(true);
    }
    getUi().stackedWidgetLocal->setCurrentIndex(m_currentLocalResultsPage);
}
void DlgMappingPresetManager::getJsonDataTest() {
    PresetObjectWAO pow;
    pow.getPresetByPresetName("/tmp/","Akai LPD8 - RK");
}
void DlgMappingPresetManager::slotSetApplyText(int index) {
    if(index == 0) {
        getUi().btn_apply->setText("Apply");
    } else {
        getUi().btn_apply->setText("Download&&Apply");
    }
}
void DlgMappingPresetManager::slotOk() {
    if(getUi().tabWidget_results->currentIndex()==0) {
        if(getSelectedPreset(m_gridLayoutListLocal,m_presetListLocal)==true) {
            qDebug()<<"filePath:====="+ m_selectedPreset.filePath();
            emit(presetReturned(m_selectedPreset.filePath()));
            close();
        }
    } else {
        if(getSelectedPreset(m_gridLayoutListCloud,m_presetListCloud)==true) {
            QMessageBox message(QMessageBox::NoIcon, "Question",
                                "Before applying a preset, you should download it first. Whether download it?",
                                QMessageBox::Yes | QMessageBox::No, NULL);
            if(message.exec() == QMessageBox::Yes) {
            	PresetObjectWAO pow;
                pow.getPresetByPresetID("./res/controllers/", m_selectedPreset.Pid());
                qDebug()<<"filePath:====="+ m_selectedPreset.filePath();
                emit(presetReturned(m_selectedPreset.filePath()));
                close();
            }
    	}
    }
}
bool DlgMappingPresetManager::getSelectedPreset(QList<QGridLayout* > layoutList,
        QList<MidiControllerPreset> presetList) {
    QList<QString> pids;
    for(int i=0; i<layoutList.size(); i++) {
        for(int j=0; j<layoutList[i]->count(); j++) {
            QWidget* item = layoutList[i]->itemAt(j)->widget();
            DlgControllerPreset* dlg = (DlgControllerPreset*)item;
            if(dlg->isSelected()) {
                pids.append(dlg->presetID());
            }
        }
    }
    //QList<MidiControllerPreset> res;
    if(pids.size()==0) {
        QString message = "Please select one preset!";
        QMessageBox::information(this, tr("Info"), message);
        return false;
    } else if (pids.size()==1) {
        for(int i=0; i<presetList.size();i++) {
            if(presetList[i].Pid()==pids[0]) {
            	m_selectedPreset = presetList[i];
            	return true;
                //res.append(presetList[i]);
            }
        }
        QString message = "Sorry, there's something wrong!";
        QMessageBox::information(this, tr("Info"), message);
        return false;
    } else {
        QString message = "More than one preset has been selected!";
        QMessageBox::information(this, tr("Info"), message);
        return false;
    }
}
