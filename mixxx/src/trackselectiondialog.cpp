
#include "trackselectiondialog.h"

#include <QFileInfo>
#include <QFutureWatcher>
#include <QPushButton>
#include <QShortcut>
#include <QTreeWidget>
#include <QUrl>
#include <QtConcurrentRun>
#include <QtDebug>

TrackSelectionDialog::TrackSelectionDialog(QWidget *parent)
                    : QDialog(parent) {
    // Setup dialog window
    setupUi(this);
    
    connect(btnApply, SIGNAL(clicked()),
            this, SLOT(apply()));
    connect(btnCancel, SIGNAL(clicked()),
            this, SLOT(cancel()));
            /*
    connect(results, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            SLOT(ResultSelected()));
            */
    
    
    
    //Resize columns
    results->setColumnWidth(0, 50);  // Track column
    results->setColumnWidth(1, 50);  // Year column
    results->setColumnWidth(2, 160); // Title column
    results->setColumnWidth(3, 160); // Artist column
    results->setColumnWidth(4, 160); // Album column
}

TrackSelectionDialog::~TrackSelectionDialog() {
}

void TrackSelectionDialog::init(const QList<TrackPointer>& tracks) {
    m_tracks=tracks;
    foreach (TrackPointer track, tracks) {
        Data data;
        data.original_track_ = track;
        m_data << data;
    }
}

void TrackSelectionDialog::apply() {
    accept();
}

void TrackSelectionDialog::cancel() {
    reject();
}

void TrackSelectionDialog::FetchTagProgress(TrackPointer track, QString& progress){
    // Find the item with this filename
    int row = -1;
    for (int i=0 ; i<m_data.count() ; ++i) {
        if (m_data[i].original_track_->getLocation() == track->getLocation()) {
            row = i;
            break;
        }
    }

    if (row == -1)
        return;

  m_data[row].progress_string_ = progress;

    // If it's the current item, update the display
    // if (ui_->song_list->currentIndex().row() == row) {
    UpdateStack(row);
    // }
}

void TrackSelectionDialog::foobar(){
    qDebug() << "do I receive this signal ;)";
}

void TrackSelectionDialog::FetchTagFinished(const TrackPointer track,
                                            const QList<TrackPointer>& tracks){
    qDebug() << "dialog got all results from the web search" << tracks.size();
    // Find the item with this filename
    int row = -1;
    for (int i=0 ; i<m_data.count() ; ++i) {
        if (m_data[i].original_track_->getLocation() == track->getLocation()) {
            row = i;
            break;
        }
    }

    if (row == -1)
        return;

    m_data[row].pending_=false;
    m_data[row].results_ = tracks;

    UpdateStack(row);
}

void TrackSelectionDialog::UpdateStack(int row) {

    const Data& data = m_data[row];
    
    if (data.pending_) {
        // stack->setCurrentWidget(loading_page);
        // progress->set_text(data.progress_string_ + "...");
        return;
    } else if (data.results_.isEmpty()) {
        stack->setCurrentWidget(error_page);
        return;
    }
        stack->setCurrentWidget(results_page);
    
    // Clear tree widget
    results->clear();
    
    // Put the original tags at the top
    AddDivider(tr("Original tags"), results);
    AddTrack(data.original_track_, -1, results);
    
    // Fill tree view with songs
    AddDivider(tr("Suggested tags"), results);

    int track_index = 0;
    foreach (const TrackPointer track, data.results_) {
        AddTrack(track, track_index++, results);
    }

    // Find the item that was selected last time
    for (int i=0 ; i<results->model()->rowCount() ; ++i) {
        const QModelIndex index = results->model()->index(i, 0);
        const QVariant id = index.data(Qt::UserRole);
        if (!id.isNull() && id.toInt() == data.selected_result_) {
            results->setCurrentIndex(index);
            break;
        }
    }
}

void TrackSelectionDialog::AddTrack(const TrackPointer track,
                                   int result_index,
                                   QTreeWidget* parent) const {
    QStringList values;
    values << track->getTrackNumber() << track->getYear() << track->getTitle()
           << track->getArtist() << track->getAlbum();

    QTreeWidgetItem* item = new QTreeWidgetItem(parent, values);
    item->setData(0, Qt::UserRole, result_index);
    item->setData(0, Qt::TextAlignmentRole, Qt::AlignRight);
}

void TrackSelectionDialog::AddDivider(const QString& text, QTreeWidget* parent) const {
  QTreeWidgetItem* item = new QTreeWidgetItem(parent);
  item->setFirstColumnSpanned(true);
  item->setText(0, text);
  item->setFlags(Qt::NoItemFlags);
  item->setForeground(0, palette().color(QPalette::Disabled, QPalette::Text));

  QFont bold_font(font());
  bold_font.setBold(true);
  item->setFont(0, bold_font);
}
