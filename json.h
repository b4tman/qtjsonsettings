/****************************************************************************
**
** Copyright (c) 2010 Girish Ramakrishnan <girish@forwardbias.in>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef JSON_H
#define JSON_H

#include <QtCore/QByteArray>
#include <QtCore/QVariant>

namespace Json {
    QVariant parse(const QByteArray &data, QString *error = 0);
    QByteArray stringify(const QVariant &variant);
    QByteArray prettyStringify(const QVariant &variant, int indent = 4, int indent0 = 0);
};

#endif // JSON_H

