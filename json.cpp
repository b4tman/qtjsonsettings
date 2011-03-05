/****************************************************************************
 **
 ** Copyright (c) 2010 Girish Ramakrishnan <girish@forwardbias.in>
 **
 ** Use, modification and distribution is allowed without limitation,
 ** warranty, liability or support of any kind.
 **
 ****************************************************************************/

#include "json.h"
#include "jsonparser.cpp"

namespace Json {

    QVariant parse(const QByteArray &json, QString *error)
    {
        JsonLexer lexer(json);
        JsonParser parser;
        if (!parser.parse(&lexer) && error)
        {
            *error = parser.errorMessage();
        }
        return parser.result();
    }

    static QByteArray escape(const QVariant &variant)
    {
        QString str = variant.toString();
        QByteArray res;
        res.reserve(str.length());
        for (int i = 0; i < str.length(); i++)
        {
            if (str[i] == '\b')
            {
                res += "\\b";
            }
            else if (str[i] == '\f')
            {
                res += "\\f";
            }
            else if (str[i] == '\n')
            {
                res += "\\n";
            }
            else if (str[i] == '\r')
            {
                res += "\\r";
            }
            else if (str[i] == '\t')
            {
                res += "\\t";
            }
            else if (str[i] == '\"')
            {
                res += "\\\"";
            }
            else if (str[i] == '\\')
            {
                res += "\\\\";
            }
            else if (str[i].unicode() > 127)
            {
                res += "\\u" + QString::number(str[i].unicode(), 16).rightJustified(4, '0');
            }
            else
            {
                res += str[i].toAscii();
            }
        }
        return res;
    }

    QByteArray stringify(const QVariant &variant)
    {
        QByteArray result;
        if (variant.type() == QVariant::List || variant.type() == QVariant::StringList)
        {
            result += "[";
            QVariantList list = variant.toList();
            for (int i = 0; i < list.count(); i++)
            {
                if (i != 0)
                    result += ",";
                result += stringify(list[i]);
            }
            result += "]";
        }
        else if (variant.type() == QVariant::Map)
        {
            QVariantMap map = variant.toMap();
            QVariantMap::const_iterator it = map.constBegin();
            result += "{";
            while (it != map.constEnd())
            {
                if (it != map.constBegin())
                    result += ",";
                result += "\"" + escape(it.key()) + "\":";
                result += stringify(it.value());
                ++it;
            }
            result += "}";
        }
        else if (variant.type() == QVariant::String || variant.type() == QVariant::ByteArray)
        {
            result = "\"" + escape(variant) + "\"";
        }
        else if (variant.type() == QVariant::Double || (int) variant.type() == (int) QMetaType::Float)
        {
            result.setNum(variant.toDouble(), 'g', 15);
        }
        else if (variant.type() == QVariant::Bool)
        {
            result = variant.toBool() ? "true" : "false";
        }
        else if (variant.type() == QVariant::Invalid)
        {
            result = "null";
        }
        else if (variant.type() == QVariant::ULongLong)
        {
            result = QByteArray::number(variant.toULongLong());
        }
        else if (variant.type() == QVariant::LongLong)
        {
            result = QByteArray::number(variant.toLongLong());
        }
        else if (variant.type() == QVariant::Int)
        {
            result = QByteArray::number(variant.toInt());
        }
        else if (variant.type() == QVariant::UInt)
        {
            result = QByteArray::number(variant.toUInt());
        }
        else if (variant.type() == QVariant::Char)
        {
            QChar c = variant.toChar();
            if (c.unicode() > 127)
                result = "\\u" + QByteArray::number(c.unicode(), 16).rightJustified(4, '0');
            else
                result.append(c.toAscii());
        }
        else if (variant.canConvert<qlonglong> ())
        {
            result = QByteArray::number(variant.toLongLong());
        }
        else if (variant.canConvert<QString> ())
        {
            result = stringify(variant);
        }

        return result;
    }

    QByteArray prettyStringify(const QVariant &variant, int indent, int indent0)
    {
        QByteArray result;

        QString indentstr0 = QString(indent0, ' ');
        QString indentstr = QString(indent0 + indent, ' ');

        switch ((int) variant.type())
        {
        case QVariant::List:
        case QVariant::StringList:
        {
            result += indentstr0;
            result += "[\n";

            QVariantList list = variant.toList();
            for (int i = 0; i < list.count(); i++)
            {
                if (i != 0)
                    result += ",\n";

                if (list[i].type() != QVariant::Map && list[i].type() != QVariant::List && list[i].type()
                                != QVariant::StringList)
                    result += indentstr;

                result += prettyStringify(list[i], indent, indent0 + indent);
            }
            result += "\n";
            result += indentstr0;
            result += "]";

            break;
        }
        case QVariant::Map:
        {
            QVariantMap map = variant.toMap();
            QVariantMap::const_iterator it = map.constBegin();

            result += indentstr0;
            result += "{\n";

            while (it != map.constEnd())
            {
                if (it != map.constBegin())
                    result += ",\n";

                result += indentstr;
                result += "\"" + escape(it.key()) + "\":";

                if (it.value().type() == QVariant::Map || it.value().type() == QVariant::List || it.value().type()
                                == QVariant::StringList)
                {
                    result += "\n";
                }
                result += prettyStringify(it.value(), indent, indent0 + indent);
                ++it;
            }
            result += "\n";
            result += indentstr0;
            result += "}";
            break;
        }
        case QVariant::String:
        case QVariant::ByteArray:
        {
            result = "\"" + escape(variant) + "\"";
            break;
        }
        case QVariant::Double:
        case QMetaType::Float:
        {
            result = QString::number(variant.toDouble(), 'g', 15).toUtf8();

            break;
        }
        case QVariant::Bool:
        {
            result = variant.toBool() ? "true" : "false";

            break;
        }
        case QVariant::Invalid:
        {
            result = "null";
            break;
        }
        case QVariant::ULongLong:
        {
            result = QByteArray::number(variant.toULongLong());
            break;
        }
        case QVariant::LongLong:
        {
            result = QByteArray::number(variant.toLongLong());
            break;
        }
        case QVariant::Int:
        {
            result = QByteArray::number(variant.toInt());
            break;
        }
        case QVariant::UInt:
        {
            result = QByteArray::number(variant.toUInt());
            break;
        }
        case QVariant::Char:
        {
            QChar c = variant.toChar();
            if (c.unicode() > 127)
                result = "\\u" + QByteArray::number(c.unicode(), 16).rightJustified(4, '0');
            else
                result.append(c.toAscii());
            break;
        }
        default:
        {
            if (variant.canConvert<qlonglong> ())
            {
                result = QByteArray::number(variant.toLongLong());
            }
            else if (variant.canConvert<QString> ())
            {
                result = stringify(variant);
            }
            break;
        }
        }

        return result;
    }
}

