#ifndef PREPARELIBRARYTABLEMODEL_H_
#define PREPARELIBRARYTABLEMODEL_H_

#include <QModelIndexList>
#include "librarytablemodel.h"

class PrepareLibraryTableModel : public LibraryTableModel
{
    Q_OBJECT
  public:
    PrepareLibraryTableModel(QObject* parent, TrackCollection* pTrackCollection,
                             QList<int> availableDirIds);
    virtual ~PrepareLibraryTableModel();

    void search(const QString& searchText,
                const QString& extraFilter = QString());
    bool isColumnInternal(int column);

  public slots:
    void showRecentSongs();
    void showAllSongs();
  private:
    bool m_bShowRecentSongs;
};

#endif
