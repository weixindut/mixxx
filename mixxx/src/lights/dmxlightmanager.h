#ifndef DMXLIGHTMANAGER_H
#define DMXLIGHTMANAGER_H

#include <QList>

#include "lights/dmx-eth.h"
#include "lights/dmxlight.h"
#include "lights/light.h"
#include "lights/lightmanager.h"

class DMXLightManager : LightManager {
  public:
    DMXLightManager(QObject* pParent, QString ip_address);
    virtual ~DMXLightManager();
    Light* newLight(int light_number);
    void sync();

  private:
    void setColor(Light* pLight, QColor color);

    QList<DMXLight*> m_lights;
    DMX_Handle* m_pHandle;

    friend class DMXLight;
};

#endif /* DMXLIGHTMANAGER_H */
