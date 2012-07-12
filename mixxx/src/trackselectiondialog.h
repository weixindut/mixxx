
#ifndef TRACKSELECTIONDIALOG_H
#define TRACKSELECTIONDIALOG_H

#include <QDialog>
#include "ui_trackselectiondialog.h"
#include "trackinfoobject.h"


class QTreeWidget;

class TrackSelectionDialog : public QDialog, public Ui::TrackSelectionDialog {
  Q_OBJECT

  public:
    TrackSelectionDialog(QWidget *parent = 0);
    ~TrackSelectionDialog();
    
    void init(const QList<TrackPointer>& tracks);

  public slots:
    void FetchTagProgress(TrackPointer, QString&);
    void FetchTagFinished(const TrackPointer,const QList<TrackPointer>& tracks);
    void foobar();

  private slots:
    void apply();
    void cancel();
    void UpdateStack(int row);
    
    // void cResultSelected();
    // void AcceptFinished();

  private:

    void AddDivider(const QString& text, QTreeWidget* parent) const;
    void AddTrack(const TrackPointer track,
                  int result_index,
                  QTreeWidget* parent) const;

  private:
    struct Data {
        Data() : pending_(true), selected_result_(0) {}

        TrackPointer original_track_;
        bool pending_;
        QString progress_string_;
        QList<TrackPointer> results_;
        int selected_result_;
    };

    QList<Data> m_data;
    QList<TrackPointer> m_tracks;
    QPushButton* m_pPrevious_button;
    QPushButton* m_pNext_button;
    
    bool m_save_on_close;
};

#endif // TRACKSELECTIONDIALOG_H


