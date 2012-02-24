#include <QtDebug>

#include "track/cues.h"

Cue::~Cue() {
    qDebug() << "~Cue()" << m_iId;
}

Cue::Cue()
        : m_bDirty(false),
          m_iId(-1),
          m_type(INVALID),
          m_iPosition(-1),
          m_iLength(0),
          m_iHotCue(-1),
          m_label("") {
    qDebug() << "Cue()";
}


Cue::Cue(int id, Cue::CueType type, int position, int length,
         int hotCue, QString label)
        : m_bDirty(false),
          m_iId(id),
          m_type(type),
          m_iPosition(position),
          m_iLength(length),
          m_iHotCue(hotCue),
          m_label(label) {
    qDebug() << "Cue(...)";
}

int Cue::getId() const {
    return m_iId;
}

void Cue::setId(int cueId) {
    m_bDirty = m_bDirty || (cueId != m_iId);
    m_iId = cueId;
    emit(updated());
}

Cue::CueType Cue::getType() const {
    return m_type;
}

void Cue::setType(Cue::CueType type) {
    m_bDirty = m_bDirty || (type != m_type);
    m_type = type;
    emit(updated());
}

int Cue::getPosition() const {
    return m_iPosition;
}

void Cue::setPosition(int position) {
    if (position % 2 != 0) {
        qDebug() << "Cue::setPosition() ignoring invalid position" << position;
        return;
    }
    m_bDirty = m_bDirty || (position != m_iPosition);
    m_iPosition = position;
    emit(updated());
}

int Cue::getLength() const {
    return m_iLength;
}

void Cue::setLength(int length) {
    if (length % 2 != 0 || length < 0) {
        qDebug() << "Cue::setLength() ignoring invalid length" << length;
        return;
    }
    m_bDirty = m_bDirty || (m_iLength != length);
    m_iLength = length;
    emit(updated());
}

int Cue::getHotCue() const {
    return m_iHotCue;
}

void Cue::setHotCue(int hotCue) {
    // TODO(XXX) enforce uniqueness?
    m_bDirty = m_bDirty || (m_iHotCue != hotCue);
    m_iHotCue = hotCue;
    emit(updated());
}

const QString Cue::getLabel() const {
    return m_label;
}

void Cue::setLabel(const QString label) {
    //qDebug() << "setLabel()" << m_label << "-" << label;
    m_bDirty = m_bDirty || (m_label != label);
    m_label = label;
    emit(updated());
}

bool Cue::isDirty() const {
    return m_bDirty;
}

void Cue::setDirty(bool dirty) {
    m_bDirty = dirty;
}
