
#include "lights/dmxlight.h"
#include "lights/dmxlightmanager.h"


DMXLight::DMXLight(DMXLightManager* pManager, int lightNumber)
        : m_pManager(pManager),
          m_iLightNumber(lightNumber) {

}

DMXLight::~DMXLight() {

}
