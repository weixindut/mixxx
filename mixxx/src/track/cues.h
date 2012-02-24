#ifndef CUES_H
#define CUES_H

#include <QSharedPointer>
#include <QString>

class Cues;
typedef QSharedPointer<Cues> CuesPointer;

class CueDAO;

// A class representing a single cue point.
class Cue {
  public:
    enum CueType {
        INVALID = 0,
        CUE,
        LOAD,
        BEAT,
        LOOP,
        JUMP,
    };

    Cue();
    Cue(int id, CueType type, int position, int length,
        int hotCue, QString label);
    virtual ~Cue();

    bool isDirty() const;
    int getId() const;

    CueType getType() const;
    void setType(CueType type);

    int getPosition() const;
    void setPosition(int position);

    int getLength() const;
    void setLength(int length);

    int getHotCue() const;
    void setHotCue(int hotCue);

    const QString getLabel() const;
    void setLabel(const QString label);

    Cue* clone() const;

  private:
    void setDirty(bool dirty);
    void setId(int id);

    // Internal tracking variables for the database.
    bool m_bDirty;
    int m_iId;

    CueType m_type;
    int m_iPosition;
    int m_iLength;
    int m_iHotCue;
    QString m_label;

    friend class CueDAO;
};

class Cues {
  public:
    // Returns the number of cues.
    int numCues() const;
    // Adds a clone of pCue to the list of cues.
    void addCue(Cue* pCue);
    // Returns a copy of the i'th Cue.
    const Cue* getCue(int i) const;
    void setCue(int i, Cue* pCue);
    void removeCue(int i);
  private:
};

#endif /* CUES_H */
