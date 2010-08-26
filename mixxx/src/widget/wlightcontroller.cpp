#include <QtDebug>
#include <QPalette>

#include "defs.h"

#include "widget/wlightcontroller.h"
#include "widget/wwidget.h"
#include "widget/wskincolor.h"

#include "lights/solidcolor.h"
#include "lights/model/controlgroupmodel.h"
#include "lights/model/comboboxdelegate.h"
#include "lights/triggermode.h"
#include "lights/controlmode.h"


WLightController::WLightController(QWidget* pParent) : QWidget(pParent) {
    // TODO(XXX) kill this someday
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
    m_controlGroupTable->setModel(new ControlGroupModel(m_pLightController));
    QStringList controlModeOptions = getControlModeOptions();
    QStringList triggerModeOptions = getTriggerModeOptions();
    m_controlGroupTable->setItemDelegateForColumn(ControlGroupModel::TRIGGER_MODE,
                                                  new ComboBoxDelegate(triggerModeOptions));
    m_controlGroupTable->setItemDelegateForColumn(ControlGroupModel::CONTROL_MODE,
                                                  new ComboBoxDelegate(controlModeOptions));
}

WLightController::~WLightController() {
}

void WLightController::slotSetColor(int hue, int value) {
    QColor color = QColor::fromHsv(hue, 255, value);
    qDebug() << "Setting to hue:" << hue << "value" << value;
    m_pLightController->setColor(color);
    m_pSolidColor->setColor(color);
}


void WLightController::setup(QDomNode node) {
    QPalette pal = palette();

    // Row colors
    if (!WWidget::selectNode(node, "BgColorRowEven").isNull() &&
        !WWidget::selectNode(node, "BgColorRowUneven").isNull()) {
        QColor r1;
        r1.setNamedColor(WWidget::selectNodeQString(node, "BgColorRowEven"));
        r1 = WSkinColor::getCorrectColor(r1);
        QColor r2;
        r2.setNamedColor(WWidget::selectNodeQString(node, "BgColorRowUneven"));
        r2 = WSkinColor::getCorrectColor(r2);

        // For now make text the inverse of the background so it's readable In
        // the future this should be configurable from the skin with this as the
        // fallback option
        QColor text(255 - r1.red(), 255 - r1.green(), 255 - r1.blue());

        //setAlternatingRowColors ( true );

        pal.setColor(QPalette::Base, r1);
        pal.setColor(QPalette::AlternateBase, r2);
        pal.setColor(QPalette::Text, text);
    }

    setPalette(pal);
}
