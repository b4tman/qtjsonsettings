#include "qtjsonsettings.h"
#include <QtDebug>
#include <QJsonDocument>

namespace {

    void processReadKey(QString& key, QSettings::SettingsMap &map, const QVariant& element)
    {
        switch (element.type())
        {
        case QVariant::Map:
        {
            auto vMap = element.toMap();
            auto it = vMap.constBegin();
            auto end = vMap.constEnd();

            for (; it != end; ++it)
            {
                key.append(it.key());
                key.append("/");
                processReadKey(key, map, it.value());
            }

            break;
        }
        case QVariant::List:
        {
            QVariantList list = element.toList();
            map.insert(key + "size", list.count());
            for (int i = 0; i < list.count(); ++i)
            {
                key.append(QString::number(i + 1));
                key.append("/");
                processReadKey(key, map, list.at(i));
            }
            break;
        }
        default:
            map.insert(key.left(key.size() - 1), element);
        }
        key.truncate(key.lastIndexOf("/", -2) + 1);
    }

    QVariant processWriteKey(QVariant& root, const QString& key, const QVariant& value)
    {
        int slashPos = key.indexOf('/');

        // If it is key
        if (slashPos < 0)
        {
            QVariantMap map = root.toMap();
            /** TODO VERY UGLY array detecting method
             * Key size always placed after all numeric keys (array indexes)
             * Convert root map to list and return it.
             * Warning. Index starts with 1
             */
            if (key == "size")
            {
                QVariantList list;
                for (int i = 1; i <= value.toInt(); ++i)
                {
                    list.append(map.value(QString::number(i)));
                }
                return list;
            }
            else
            {
                map.insert(key, value);
                return map;
            }
        }

        // get group name
        QString groupName = key.left(slashPos);

        // if name is number then it's row of array, convert to list
        QVariantMap map = root.toMap();
        QVariant item = map.value(groupName);
        map.insert(groupName, processWriteKey(item, key.mid(slashPos + 1), value));
        return map;
    }
}
const QSettings::Format QtJsonSettings::json_format = QSettings::registerFormat("json", readJsonFile, writeJsonFile);

bool QtJsonSettings::readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QJsonParseError error;
    auto document = QJsonDocument::fromJson(device.readAll(), &error);

    if (QJsonParseError::NoError == error.error)
    {
        if (!document.isEmpty()){
            QString str;
            processReadKey(str, map, document.toVariant());
        }
    }
    else
    {
        qWarning() << error.errorString();
    }

    return true;
}

bool QtJsonSettings::writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    QVariant resultMap;

    auto it = map.constBegin();
    auto end = map.constEnd();

    for (; it != end; ++it)
    {
        resultMap = processWriteKey(resultMap, it.key(), it.value());
    };

    device.write(QJsonDocument::fromVariant(resultMap).toJson());
    return true;

}
