#if defined (__WINDOWS__)
#include <windows.h>
#include <Shellapi.h>
#include <Shobjidl.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <QtGui>
#include <QDirModel>
#include <QStringList>
#include <QFileInfo>
#include <QDesktopServices>

#include "library/treeitem.h"
#include "library/browse/foldertreemodel.h"
#include "library/browse/browsefeature.h"

FolderTreeModel::FolderTreeModel(QObject *parent)
        : TreeItemModel(parent) {
}

FolderTreeModel::~FolderTreeModel() {
}

/* A tree model of the filesystem should be initialized lazy.
 * It will take the universe to iterate over all files over filesystem
 * hasChildren() returns true if a folder has subfolders although
 * we do not know the precise number of subfolders.
 *
 * Note that BrowseFeature inserts folder trees dynamically and rowCount()
 * is only called if necessary.
 */
bool FolderTreeModel::hasChildren( const QModelIndex & parent) const {
    TreeItem *item = static_cast<TreeItem*>(parent.internalPointer());
    /* Usually the child count is 0 becuase we do lazy initalization
     * However, for, buid-in items such as 'Quick Links' there exist
     * child items at init time
     */
    if(item->dataPath().toString() == QUICK_LINK_NODE)
        return true;
    //Can only happen on Windows
    if(item->dataPath().toString() == DEVICE_NODE)
        return true;

    // In all other cases the dataPath() points to a folder
    QString folder = item->dataPath().toString();
    return directoryHasChildren(folder);
}

bool FolderTreeModel::directoryHasChildren(const QString& path) const {
    QHash<QString, bool>::const_iterator it = m_directoryCache.find(path);
    if (it != m_directoryCache.end()) {
        return it.value();
    }

    /*
     *  The following code is too expensive, general and SLOW since
     *  QDIR::EntryInfoList returns a full QFileInfolist
     *
     *
     *  QDir dir(item->dataPath().toString());
     *  QFileInfoList all = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
     *  return (all.count() > 0);
     *
     *  We can benefit from low-level filesystem APIs, i.e.,
     *  Windows API or SystemCalls
     */

    bool has_children = false;

#if defined (__WINDOWS__)
    QString folder = path;
    folder.replace("/","\\");

    //quick subfolder test
    SHFILEINFOW sfi;
    SHGetFileInfo((LPCWSTR) folder.constData(), NULL, &sfi, sizeof(sfi), SHGFI_ATTRIBUTES);
    has_children = (sfi.dwAttributes & SFGAO_HASSUBFOLDER);
#else
    // For OS X and Linux
    // http://stackoverflow.com/questions/2579948/checking-if-subfolders-exist-linux

    std::string dot("."), dotdot("..");
    QByteArray ba = path.toLocal8Bit();
    DIR *directory = opendir(ba);
    int unknown_count = 0;
    int total_count = 0;
    if (directory != NULL) {
        struct dirent *entry;
        while (!has_children && ((entry = readdir(directory)) != NULL)) {
            if (entry->d_name != dot && entry->d_name != dotdot) {
                total_count++;
                if (entry->d_type == DT_UNKNOWN) {
                    unknown_count++;
                }
                has_children = (entry->d_type == DT_DIR || entry->d_type == DT_LNK);
            }
        }
        closedir(directory);
    }

    // If all files are of type DH_UNKNOWN then do a costlier analysis to
    // determine if the directory has subdirectories. This affects folders on
    // filesystems that do not fully implement readdir such as JFS.
    if (directory == NULL || (unknown_count == total_count && total_count > 0)) {
        QDir dir(path);
        QFileInfoList all = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        has_children = all.count() > 0;
    }
#endif

    // Cache and return the result
    m_directoryCache[path] = has_children;
    return has_children;
}
