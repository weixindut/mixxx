/**
* @file controllerpreset.h
* @author Sean Pappalardo spappalardo@mixxx.org
* @date Mon 9 Apr 2012
* @brief Controller preset
*
* This class represents a controller preset, containing the data elements that
* make it up.
*/

#ifndef CONTROLLERPRESET_H
#define CONTROLLERPRESET_H

#include <QtCore>
#include <QHash>
#include <QSharedPointer>

class ControllerPresetVisitor;

class ControllerPreset {
  public:
    ControllerPreset() {}
    virtual ~ControllerPreset() {}

    /** addScriptFile(QString,QString)
     * Adds an entry to the list of script file names & associated list of function prefixes
     * @param filename Name of the XML file to add
     * @param functionprefix Function prefix to add
     */
    void addScriptFile(QString filename, QString functionprefix) {
        scriptFileNames.append(filename);
        scriptFunctionPrefixes.append(functionprefix);
    }
    void addPictureFile(QString name) {
    	m_pictureFileNames.append(name);
    }
    QList<QString> PictureFileNames() {
    	return m_pictureFileNames;
    }
    inline void setDeviceId(const QString id) {
        m_deviceId = id;
    }

    inline QString deviceId() const {
        return m_deviceId;
    }

    inline void setFilePath(const QString filePath) {
        m_filePath = filePath;
    }

    inline QString filePath() const {
        return m_filePath;
    }

    inline void setName(const QString name) {
        m_name = name;
    }

    inline QString name() const {
        return m_name;
    }

    inline void setAuthor(const QString author) {
        m_author = author;
    }

    inline QString author() const {
        return m_author;
    }

    inline void setDescription(const QString description) {
        m_description = description;
    }

    inline QString description() const {
        return m_description;
    }

    inline void setForumLink(const QString forumlink) {
        m_forumlink = forumlink;
    }

    inline QString forumlink() const {
        return m_forumlink;
    }

    inline void setWikiLink(const QString wikilink) {
        m_wikilink = wikilink;
    }

    inline QString wikilink() const {
        return m_wikilink;
    }

    inline void setSchemaVersion(const QString schemaVersion) {
        m_schemaVersion = schemaVersion;
    }

    inline QString schemaVersion() const {
        return m_schemaVersion;
    }

    inline void setMixxxVersion(const QString mixxxVersion) {
        m_mixxxVersion = mixxxVersion;
    }

    inline QString mixxxVersion() const {
        return m_mixxxVersion;
    }

    inline void setPresetSource(const QString presetSource) {
        m_preset_source = presetSource;
    }

    inline QString presetSource() const {
        return m_preset_source;
    }

    inline void setPresetStatus(const QString presetStatus) {
        m_preset_status = presetStatus;
    }

    inline QString presetStatus() const {
        return m_preset_status;
    }

    inline void setPid(const QString pid) {
        m_pid = pid;
    }

    inline QString Pid() const {
        return m_pid;
    }

    //inline void setPicturePath(const QString path) {
    //    m_picture_path = path;
    //}

    //inline QString picturePath() const {
    //    return m_picture_path;
    //}

    inline void setRatings(const float ratings) {
    	m_ratings = ratings;
    }

    inline float Ratings() const {
        return m_ratings;
    }

    inline void addProductMatch(QHash<QString,QString> match) {
        m_productMatches.append(match);
    }

    virtual void accept(ControllerPresetVisitor* visitor) const = 0;
    virtual bool isMappable() const = 0;

    // Data elements
    // TODO(XXX) make private
    QList<QString> scriptFileNames;
    QList<QString> scriptFunctionPrefixes;
    // Optional list of controller device match details
    QList< QHash<QString,QString> > m_productMatches;


  private:
    QString m_deviceId;
    QString m_filePath;
    QString m_name;
    QString m_author;
    QString m_description;
    QString m_forumlink;
    QString m_wikilink;
    QString m_schemaVersion;
    QString m_mixxxVersion;
    QString m_preset_source;
    QString m_preset_status;
    QString m_pid;
    //QString m_picture_path;
    QList<QString> m_pictureFileNames;
    float m_ratings;
};

typedef QSharedPointer<ControllerPreset> ControllerPresetPointer;

#endif
