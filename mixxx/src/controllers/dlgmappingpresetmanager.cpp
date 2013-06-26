/*
 * dlgmappingpresetmanager.cpp
 *
 *  Created on: 2013-6-20
 *      Author: Xin Wei
 */

#include <QtGui>
#include <QHash>
#include <QVariant>
#include <QVariantList>
#include <QMap>
#include "controllers/dlgmappingpresetmanager.h"
#include "controllers/httpclient.h"
#include "controllers/json.h"
using namespace QtJson;

DlgMappingPresetManager::DlgMappingPresetManager(QWidget* parent)
        : QDialog(parent) {
	m_ui.setupUi(this);
	//this->show();
}
void DlgMappingPresetManager::getJsonDataTest() {
	HttpClient httpclient;
	//QScriptEngine engine;
	bool ok;
	QString data = httpclient.get("http://127.0.0.1:8000/api/v1/midi/company/?format=json");
	qDebug() << "Print JsonObject:"+data;
	//QJsonValue qjvalue(data);
	//QJsonObject qjObject = qjvalue.toObject();
	//QVariantMap result = qjObject.toVariantMap();
	//QMap<QString, QVariant> result = QtJson::parse(data, ok).toMap();
	qDebug() <<"========Print parseJsonVariant:"+QtJson::parse(data, ok).toString();
	QVariantMap result= QtJson::parse(data, ok).toMap();
	if(!ok) {
	    qFatal("An error occurred during parsing");
	    exit(1);
	}
	QVariantList list = result["objects"].toList();
	foreach(QVariant qv,list) {
		qDebug() << "========Print parseJsonObject:"+qv.toMap()["id"].toInt();
	}

	//QVariantMap nestedMap = result["objects"].toList();
	//foreach(QVariantMap nestedMap, result["objects"]) {
	//	qDebug() << "http get========================="+nestedMap["company_name"].toString();
	//}

}
