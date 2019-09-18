// mytcpsocket.cpp

#include "mytcpsocket.h"
#include "config.h"
#include "global.h"
#include <QCryptographicHash>


QByteArray data;
QString whitelist_version_ini;
QString person_version_ini;
QString whitelist_version_usc;
QString person_version_usc;
QJsonObject sales_object;
int dcr_adress=1;
int message=0;
int close_socket_count=0;
bool usc=true;
bool connection=false;
void saveUnitPrice(quint8 NZL);
int dec_to_bcd(int dec);
QByteArray convert_format(QByteArray data,unsigned char dot_ind);
MyTcpSocket::MyTcpSocket(QObject *parent) :
    QObject(parent)
{
    database=new Database();
    tcptimer = new QTimer(this);
    configuration=false;
    whitelist_version=false;
    person_version=false;
    //QSettings settings(SettingsFile, QSettings::IniFormat);
    whitelist_all=settings.value("whitelist_all").toBool();
    sales_send=true;
}

void MyTcpSocket::doConnect()
{
    Crypto  Cryptlock;
    //QSettings settings(SettingsFile, QSettings::IniFormat);
    QByteArray Bytearraysaved_day=QByteArray::fromHex(settings.value("saved_day").toByteArray());
    //qDebug()<<"saved_day_debug"<<Bytearraysaved_day.toHex();
    Bytearraysaved_day=QString::fromStdString(Cryptlock.decrypt(QString::fromLatin1(Bytearraysaved_day).toStdString().c_str())).toLatin1();
    if(test)qDebug()<<"saved_day"<<QString::fromUtf8(Bytearraysaved_day)<<QDate::currentDate().toString("dd.MM.yyyy");

    if(QString::fromUtf8(Bytearraysaved_day).length()>8)
    {
        if(test)qDebug()<<"saved_day-2"<<QString::fromUtf8(Bytearraysaved_day).length();
        int year=QString::fromUtf8(Bytearraysaved_day).split(".").at(2).toInt();
        int month=QString::fromUtf8(Bytearraysaved_day).split(".").at(1).toInt();
        int day=QString::fromUtf8(Bytearraysaved_day).split(".").at(0).toInt();


        QDate dnow(QDate::currentDate());
        QDate enow(year,month,day);

        if(test) qDebug()<<"Today is :"<<dnow.toString("dd.MM.yyyy")<<"Days :"<<dnow.daysTo(enow);

        //    if(dnow.daysTo(enow)<0)
        //    {
        //        if(-1*dnow.daysTo(enow)>=lock_day)
        //        {
        //            system_lock=true;
        //        }
        //    }
        //    else
        //    {
        //        if(dnow.daysTo(enow)>=lock_day)
        //        {
        //            system_lock=true;
        //        }
        //    }

    }
    if(usc)
    {
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
        connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));

        //connect(socket, SIGNAL(connected()),this, SLOT(connected()));
        if(test) qDebug()<<"TCP-1"<<socket->state();
        getverison_count++;
        if(getverison_count>=3){connection_is_exist=false; getverison_count=0;}
        if(socket->state()==QTcpSocket::UnconnectedState)
        {
            if(test) qDebug()<<"connecting....";
            socket->connectToHost(usc_ip, usc_port.toInt());
            if(test) qDebug()<<"TCP-2"<<socket->state();
            if(!socket->waitForConnected(1000))
            {
                if(test)  qDebug() << "Error: " << socket->errorString();
                socket->disconnect();
            }
            else
            {
                if(test) qDebug()<<"manual connected";
                connected();
            }
            if(test) qDebug()<<"TCP-3"<<socket->state();
        }
        else
        {
            if(test) qDebug()<<"already connected....";
        }
    }
}
void MyTcpSocket::write(QByteArray bytes)
{
    Crypto Crypt;
    bytes=QString::fromStdString(Crypt.encrypt(QString::fromLatin1(bytes).toStdString().c_str())).toLatin1();
    socket->write(bytes);
}


void MyTcpSocket::getversion()
{
    QJsonObject get_version;
    get_version["Command"]=QString::fromStdString("GETVERSION");
    QJsonDocument doc(get_version);
    QByteArray bytes=doc.toJson(QJsonDocument::Compact);
    if(test) qDebug()<<"GETVERSION ="<<bytes;
    write(bytes);
}

