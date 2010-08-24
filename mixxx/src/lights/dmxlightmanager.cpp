
#include "lights/dmxlightmanager.h"
#include "lights/dmxlight.h"

DMXLightManager::DMXLightManager(QObject* pParent, QString ip_address)
        : LightManager(pParent) {
    m_pHandle = new_dmx_handle(ip_address.toAscii().data());
}

DMXLightManager::~DMXLightManager() {

}

Light* DMXLightManager::newLight(int light_number) {
    DMXLight* pLight = new DMXLight(this, light_number);
    m_lights.append(pLight);
    return pLight;
}

void DMXLightManager::setColor(Light* pLight, QColor color) {
    DMXLight* pDMXLight = dynamic_cast<DMXLight*>(pLight);
    Q_ASSERT(pDMXLight);

    set_dmx_color(m_pHandle, pDMXLight->getLightNumber(), color.red(), color.green(), color.blue());
}

void DMXLightManager::sync() {
    foreach (DMXLight* pLight, m_lights) {
        setColor(pLight, pLight->getColor());
    }
    publish_dmx_update(m_pHandle);
}
