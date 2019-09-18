#include "updater.h"
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <QCryptographicHash>
#include <QFile>
#include <QThread>
#include <QDateTime>
#include "syslog.h"
#include"unistd.h"

QString getMacAddress();
quint64 file_size_last=0;
QSettings setting_checkMyIniFile(SettingsFile,QSettings::IniFormat);
QSettings setting_checkDownloadIniFile("/wizard/version.ini",QSettings::IniFormat);
updater::updater(QObject *parent) :
    QObject(parent)
{


    update_count=0;
    check_server=false;
//    updaterTimer= new QTimer(this);
//    conrolTimer=new QTimer(this);
//    versionTimer=new QTimer(this);
    debugTimer=new QTimer(this);
    request_flg=1;
    update_isUpdated_flg=0;
    send_log_flg=0;


    //______Command 1_______//
    command1="wget --timeout=6 -O ";                // targetPath de belirtilen adresten ini dosyasını alıp savePath ile belirtilen yola dosyayı indiriyor
    command1.append(savePath);
    command1.append(saveIniFile);
    command1.append(" ");
    command1.append(targetPath);
    command1.append(downloadIniFile);

    //______Command 2_______//         // zip dosyası halinde indirme
    command2="wget --timeout=6 -c ";
    command2.append(targetPath);
    command2.append(downloadExFile);
    command2.append(" -P ");
    command2.append(savePath);

    //______Command 3_______//
    command3="unzip -o ";
    command3.append(savePath);
    command3.append(downloadExFile);
    command3.append(" -d ");
    command3.append(savePath);

    //______Command 4 _______//           // ini file güncelle
    command4="mv -f ";
    command4.append(savePath);
    command4.append("/setting.ini");
    command4.append(" /var/www/html/");

    command5="mv -f ";
    command5.append(savePath);
    command5.append("/PSM ");
    command5.append("/exec");


    command6="chmod 777 ";
    command6.append("/exec/PSM");

    command7="/root/Desktop/ftpConnection.sh debug ";
    command7.append(getMacAddress().remove(":"));

    command8="mv -f /var/log/syslog /var/www/html/syslog.txt";
    command9="service rsyslog restart";
    sync();
    system("sync");
    debugTimer->start(20000);


    update_check_flg=setting_checkMyIniFile.value("update_check_flg").toInt();
    read_Date=setting_checkDownloadIniFile.value("Date").toString();
    read_md5sum=setting_checkDownloadIniFile.value("md5sum").toString();
    read_Version=setting_checkDownloadIniFile.value("Version").toString();
    read_iniUpdate=setting_checkDownloadIniFile.value("iniUpdate").toString();
    sync();
    system("sync");

}

