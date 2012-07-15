
#ifndef TRACKSELECTIONDIALOG_H
#define TRACKSELECTIONDIALOG_H

#include <QDialog>
#include "ui_dlgtagfetcher.h"
#include "trackinfoobject.h"


class QTreeWidget;

class DlgTagFetcher : public QDialog, public Ui::DlgTagFetcher {
  Q_OBJECT

  public:
    DlgTagFetcher(QWidget *parent = 0);
    ~DlgTagFetcher();

    void init(const TrackPointer track);

  signals:
    void next();
    void previous();
    void finished();

  public slots:
    void FetchTagFinished(const TrackPointer,const QList<TrackPointer>& tracks);

  private slots:
    void apply();
    void cancel();
    void UpdateStack();

  private:

    void AddDivider(const QString& text, QTreeWidget* parent) const;
    void AddTrack(const TrackPointer track,
                  int resultIndex,
                  QTreeWidget* parent) const;

  private:
    struct Data {
        Data() : m_pending(true) {}

        bool m_pending;
        QList<TrackPointer> m_results;
    };

    TrackPointer m_track;
    Data m_data;
};

#endif // TRACKSELECTIONDIALOG_H


