
#include "lights/lightbrickmanager.h"

LightBrickManager::LightBrickManager(QObject* pParent)
        : LightManager(pParent) {

}

LightBrickManager::~LightBrickManager() {

}

Light* LightBrickManager::newLight(QString ip_address, QString port) {
    LightBrick* pLight = new LightBrick(this, ip_address, port);
    m_lights.append(pLight);
    return pLight;
}

void LightBrickManager::sync() {
    foreach(LightBrick* pLight, m_lights) {
        pLight->sync();
    }
}