void updater::checkDownloadFile(quint8 var)
{
    QSettings setting_checkDownloadIniFile("/wizard/version.ini",QSettings::IniFormat);
    QSettings setting_checkMyIniFile(SettingsFile,QSettings::IniFormat);
    read_Date=setting_checkDownloadIniFile.value("Date").toString();
    read_md5sum=setting_checkDownloadIniFile.value("md5sum").toString();
    read_Version=setting_checkDownloadIniFile.value("Version").toString();
    read_iniUpdate=setting_checkDownloadIniFile.value("iniUpdate").toString();
    sync();
    system("sync");
    syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION KONTROL %d %d %d ",updatedatabase,card_kontrol,checkinternet);
    if((updatedatabase==false) && (card_kontrol==false) && (checkinternet))
    {
        syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION GIRDI");
        if(var==1){
        syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION VAR=1");
            system(command1.toStdString().c_str());

            QFile file_check;
            file_check.setFileName(check_dowloadIniFile);
            if(!file_check.open(QIODevice::ReadOnly)){
                network_check_flg=0;

            }
            else {
                network_check_flg=1;
            }

            //            if(update_debug_flg==true){
            //                qDebug()<<"MyVersion"<<Myversion;
            //                qDebug()<<"MyVersionDate"<<MyVersionDate;
            //                qDebug()<<"ReadVersion"<<read_Version;
            //                qDebug()<<"read_Date"<<read_Date;
            //                qDebug()<<"read_IniUpdate"<<read_iniUpdate;
            //            }
            //qDebug()<<"update_isImmediate_flg"<<update_isImmediate_flg;
            if(update_isUpdated_flg==0)
            { // yeni version indirilmemisse
                if((read_Date!=MyVersionDate || read_Version!=Myversion) && (MyVersionDate.length()>0) && (Myversion.length()>0) && (read_Date.length()>0) && (read_Version.length()>0))
                {
                    syslog(LOG_DEBUG,"UPDATER_DEBUG Yeni versiyon indiriliyor.");
                    update_download_flg=1;
                    request_flg=2;

                }
            }
            else if(read_Date==MyVersionDate && read_Version==Myversion){
                syslog(LOG_DEBUG,"UPDATER_DEBUG Son Versiyon Yuklu");
                request_flg=1;
            }
            sync();
            system("sync");
        }
        else if(var==2){

            syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION VAR=2");
            update_download_flg=1;
            system(command2.toStdString().c_str());
            // güncelleme indiriliyor

            QFile file_zipFile;
            file_zipFile.setFileName(zipFile);
            QCryptographicHash calculateMD5(QCryptographicHash::Md5);
            if(!file_zipFile.open(QIODevice::ReadOnly))
                return;
            calculateMD5.addData(file_zipFile.readAll());
            if(read_md5sum==calculateMD5.result().toHex()){
                syslog(LOG_DEBUG,"UPDATER_DEBUG MD5 Ayni");
                //update_check_flg=1;
                //setting_checkMyIniFile.setValue("update_check_flg",1);
                sync();
                system("sync");
                update_download_flg=0; // indirme işlemi tamamlandı.
                update_isUpdated_flg=1;
                request_flg=3;
                update_count=0;
            }
            else{
                syslog(LOG_DEBUG,"UPDATER_DEBUG MD5 Ayni degil");
                update_count++;
                request_flg=1;
                file_zipFile.close();
                file_zipFile.remove();
                if(update_count > 5){
                    request_flg=5;
                }

            }
            sync();
            system("sync");
            file_zipFile.close();

        }

        else if(var==3){
            syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION VAR=3");
            system(command3.toStdString().c_str());
            system(command6.toStdString().c_str());

            //            setting_checkMyIniFile.setValue("MyVersionDate",read_Date);
            //            setting_checkMyIniFile.setValue("Myversion",read_Version);
            //          MyVersionDate=setting_checkMyIniFile.value("MyVersionDate").toString();
            //        Myversion=setting_checkMyIniFile.value("Myversion").toString();
            system("sync &");
            if(read_iniUpdate=="true"){
                request_flg=4;
            }
            else if(read_iniUpdate=="false"){
                request_flg=1;
            }
            setting_checkMyIniFile.setValue("update_check_flg",1);
            sync();
            system("sync");

            QFile file;
            file.setFileName(zipFile);
            file.remove();
            sync();
            system("sync");
        }
        else if(var==4){
            syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION VAR=4");
            system(command4.toStdString().c_str());
            syslog(LOG_DEBUG,"UPDATER_DEBUG Ini dosyasi guncellendi...");
            sync();
            system("sync");

        }
        else if (var==5) {
            syslog(LOG_DEBUG,"UPDATER_DEBUG VERSION VAR=5");
            syslog(LOG_DEBUG,"UPDATER_DEBUG Ini dosyasinda hata var, Indirme islemi yapilamiyor.");
            sync();
            system("sync");
            //versionTimer->start(7200000);
        }
    }
}

void updater::updatetoThread(QThread &updateThread)
{
//    connect(updaterTimer, SIGNAL(timeout()), this, SLOT(updater_routine()));
//    connect(conrolTimer,SIGNAL(timeout()),this,SLOT(control_routine()));
//    connect(versionTimer,SIGNAL(timeout()),this,SLOT(replay_versionFile()));
    connect(debugTimer,SIGNAL(timeout()),this,SLOT(debug_write()));
    connect(&updateThread,SIGNAL(started()),this,SLOT(update_timer_start()));

}

