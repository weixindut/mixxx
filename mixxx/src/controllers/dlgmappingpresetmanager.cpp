/*
 * dlgmappingpresetmanager.cpp
 *
 *  Created on: 2013-6-20
 *      Author: Xin Wei
 */

#include <QtGui>
#include <QHash>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QJsonObject>
#include <QJsonValue>

#include "controllers/dlgmappingpresetmanager.h"
#include "controllers/httpclient.h"
//#include "controllers/json.h"
//using namespace QtJson;

DlgMappingPresetManager::DlgMappingPresetManager(QWidget* parent)
        : QDialog(parent) {
	m_ui.setupUi(this);
	//this->show();
}
void DlgMappingPresetManager::getJsonDataTest() {
	HttpClient httpclient;
	QScriptEngine engine;
	//bool ok;
	QString data = httpclient.get("http://127.0.0.1:8000/api/v1/midi/company/?format=json");
	qDebug() << "Print JsonObject:"+data;
	//QJsonValue qjvalue(data);
	//QJsonObject qjObject = qjvalue.toObject();
	//QVariantMap result = qjObject.toVariantMap();
	//QVariantMap result = QtJson::parse(data, ok).toMap();
	//foreach(QVariant plugin, result) {
	//	qDebug() << "http get========================="+plugin.toString();
	//}

}
