#ifndef JSON_H
#define JSON_H

#include <QVariant>
#include <QString>

namespace QtJson {
    QVariant parse(const QString& json);
    QVariant parse(const QString& json, bool& success);
    QByteArray serialize(const QVariant& data);
    QByteArray serialize(const QVariant& data, bool& success);
    QString serializeStr(const QVariant& data);
    QString serializeStr(const QVariant& data, bool& success);
}

#endif
