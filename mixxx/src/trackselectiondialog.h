
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

    void init(const TrackPointer track);

  signals:
    void next();
    void previous();

  public slots:
    void FetchTagProgress(TrackPointer, QString&);
    void FetchTagFinished(const TrackPointer,const QList<TrackPointer>& tracks);

  private slots:
    void apply();
    void cancel();
    void UpdateStack(int row);
    
    // void cResultSelected();
    // void AcceptFinished();

  private:

    void AddDivider(const QString& text, QTreeWidget* parent) const;
    void AddTrack(const TrackPointer track,
                  int resultIndex,
                  QTreeWidget* parent) const;

  private:
    struct Data {
        Data() : m_pending(true), m_selectedResult(0) {}

        TrackPointer m_originalTrack;
        bool m_pending;
        QString m_progressString;
        QList<TrackPointer> m_results;
        int m_selectedResult;
    };

    QList<Data> m_data;
    TrackPointer m_track;
    QPushButton* m_pPreviousButton;
    QPushButton* m_pNextButton;
    
    int m_row;
    bool m_saveOnClose;
};

#endif // TRACKSELECTIONDIALOG_H


