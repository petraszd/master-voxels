#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <QString>
#include <QVariant>
#include <QHash>

class Config
{
    private:
        static QHash<QString, QVariant> hash;

    public:
        static void init(const QString &configName);

        static QVariant get(const QString &key) {
            return Config::hash.value(key);
        }

        static unsigned int getUInt(const QString &key) {
            return Config::get(key).toUInt();
        }

        static float getFloat(const QString &key) {
            return Config::get(key).toFloat();
        }

    private:
        Config();
        Config(const Config &cfg);
};

#endif

