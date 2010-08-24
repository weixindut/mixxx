
#ifndef LIGHTBRICKMANAGER_H
#define LIGHTBRICKMANAGER_H

#include <QList>

#include "lights/lightbrick.h"
#include "lights/lightmanager.h"

class LightBrickManager : public LightManager {
    Q_OBJECT
  public:
    LightBrickManager(QObject* pParent);
    virtual ~LightBrickManager();

    Light* newLight(QString ip_address, QString port);
    void sync();
  private:
    QList<LightBrick*> m_lights;
};


#endif /* LIGHTBRICKMANAGER_H */