void MyTcpSocket::getconfig()
{
    QJsonObject get_config;
    get_config["Command"]=QString::fromStdString("GETCONFIGURATION");
    QJsonDocument doc(get_config);
    QByteArray bytes=doc.toJson(QJsonDocument::Compact);
    if(test) qDebug()<<"GETCONFIGURATION ="<<bytes;
    write(bytes);
}

void MyTcpSocket::getperson()
{
    QJsonObject get_person;
    get_person["Command"]=QString::fromStdString("GETPERSON");
    QJsonDocument doc(get_person);
    QByteArray bytes=doc.toJson(QJsonDocument::Compact);
    if(test) qDebug()<<"GETPERSON ="<<bytes;
    write(bytes);
}

void MyTcpSocket::getwhiteversion()
{
    QJsonObject get_whiteversion;
    get_whiteversion["Command"]=QString::fromStdString("GETWHITELIST");
    QJsonDocument doc(get_whiteversion);
    QByteArray bytes=doc.toJson(QJsonDocument::Compact);
    if(test) qDebug()<<"GETWHITELIST ="<<bytes;
    write(bytes);
}

void MyTcpSocket::getwhiteall()
{
    QJsonObject get_whiteall;
    get_whiteall["Command"]=QString::fromStdString("GETALLWHITELIST");
    QJsonDocument doc(get_whiteall);
    QByteArray bytes=doc.toJson(QJsonDocument::Compact);
    if(test) qDebug()<<"GETALLWHITELIST ="<<bytes;
    write(bytes);
}

void MyTcpSocket::sendsales()
{
    sales_object=database->kontrolSalesStatus();
    if(sales_object.value("SALES").toArray().size()>0)
    {
        QFileInfo send_json_info("/var/www/html/send_json.txt");
        if(send_json_info.size()>10000000)
        {
            system("rm -f /var/www/html/send_json.txt");
        }
        QJsonDocument doc(sales_object);
        QFile send_json("/var/www/html/send_json.txt");
        if(send_json.open(QFile::Append))
        {
            QTextStream in(&send_json);
            in<<doc.toJson(QJsonDocument::Compact);
        }
        send_json.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        send_json.close();
        sync();
        QByteArray bytes=doc.toJson(QJsonDocument::Compact);
        if(test)qDebug()<<"SALES ="<<bytes;
        write(bytes);
    }
    else
    {
        sales_send=false;
    }
}

