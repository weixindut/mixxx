#ifndef LIGHTGROUPMODEL_H
#define LIGHTGROUPMODEL_H

#include <QVariant>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QItemDelegate>
#include <QComboBox>
#include <QStringList>
#include <QStringListModel>

class LightController;

class ControlGroupModel : public QAbstractItemModel {
  public:
    ControlGroupModel(LightController* pLightController);
    virtual ~ControlGroupModel();

    enum ControlGroupColumns {
        NAME = 0,
        TRIGGER_MODE,
        CONTROL_MODE,
        COLOR_GENERATOR,
        NUM_LIGHTS,
        NUM_COLUMNS
    };

    virtual int columnCount(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& index) const;
    virtual QModelIndex	index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    virtual QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;
    QModelIndex parent(const QModelIndex& index) const;

  private:
    mutable LightController* m_pLightController;
};

// class ComboBoxDelegate : public QItemDelegate {
//   public:
//     ComboBoxDelegate(QStringList choices);

//     QWidget *createEditor(QWidget *parent,
//                           const QStyleOptionViewItem &option,
//                           const QModelIndex &index) const;

//     void setEditorData(QWidget *editor, const QModelIndex &index) const;
//     void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
//   private:
//     QStringList m_choices;
// };


#endif /* LIGHTGROUPMODEL_H */
