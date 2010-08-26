#include <QtDebug>

#include "lights/model/lightmodel.h"
#include "lights/lightcontroller.h"
#include "lights/controlgroup.h"

#include "lights/triggermode.h"
#include "lights/controlmode.h"

LightModel::LightModel(LightController* pController)
        : m_pLightController(pController) {

    for (int i = 0; i < m_pLightController->numLights(); ++i) {
        Light* pLight = m_pLightController->getLight(i);
        m_mapper.setMapping(pLight, i);
        connect(pLight, SIGNAL(updated()), &m_mapper, SLOT(map()));
    }
    connect(&m_mapper, SIGNAL(mapped(int)), this, SLOT(lightUpdated(int)));
}

LightModel::~LightModel() {

}

void LightModel::lightUpdated(int lightNumber) {
    emit(dataChanged(index(lightNumber, 0), index(lightNumber, NUM_COLUMNS-1)));
}

int LightModel::rowCount(const QModelIndex& index) const {
    if (index.isValid()) {
        return 0;
    }

    return m_pLightController->numLights();
}

int LightModel::columnCount(const QModelIndex& index) const {
    if (index.isValid()) {
        return 0;
    }
    return LightModel::NUM_COLUMNS;
}

QModelIndex LightModel::index(int row, int column, const QModelIndex& parent) const {
    if (row < 0 || row >= rowCount(parent)) {
        return QModelIndex();
    }
    if (column < 0 || column >= columnCount(parent)) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex LightModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}


QVariant LightModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case LightModel::NAME:
                return QString(tr("Name"));
            case LightModel::STATE:
                return QString(tr("State"));
            case LightModel::COLOR:
                return QString(tr("Color"));
            default:
                return QString(tr("Unknown"));
        }
    }

    return QVariant();
}

QVariant LightModel::data(const QModelIndex& index, int role) const {
    Light* pLight = m_pLightController->getLight(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case LightModel::NAME:
                return pLight->getName();
            case LightModel::STATE:
                return lightStateToString(pLight->getState());
            case LightModel::COLOR:
                return pLight->getColor();
            default:
                return QString(tr("Unknown"));
        }
    }

    return QVariant();
}

bool LightModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        Light* pLight = m_pLightController->getLight(index.row());
        switch (index.column()) {
            case LightModel::COLOR:
                if (qVariantCanConvert<QColor>(value)) {
                    pLight->setColor(value.value<QColor>());
                    return true;
                }
            case LightModel::STATE:
            case LightModel::NAME:
            default:
                break;
        }
    }
    return false;
}

Qt::ItemFlags	LightModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    switch (index.column()) {
        case LightModel::COLOR:
            flags |= Qt::ItemIsEditable;;
            break;
        case LightModel::NAME:
        case LightModel::STATE:
        default:
            break;
    }

    return flags;
}
