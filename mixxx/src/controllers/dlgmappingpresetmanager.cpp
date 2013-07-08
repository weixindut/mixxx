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
    getUi().label_localleft
    connect(getUi().btn_search, SIGNAL(clicked()),
               this, SLOT(slotSearch()));
}
void DlgMappingPresetManager::addDlgControllerPreset(QWidget w) {

}
void DlgMappingPresetManager::slotSearch() {
	getUi().tabWidget_results
}
void DlgMappingPresetManager::getJsonDataTest() {
	PresetObjectWAO pow;
    pow.getPresetByPresetName("Akai LPD8 - RK");

    //HttpClient httpclient;
    //bool ok;
    //QString data = httpclient.get("http://127.0.0.1:8000/api/v1/midi/company/?format=json");
    //qDebug() << "Print JsonObject:"+data;

    //QVariantMap result = QtJson::parse(data,ok).toMap();
    //if(!ok) {
    //    exit(1);
    //}
    //foreach(QVariant plugin, result["objects"].toList()) {
    //    QVariantMap res = plugin.toMap();
    //    qDebug() <<"print company name:"+res["company_name"].toString();
    //}
}
