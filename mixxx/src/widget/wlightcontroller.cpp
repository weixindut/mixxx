#include <QtDebug>

#include "defs.h"

#include "widget/wlightcontroller.h"

#include "lights/solidcolor.h"


WLightController::WLightController(QWidget* pParent) : QWidget(pParent) {
    m_pLightController = LightController::sInstance;

    // We should have been setup after the engine
    Q_ASSERT(m_pLightController);

    setupUi(this);

    QBoxLayout* box = dynamic_cast<QBoxLayout*>(layout());
    Q_ASSERT(box); //Assumes the form layout is a QVBox/QHBoxLayout!
    box->removeWidget(m_colorPickerPlaceholder);
    m_colorPickerPlaceholder->hide();
    m_pColorPicker = new QColorPicker(this);
    box->insertWidget(1, m_pColorPicker);
    connect(m_pColorPicker, SIGNAL(newCol(int, int)), this, SLOT(slotSetColor(int, int)));

    m_pSolidColor = new SolidColor(Qt::black);
}

WLightController::~WLightController() {
}

void WLightController::slotSetColor(int hue, int value) {
    QColor color = QColor::fromHsv(hue, 255, value);
    qDebug() << "Setting to hue:" << hue << "value" << value;
    m_pLightController->setColor(color);
    m_pSolidColor->setColor(color);
}

