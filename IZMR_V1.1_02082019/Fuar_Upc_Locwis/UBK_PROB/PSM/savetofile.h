#ifndef SAVETOFILE_H
#define SAVETOFILE_H

#include <QObject>
#include <QMutex>
#include "global.h"

class SaveToFile : public QObject
{
    Q_OBJECT
public:
    explicit SaveToFile(QObject *parent = 0);
    QMutex mutexValueSaveFile;
    QMutex mutexSetValueSaveFile;
    QVariant Value(QString deger);
    void setValue(QString tag , QVariant value);

    class Language
    {
    public:
        Language() {}
        QMutex mutexLanguage;
        QVariant readVariables(QString tag);
    };

    class Setting
    {
    public:
        Setting(){}
        QVariant readSetting(QString tag2);
    };

    class LOG
    {
    public:
        LOG() {}
        QMutex mutecLOG;
        QVariant writeLOG(QString tag, QByteArray data);
    };

    class Network
    {
    public:
        Network() {}
        QVariant writeDHCP();
        QVariant writeStatic(QString add,QString netmask,QString subnet);
    };
signals:

public slots:

};
extern SaveToFile::Setting *SettingFile;
extern SaveToFile::Language *LanguageFile;
extern SaveToFile SaveFileToJsonFormat;
extern SaveToFile::Network *NetworkFile;
#endif // SAVETOFILE_H
