#include <config.h>
#include <QSettings>
#include <QStringList>

QHash<QString, QVariant> Config::hash;

void Config::init(const QString &configName)
{
    QSettings settings("config.cfg", QSettings::IniFormat);
    foreach(QString key, settings.allKeys()) {
        hash[key] = settings.value(key);
    }
}

