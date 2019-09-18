/**************************************************************************************
 * savetofile.cpp class definations
 * Starting Date:05.10.2018
 * Last Modified Date:08.10.2018
 * Author:Samet KÜSTÜL
 * Definations:
 * Dosya kaydetme işleminin yapıldığı sınıftır.
 * Şuan için veriler JSON formatında kaydediliyor.
 *
 *
 *
 *
 *
 *
 *
 * **************************************************************************************/
#include "savetofile.h"
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QVariant>
#include<QFile>
#include<QSaveFile>
#include <QDebug>
SaveToFile SaveFileToJsonFormat;
SaveToFile::Language *LanguageFile;
SaveToFile::Network *NetworkFile;
SaveToFile::Setting *SettingFile;
SaveToFile::SaveToFile(QObject *parent) :
    QObject(parent)
{
}

QVariant SaveToFile::Value(QString deger)
{
    mutexValueSaveFile.lock();
    QVariant var;
    QString data;
    QFile file("/exec/PCR157/setting_save.txt");
    if(file.open(QFile::ReadWrite))
    {
        data=file.readAll();
    }
    file.close();
    QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj2 = doc2.object();
    QJsonArray save_array= obj2["SETTINGS"].toArray();
    QJsonObject save_Data=save_array.at(0).toObject();
    var=save_Data.value(deger).toVariant();
    system("sync");
    mutexValueSaveFile.unlock();
    return var;
}

void SaveToFile::setValue(QString tag, QVariant value)
{

    mutexSetValueSaveFile.lock();
    QJsonObject file_object;
    QJsonArray save_read;
    QJsonArray save_write;
    QJsonObject save_Data;
    QString data;
    QFile file("/exec/PCR157/setting_save.txt");
    file.setPermissions(QFile::ExeGroup | QFile::ReadOther | QFile::WriteOther);
    system("sync");
    if(file.open(QFile::ReadWrite))
    {
        data=file.readAll();
    }
    file.close();
    QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
    save_Data = doc2.object();
    save_read= save_Data["SETTINGS"].toArray();
    save_Data=save_read.at(0).toObject();

    save_Data[tag]=/*QJsonValue::fromVariant(value);*/value.toString();
    qDebug()<<"tag:"<<tag<<"value:"<<value;
    save_write.append(save_Data);

    file_object["SETTINGS"]=save_write;
    QJsonDocument doc(file_object);
    QSaveFile Savefile("/exec/PCR157/setting_save.txt");
    if(Savefile.open(QSaveFile::WriteOnly))
    {
        Savefile.write(doc.toJson(QJsonDocument::Compact));
    }
     Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
    Savefile.commit();
    system("sync");
    mutexSetValueSaveFile.unlock();
}


QVariant SaveToFile::Language::readVariables(QString tag)
{

    QVariant var;
    QString data;

    QFile file("/var/www/html/language_tr.json");
    if(file.open(QFile::ReadWrite))
    {
        data=file.readAll();
    }
    file.close();
    QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj2 = doc2.object();
    QJsonArray save_array= obj2["LANGUAGE"].toArray();
    QJsonObject save_Data=save_array.at(0).toObject();
    var=save_Data.value(tag).toVariant();
    system("sync");
    return var;
}

QVariant SaveToFile::Setting::readSetting(QString tag2){
    QVariant var;
    QString data;
    QFile file("/exec/chromiumSettings.json");
    if(file.open(QFile::ReadWrite)){

        data=file.readAll();
    }
    file.close();
    QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj2 = doc2.object();
    QJsonArray save_array= obj2["SETTINGS"].toArray();
    QJsonObject save_Data=save_array.at(0).toObject();
    var=save_Data.value(tag2).toVariant();
    system("sync");

    return var;
}



QVariant SaveToFile::Network::writeDHCP()
{
    // Dinamik Ip kaydetme
    QSaveFile file("/etc/network/interfaces");
    if(file.open(QSaveFile::WriteOnly)){
    QString network1="source-directory /etc/network/interfaces.d\nConfigure Eth0_DHCP\nauto eth0\nallow-hotplug eth0\niface eth0 inet dhcp";
    file.write(network1.toUtf8());
    }
    file.commit();
}

QVariant SaveToFile::Network::writeStatic(QString add,QString netmask,QString subnet)
{
   QSaveFile file("/etc/network/interfaces");
   if(file.open(QSaveFile::WriteOnly)){
   QString network1="source-directory /etc/network/interfaces.d\nConfigure Eth0_DHCP\nauto eth0\nallow-hotplug eth0\niface eth0 inet static\naddress "+add+"\nnetmask "+netmask+"\ngateway "+subnet;
   qDebug()<<network1;
   file.write(network1.toUtf8());
   }
   file.commit();
}