void MyTcpSocket::saveversion()
{
    Crypto  Cryptversion;
    QJsonArray Json_unit_prices_array;
    if(test) qDebug()<<"Version ="<<data;
    QJsonParseError parseerror;
    QJsonDocument doc;
    QJsonObject json;
    doc = QJsonDocument::fromJson(data,&parseerror);
    if(parseerror.error <= 0) {
        json=doc.object();
        QString Control_data=json.value("CONTROL").toString();
        QTime control_hour=QTime::currentTime();
        QString hour=control_hour.toString("hh");
        QString date=json.value("DateTime").toString();
        if(test) qDebug()<<"HOUR :"<<hour;
        if(hour!=date.mid(9,2))
        {
            if(test) qDebug()<<"UPDATETIME";
            QProcess proc;
            proc.start("date -s \""+date.mid(2,2)+"/"+date.mid(0,2)+"/"+date.mid(4,4)+" "+date.mid(9,2)+":"+date.mid(11,2)+":00"+"\"");
            proc.waitForFinished();
            system("hwclock -w");
        }
        if(test) qDebug()<<"Date :"<<date;
        if(test) qDebug()<<"Control_data"<<Control_data.toLatin1();
        Json_unit_prices_array=json.value("PRICES").toArray();
        if(test) qDebug()<<"Json_unit_prices"<<Json_unit_prices_array.at(0).toDouble();
        person_version_usc=json.value("PersonVersion").toString();
        if(test) qDebug()<<"person_version_usc"<<person_version_usc;
        whitelist_version_usc=json.value("WhiteListVersion").toString();
        if(test) qDebug()<<"whitelist_version_usc"<<whitelist_version_usc;

        // nozzle yerindeyse  ve configuraston tamamsa gelen birim fiyatları kabul et
        if((Nozzle_up_flg==0) && (config_error==false))
        {
            for(int i=0;i<Json_unit_prices_array.size();i++)
            {
                if((pump[pumpNo].PrdctNozzleNo[i+1]<=Number_of_Nozzle) &&(pump[pumpNo].PrdctNozzleNo[i+1]>0)) // o productın nozzle nosu
                {
                    if(test) qDebug()<<"Setting_ini_Uprice"<<Unit_price_for_save[pump[pumpNo].PrdctNozzleNo[i+1]];
                    if(test) qDebug()<<"Json_New_Unit_price"<<pump[pumpNo].PrdctNozzleNo[i+1]<<QByteArray::number(Json_unit_prices_array.at(i).toDouble());
                    if(Unit_price_for_save[pump[pumpNo].PrdctNozzleNo[i+1]]!=QByteArray::number(Json_unit_prices_array.at(i).toDouble()))
                    {
                        Unit_price_for_save[pump[pumpNo].PrdctNozzleNo[i+1]]=QByteArray::number(Json_unit_prices_array.at(i).toDouble());
                        if(test) qDebug()<<"Json_New_unit_prices_saved"<<pump[pumpNo].PrdctNozzleNo[i+1]<<Unit_price_for_save[pump[pumpNo].PrdctNozzleNo[i+1]];
                        saveUnitPrice(pump[pumpNo].PrdctNozzleNo[i+1]);
                        qDebug()<<"CAGIR111";
                        Unit_price=convert_format(Unit_price_for_save[pump[pumpNo].PrdctNozzleNo[i+1]],unit_price_dot_index);
                        qDebug()<<"CAGIR111--";
                        dec_to_bcd(Unit_price.toInt()); // 4 byte 00 00 12 34
                        pump[pumpNo].nozzle[pump[pumpNo].PrdctNozzleNo[i+1]].unitprice[0]=bcd[2];
                        pump[pumpNo].nozzle[pump[pumpNo].PrdctNozzleNo[i+1]].unitprice[1]=bcd[1];
                        pump[pumpNo].nozzle[pump[pumpNo].PrdctNozzleNo[i+1]].unitprice[2]=bcd[0];
                    }
                }
            }
        }
        //QSettings settings(SettingsFile, QSettings::IniFormat);
        QDate control_date=QDate::currentDate();
        QByteArray hash=QCryptographicHash::hash(control_date.toString("ddMMyyyy").toLatin1().toPercentEncoding(),QCryptographicHash::Sha1);
        if(test)qDebug()<<"hash"<<hash.toHex();
        if(hash.toHex()==Control_data.toLatin1())
        {
            pump_lock_count=0;
            saved_day=QDate::currentDate().toString("dd.MM.yyyy");
            QByteArray Bytearraysaved_day=QString::fromStdString(Cryptversion.encrypt(saved_day.toStdString())).toLatin1();
            if(test) qDebug()<<"saved_day"<<Bytearraysaved_day;
            settings.setValue("saved_day",Bytearraysaved_day.toHex());
            sync();
            system("sync");
            system_lock=false;

            if(LCD_status==13 && licenceCheck==0)
            {
                LCD_status=lock_status;
                cannotchange_frame=false;
            }

        }

        Version_config_ini=settings.value("Version_config").toString();
        if(test) qDebug()<<"Version_config_ini"<<Version_config_ini;
        whitelist_version_ini=settings.value("whitelist_version").toString();
        if(test) qDebug()<<"whitelist_version_ini"<<whitelist_version_ini;
        person_version_ini=settings.value("person_version").toString();
        if(test) qDebug()<<"person_version_ini"<<person_version_ini;


        if(Version_config_ini!=json.value("Version").toString())
        {
            configuration=true;

        }
        else
        {
            configuration=false;
        }
        if(whitelist_version_ini!=json.value("WhiteListVersion").toString())
        {
            whitelist_version=true;

        }
        else
        {
            whitelist_version=false;
        }
        if(person_version_ini!=json.value("PersonVersion").toString())
        {
            person_version=true;

        }
        else
        {
            person_version=false;
        }


    }
    else
    {
        if(test) qDebug()<<"saveversion-Json validation Error : "<<parseerror.errorString();
    }
}

