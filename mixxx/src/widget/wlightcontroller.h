#ifndef WLIGHTCONTROLLER_H
#define WLIGHTCONTROLLER_H

#include "ui_dlglights.h"

#include <QWidget>

#include "lights/lightcontroller.h"
#include "widget/qcolorpicker.h"

class SolidColor;

class WLightController : public QWidget, public Ui::DlgLights {
    Q_OBJECT
  public:
    WLightController(QWidget *pParent);
    virtual ~WLightController();

  private slots:
    // void on_beat();
    // void on_onset();
    // void on_pitch(float newPitch);
    // void receive_fft();
    void slotSetColor(int hue, int sat);
  private:
    LightController* m_pLightController;
    QColorPicker* m_pColorPicker;
    SolidColor* m_pSolidColor;
};

#endif /* WLIGHTCONTROLLER_H */