void updater::updater_routine()
{
    switch (request_flg) {
    case 1: // Dosya indirme kontrolu
        checkDownloadFile(1);
        break;
    case 2: // Dosya indir
        checkDownloadFile(2);
        break;
    case 3:
        checkDownloadFile(3); // zip ten çıkar
        break;
    case 4:
        checkDownloadFile(4); // ini dosyasını değiştir
        request_flg=1;
        break;
    case 5:
        checkDownloadFile(5);
        break;


    default:
        syslog(LOG_DEBUG,"UPDATER_DEBUG indirme İslemi basarili \n Bekleyiniz...");
        request_flg=1;
        break;
    }
}

void updater::update_timer_start()
{
   // updaterTimer->start(timeoutTime); // 3600000 // güncelleme olup olmadıgını kontrol eden
   // conrolTimer->start(5000); // program icindeki şartlara bakan
}

void updater::control_routine()
{
    syslog(LOG_DEBUG,"UPDATER_DEBUG GUNCELLEME KONTROL");
    update_isImmediate_flg=setting_checkDownloadIniFile.value("update_isImmediate","").toBool();

    currentTime=QDateTime::currentDateTime().toString("HH");
    currentTime1=currentTime.toInt(&ok);
    QSettings settingUpdate(SettingsFile,QSettings::IniFormat);

    sync();
    system("sync");
    update_check_flg=settingUpdate.value("update_check_flg").toInt();
    syslog(LOG_DEBUG,"UPDATER_DEBUG UPDATE CHECK FLAG = %d",update_check_flg);
    if(update_check_flg==1){
        if(update_isImmediate_flg==true){
            syslog(LOG_DEBUG,"UPDATER_DEBUG IMMEDIATE");
            if(Nozzle_up_flg==0){
                // güncelleme script 2 ;

                system("sync &");
                //update_check_flg=0;
                system(command5.toStdString().c_str());
                QSettings setting1(SettingsFile,QSettings::IniFormat);
                setting1.setValue("MyVersionDate",read_Date);
                setting1.setValue("Myversion",read_Version);
                setting1.setValue("update_check_flg",0);
                sync();
                system("sync");
                update_isUpdated_flg=0;
                system(command6.toStdString().c_str());
                sync();
                system("sync");
                system("shutdown -r now");

            }
        }
        else if(update_isImmediate_flg==0){
            if(currentTime1>=1 && currentTime1<=3){ // Saat bilgisi
                if(Nozzle_up_flg==0){
                    //  update_check_flg=0;
                    setting_checkMyIniFile.setValue("update_check_flg",0);
                    //güncelleme script 2 ;

                    system(command5.toStdString().c_str());
                    QSettings setting(SettingsFile,QSettings::IniFormat);
                    setting.setValue("MyVersionDate",read_Date);
                    setting.setValue("Myversion",read_Version);
                    setting.setValue("update_check_flg",0);
                    sync();
                    system("sync");
                    update_isUpdated_flg=0;
                    system(command6.toStdString().c_str());
                    sync();
                    system("sync");
                    system("shutdown -r now");
                }
            }
        }

    }
}

void updater::replay_versionFile()
{
    request_flg=1;
}

void updater::syslogcreate()
{
    system(command9.toStdString().c_str());
    system(command8.toStdString().c_str());
    system("chmod 777 /var/www/html/*");
}


void updater::debug_write()
{
    QFileInfo file("/var/www/html/syslog.txt");
    if(!file.exists())
    {
        syslogcreate();
    }
    else
    {
        if(file.size()>30000000)
        {
            syslogcreate();
        }
        else
        {
            quint64 file_size_fark=file.size()-file_size_last;
            if(file_size_fark<1000)
            {
                syslogcreate();
            }
        }
        QFileInfo file2("/var/www/html/syslog.txt");
        file_size_last=file2.size();
    }
}