void MyTcpSocket::saveconfig()
{
    virtual_pump=false;
    int nozzle_counter=0;
    if(test) qDebug()<<"Configuration ="<<data;
    QJsonParseError parseerror;
    QJsonDocument doc;
    QJsonObject json;
    doc = QJsonDocument::fromJson(data,&parseerror);
    if(parseerror.error <= 0)
    {
        json=doc.object();
        Version_config_ini=json.value("Version").toString();
        if(test) qDebug()<<"Version_config_json"<<Version_config_ini;
        QJsonArray nozzle_array= json["NOZZLE"].toArray();
        database->delete_table("NOZZLE");
        database->Open_Nozzles_Database();
        for(int i=0;i<nozzle_array.size();i++)
        {
            QJsonObject nozzle=nozzle_array.at(i).toObject();
            int tank_no=nozzle.value("TANK").toInt();
            int nozzle_no=nozzle.value("NozzleNo").toInt();
            int real_nozzle_no=nozzle.value("RealNozzleNo").toInt();
            int pump_no=nozzle.value("PUMP").toInt();
            QJsonArray pump_array= json["PUMP"].toArray();
            for(int m=0;m<pump_array.size();m++)
            {
                QJsonObject pump=pump_array.at(m).toObject();
                if(ABU_address==pump.value("DCR").toInt() && pump_no==pump.value("PumpNo").toInt())
                {
                    if(virtual_pump==false)
                    {
                        virtual_pumpno=pump_no;
                        //QSettings settings(settingPath, QSettings::IniFormat);
                        settings.setValue("virtual_pumpno",virtual_pumpno);
                        sync();
                        system("sync");
                        virtual_pump=true;
                    }
                    QJsonArray tank_array= json["TANK"].toArray();
                    for(int j=0;j<tank_array.size();j++)
                    {
                        QJsonObject tank=tank_array.at(j).toObject();
                        if(tank_no==tank.value("TankNo").toInt())
                        {
                            int fuel_no=tank.value("FUEL").toInt();
                            QJsonArray fuel_array= json["FUEL"].toArray();
                            for(int k=0;k<fuel_array.size();k++)
                            {
                                QJsonObject fuel=fuel_array.at(k).toObject();
                                if(fuel_no==fuel.value("FuelNo").toInt())
                                {
                                    if(nozzle_counter<Number_of_Nozzle)
                                    {
                                        settings.setValue("Virtual_nozzle"+QString::number(real_nozzle_no), nozzle_no);
                                        if(test) qDebug()<<"InsertNozzle";
                                        database->InsertNozzle(real_nozzle_no,fuel.value("FuelNo").toInt(),tank_no,QString::number(fuel.value("Price").toDouble()),
                                                               QString::number(fuel.value("MaxPrice").toDouble()),QString::number(fuel.value("MinPrice").toDouble()));
                                        nozzle_counter++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        //QSettings settings(settingPath, QSettings::IniFormat);

        if(database->NozzleValues())
        {
            settings.setValue("Version_config", Version_config_ini);
            sync();
            system("sync");
            if(test) qDebug()<<"Config_values_Read";
            config_error=false;
            TEXT_fromMaster="              ";
            cannotchange_frame=false;
        }
        else
        {   Version_config_ini="0";
            settings.setValue("Version_config", Version_config_ini);
            sync();
            system("sync");
            if(test) qDebug()<<"Config_values_error";
        }
        database->Close_Nozzle_Database();
        configuration=false;

    }
    else
    {
        if(test) qDebug()<<"saveconfig-Json validation Error : "<<parseerror.errorString();
    }


}

void MyTcpSocket::saveperson()
{
    if(test) qDebug()<<"PERSON ="<<data;

    QJsonParseError parseerror;
    QJsonDocument doc;
    QJsonObject json;
    doc = QJsonDocument::fromJson(data,&parseerror);
    if(parseerror.error <= 0) {
        database->delete_table("PERSON");
        database->Open_Person_Database();
        int count=1;
        json=doc.object();
        QJsonObject json_value=json.value("VALUES").toObject();
        QJsonArray person_array=json_value["PERSONS"].toArray();
        if(test) qDebug()<<"person_version_usc++"<<person_version_usc;
        for(int i=0;i<person_array.size();i++)
        {
            QJsonObject person=person_array.at(i).toObject();
            database->InsertPersonRfid(count,person.value("RfID").toString(),person.value("Attendant").toString(),person.value("DGroupNo").toInt());
            count++;
        }
        //QSettings settings(settingPath, QSettings::IniFormat);
        settings.setValue("person_version", person_version_usc);
        sync();
        system("sync");
        database->Close_Person_Database();
        person_version=false;
    }
    else
    {
        if(test) qDebug()<<"saveperson-Json validation Error : "<<parseerror.errorString();
    }


}

void MyTcpSocket::savewhiteversion()
{

    if(test) qDebug()<<"WHITELISTVERSION ="<<data;
    database->Open_Whitelist_Database();
    QStringList whitelist_array_new=QString::fromUtf8(data).split('\n');
    int count=database->CountId("WHITELIST");
    for(int i=0;i<whitelist_array_new.length()-1;i++)
    {
        QStringList whitelist_array=whitelist_array_new.at(i).split('&');
        QString central_price;

        if(whitelist_array.at(13)=="2")
        {
            database->Delete_ServerId(whitelist_array.at(0));
        }
        else
        {
            if(database->kontrol_ServerId(whitelist_array.at(0)))
            {
                if(whitelist_array.at(8)=="True"){central_price="1";}else{central_price="0";}
                database->UpdateWhitelist(whitelist_array.at(0),whitelist_array.at(1),whitelist_array.at(2),
                                          whitelist_array.at(3),whitelist_array.at(4),whitelist_array.at(5),
                                          whitelist_array.at(6),whitelist_array.at(7),central_price,
                                          whitelist_array.at(9),whitelist_array.at(10),whitelist_array.at(11),
                                          whitelist_array.at(12),whitelist_array.at(13),whitelist_array.at(14),
                                          whitelist_array.at(15));//chn
            }
            else
            {

                count++;
                if(whitelist_array.at(8)=="True"){central_price="1";}else{central_price="0";}
                database->InsertWhitelist(count,whitelist_array.at(0),whitelist_array.at(1),whitelist_array.at(2),
                                          whitelist_array.at(3),whitelist_array.at(4),whitelist_array.at(5),
                                          whitelist_array.at(6),whitelist_array.at(7),central_price,
                                          whitelist_array.at(9),whitelist_array.at(10),whitelist_array.at(11),
                                          whitelist_array.at(12),whitelist_array.at(13),whitelist_array.at(14),whitelist_array.at(15),
                                          whitelist_array.at(16));//chn
            }
        }
    }
    database->Close_Whitelist_Database();
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("whitelist_version", whitelist_version_usc);
    sync();
    system("sync");
    whitelist_version=false;
}

void MyTcpSocket::savewhiteall()
{
    if(test)qDebug()<<"WHITELISTALL ="<<data;

    QJsonParseError parseerror;
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(data,&parseerror);
    if(parseerror.error <= 0) {
        database->delete_table("WHITELIST");
        int count=1;
        database->Open_Whitelist_Database();
        QJsonArray whitelist_array=doc.array();
        if(test) qDebug()<<"SIZEEE ="<<whitelist_array.size();
        qDebug()<<"SIZEEE ="<<whitelist_array.size();

        for(int i=0;i<whitelist_array.size();i++)
        {
            QJsonObject whitelist=whitelist_array.at(i).toObject();
            database->InsertWhitelist(count,QString::number(whitelist.value("ServerID").toInt()),whitelist.value("RFID").toString(),
                                      whitelist.value("Plate").toString(),whitelist.value("CustomerName").toString(),
                                      whitelist.value("Products").toString(),whitelist.value("CardType").toString(),
                                      whitelist.value("ListType").toString(),QString::number(whitelist.value("OfflineLimit").toInt()),
                                      QString::number(whitelist.value("CentralPrice").toBool()),whitelist.value("DiscountGroupNo").toString(),
                                      whitelist.value("PromotionGroupNo").toString(),whitelist.value("AuthorizationType").toString(),
                                      whitelist.value("CardPassword").toString(),whitelist.value("Event").toString(),
                                      whitelist.value("EventTime").toString(),whitelist.value("BalanceLimit").toString(),
                                      whitelist.value("vehiclekmtr").toString()); //30.05.2019 chn --vehiclekm
            count++;
        }
        database->Close_Whitelist_Database();
        whitelist_all=false;
        whitelist_version=true;
        //QSettings settings(settingPath, QSettings::IniFormat);
        settings.setValue("whitelist_all", whitelist_all);
        sync();
        system("sync");

    }
    else
    {
        if(test) qDebug()<<"savewhiteall-Json validation Error : "<<parseerror.errorString();
    }
}

void MyTcpSocket::savesales()
{
    if(test)qDebug()<<"Save ="<<data;


    QJsonParseError parseerror;
    QJsonDocument doc;
    QJsonObject json;
    doc = QJsonDocument::fromJson(data,&parseerror);
    if(parseerror.error <= 0) {
        json=doc.object();
        bool test=json.value("Result").toBool();
        if(test)
        {
            QJsonArray array_sales= sales_object["SALES"].toArray();
            for(int i=0;i<array_sales.size();i++)
            {
                QJsonObject json2=array_sales.at(i).toObject();
                database->UpdateStatus(json2.value("SLS_ID").toString());
            }
        }
        else
        {
            QJsonArray array_sales= sales_object["SALES"].toArray();
            QJsonArray array_saleserror=json.value("ERRORS").toArray();
            for(int i=0;i<array_sales.size();i++)
            {
                QJsonObject json_sales=array_sales.at(i).toObject();
                kontrol_id=false;
                for(int j=0;j<array_saleserror.size();j++)
                {
                    if(json_sales.value("SLS_ID").toString()==array_saleserror.at(j).toString())
                    {
                        kontrol_id=true;
                        break;
                    }
                }
                if(kontrol_id)
                {
                    database->UpdateStatus2(json_sales.value("SLS_ID").toString());
                }
                else
                {
                    database->UpdateStatus(json_sales.value("SLS_ID").toString());
                }
            }
        }
    }
    else
    {
        if(test) qDebug()<<"savesales-Json validation Error : "<<parseerror.errorString();
    }

}



void MyTcpSocket::connected()
{

    message=0;

    if(person_version)
    {
        message=2;
    }
    if(whitelist_version)
    {
        message=3;
    }
    if(whitelist_all)
    {
        message=4;
    }
    if(sales_send)
    {
        message=5;
    }
    if(configuration)
    {
        message=1;
    }
    switch (message)
    {
    case 1:
        getconfig();
        break;
    case 2:
        getperson();
        break;
    case 3:
        getwhiteversion();
        break;
    case 4:
        getwhiteall();
        break;
    case 5:
        sendsales();
        break;
    default:
        getversion();
        break;
    }
}

void MyTcpSocket::disconnected()
{
    if(close_socket_count<7)
    {
        Crypto Crypt;
        data=QString::fromStdString(Crypt.decrypt(QString::fromUtf8(data).toStdString())).toUtf8();
        getverison_count=0;
        connection_is_exist=true;
        if(test) qDebug()<<"Server disconnect";
        close_socket_count=0;
        connection=false;
        switch (message) {
        case 1:
            saveconfig();
            break;
        case 2:
            saveperson();
            break;
        case 3:
            savewhiteversion();
            break;
        case 4:
            savewhiteall();
            break;
        case 5:
            savesales();
            break;
        default:
            saveversion();
            break;
        }
    }
    else
    {
        if(test) qDebug()<<"Client disconnect";
    }
    socket->deleteLater();
    data.clear();
}

void MyTcpSocket::bytesWritten(qint64 bytes)
{
    if(test) qDebug() << bytes << " bytes written...";
}

void MyTcpSocket::readyRead()
{
    data=socket->readAll();
    //if(test) qDebug()<<"read_data"<<data;
    while(socket->waitForReadyRead(1000)){
        data.append(socket->readAll());
        //qDebug()<<"read_data_1"<<data;
    }

}

void MyTcpSocket::TcptoThread(QThread &TcpThread)
{
    connect(tcptimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(&TcpThread,SIGNAL(started()),this,SLOT(tcp_scan_timer()));

}

void MyTcpSocket::update()
{
    if((Nozzle_up_flg==0) && (protect_flg==false) && (saving_the_sales!=true))
    {

        if(connection)
        {
            if(test) qDebug()<<"close_socket_count :"<<QString::number(close_socket_count);
            close_socket_count++;
        }
        else
        {
            sales_send_count++;
            if(sales_send_count%181==0)
            {
                sales_send=true;
                doConnect();
                connection=true;
            }
            else
            {
                if((ABU_addr-79)>4)
                {
                    if(sales_send_count%(411+(((ABU_addr-79)-4)*26))==0)
                    {
                        doConnect();
                        connection=true;
                    }

                }
                else
                {
                    if(sales_send_count%(517+((ABU_addr-79)*16))==0)
                    {
                        doConnect();
                        connection=true;
                    }

                }

            }
        }
        if(close_socket_count>=7)
        {
            if(test) qDebug()<<"Close Connection";
            socket->close();
            socket->deleteLater();
            close_socket_count=0;
            connection=false;
        }
        if(sales_send_count>=30000)
        {sales_send_count=1;}
    }
}

void MyTcpSocket::tcp_scan_timer()
{
    tcptimer->start(1000);

}
