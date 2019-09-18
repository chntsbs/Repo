#include "database.h"
#include <QtWidgets>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "global.h"
#include "QtMath"
#include "syslog.h"
#include "unistd.h"
#include "mainwindow.h"
QSqlDatabase nozzle;
QSqlDatabase whitelist;
QSqlDatabase person;
QSqlDatabase plates;
int dec_to_bcd(int dec);
bool mts_flg=false;
QSqlDatabase constructor;
QByteArray convert_format(QByteArray data,unsigned char dot_ind);
Database::Database()
{
    constructor = QSqlDatabase::database("constructor");

    if(!constructor.open())
    {
        settings.setlog("DATABASE_DEBUG constructor-Error: Connection with database fail");
    }
    else
    {
        settings.setlog("DATABASE_DEBUG Databe:constructor Connection ok");
        QSqlQuery query(constructor);
        query.exec("PRAGMA journal_mode=WAL");
        query.prepare("CREATE TABLE IF NOT EXISTS WHITELIST (WHITE_ID NUMBER,SERVER_ID TEXT,WHITE_RFID TEXT,WHITE_PLATE TEXT,CUSTOMER_NAME TEXT,WHITE_PRODUCTS TEXT,CARD_TYPE TEXT,LIST_TYPE TEXT,OFFLINE_LIMIT NUMBER,CENTRAL_PRICE TEXT,DISCOUNT_GROUP TEXT,PROMOTION_GROUP TEXT,AUTHORIZATION_TYPE TEXT,CARD_PASSWORD TEXT,EVENT TEXT,EVENT_TIME TEXT,STATUS TEXT,BALANCE_LIMIT TEXT,VEHICLE_KM NUMBER)");//CHN --vehicle km
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS PLATES (PlateID NUMBER,Plate TEXT,CustomerName TEXT,ProductID NUMBER,OfflineLimit TEXT,Status TEXT,BalanceLimit TEXT,VehicleKM NUMBER)");//CHN
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS PERSON (PERSON_ID NUMBER,PERSON_RFID TEXT,PERSON_NAME TEXT,GROUP_NO NUMBER,STATUS NUMERIC)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS NOZZLE (NOZZLE_ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,PRODUCT_ID NUMBER,TANK_NO NUMBER,PRODUCT_PRICE TEXT,MAX_UPRICE TEXT,MIN_UPRICE TEXT)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS SALES  (SALES_ID TEXT, TRNO NUMBER, RNO NUMBER, SALES_DATE TEXT,SALES_CLOCK TEXT,PUMP NUMBER, NOZZLE NUMBER, PRODUCT_N0 NUMBER, TANK_N0 NUMBER, UNIT_PRC TEXT,SALES_LITER TEXT,SALES_AMOUNT TEXT, BEGIN_TOTAL_AMOUNT TEXT, END_TOTAL_AMOUNT TEXT,BEGIN_TOTAL_VOLUME TEXT,  END_TOTAL_VOLUME TEXT, FILLING_TYPE NUMBER, CARD_TYPE NUMBER,AUTHORIZE_TYPE NUMBER,SALES_TYPE NUMBER,PAYMENT_TYPE NUMBER, CustomerRFID TEXT, DriverRFID TEXT, PersonRFID TEXT, PLATE TEXT, VEHICLEKM TEXT,VEHICLERUNTIME TEXT,NOZZLE_UP_TIME TEXT,NOZZLE_DOWN_TIME TEXT, RFID_READ_TIME TEXT, RFID_RESULT_TIME TEXT, AUTHORIZE_TIME TEXT, FILLING_START_TIME TEXT, PAID_CONFIRM_TIME TEXT, PAID_FISCAL_TIME TEXT, STATUS NUMBER)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS SALES2 (SALES_ID TEXT, TRNO NUMBER, RNO NUMBER, SALES_DATE TEXT,SALES_CLOCK TEXT,PUMP NUMBER, NOZZLE NUMBER, PRODUCT_N0 NUMBER, TANK_N0 NUMBER, UNIT_PRC TEXT,SALES_LITER TEXT,SALES_AMOUNT TEXT, BEGIN_TOTAL_AMOUNT TEXT, END_TOTAL_AMOUNT TEXT,BEGIN_TOTAL_VOLUME TEXT,  END_TOTAL_VOLUME TEXT, FILLING_TYPE NUMBER, CARD_TYPE NUMBER,AUTHORIZE_TYPE NUMBER,SALES_TYPE NUMBER,PAYMENT_TYPE NUMBER, CustomerRFID TEXT, DriverRFID TEXT, PersonRFID TEXT, PLATE TEXT, VEHICLEKM TEXT,NOZZLE_UP_TIME TEXT,NOZZLE_DOWN_TIME TEXT, RFID_READ_TIME TEXT, RFID_RESULT_TIME TEXT, AUTHORIZE_TIME TEXT, FILLING_START_TIME TEXT, PAID_CONFIRM_TIME TEXT, PAID_FISCAL_TIME TEXT, STATUS NUMBER)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS PROBE (PROBE_PRODUCT TEXT, PROBE_ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, PROBE_ADDRESS TEXT, PROBE_STATUS TEXT, PROBE_FUELLEVEL TEXT,PROBE_WATERLEVEL TEXT,PROBE_TEMPERATURE TEXT,PROBE_TANK_BOY NUMBER,PROBE_OFFSET NUMBER,MIN_LEVEL NUMBER, MAX_LEVEL NUMBER, MIN_TEMPERATURE NUMBER, MAX_TEMPERATURE NUMBER, PROBE_WATERMAXLEVEL TEXT, PROBE_WATERMINLEVEL TEXT, PROBE_WATEROFFSET TEXT)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS SETTING (ID NUMBER, CARD_ADDRESS TEXT, AMOUNT NUMBER, VOLUME NUMBER, UNIT_PRICE NUMBER,LINE_BREAK TEXT,LINE_BREAK2 TEXT,TITLE TEXT,TITLE2 TEXT,KM_REPORT TEXT,SELLING_STYLE TEXT,PLATE_CONTROL TEXT)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS PRODUCT (ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,PRODUCT_ID NUMBER,NEW_UNIT_PRICE TEXT,OLD_UNIT_PRICE TEXT,PRODUCT_NAME TEXT,TANK_NO NUMBER)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS USER (user_id NUMBER,user_name TEXT,user_pass TEXT,authority NUMBER)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS STATUS (STATUS_ID NUMBER,STATUS_EN TEXT,STATUS_TR TEXT)");
        query.exec();
        query.prepare("CREATE TABLE IF NOT EXISTS TANK (LEVEL NUMERIC,VALUE NUMBERIC)");
        query.exec();
        constructor.commit();
        constructor.close();

    }


    system("chmod 777 /var/www/html/*");
}

void Database::insertToStatus()
{
    constructor.open();
    if(constructor.isOpen()){
        QSqlQuery query(constructor);
        query.prepare("Delete from STATUS");
        query.exec();
        query.prepare("Insert into STATUS (STATUS_ID, STATUS_EN, STATUS_TR) VALUES (0,'PASSIVE','PASİF')");
        query.exec();
        query.prepare("Insert into STATUS (STATUS_ID, STATUS_EN, STATUS_TR) VALUES (1,'ACTIVE','AKTİF')");
        query.exec();
        query.prepare("Insert into STATUS (STATUS_ID, STATUS_EN, STATUS_TR) VALUES (2,'DEAD','DEAD')");
        query.exec();
    }
    else{
        if(test)qDebug()<<"Fail insert to status -- Cannot open db file !!";
    }

    constructor.commit();
    constructor.close();
}

void Database::UpdateProbe(QString adres, QString status, QString fuel_level, QString water_level, QString temperature)
{   //veritabınında Probe bilgilerini günceller.
    QSqlDatabase update_probe = QSqlDatabase::database("update_probe");
    if(update_probe.open())
    {
        QSqlQuery query(update_probe);
        query.prepare("UPDATE PROBE SET PROBE_STATUS=:status, PROBE_FUELLEVEL=:fuel_level, PROBE_WATERLEVEL=:water_level, PROBE_TEMPERATURE=:temperature WHERE PROBE_ADDRESS=:adres ");
        query.bindValue(":status",status);
        query.bindValue(":fuel_level",fuel_level);
        query.bindValue(":water_level",water_level);
        query.bindValue(":adres",adres);
        query.bindValue(":temperature",temperature);
        query.exec();
        update_probe.commit();
        update_probe.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzleProduct-Error Database";
    }
}


void Database::delete_first_sales()
{   //İlk Satışı siler yeni gelene yer açar.
    QSqlDatabase delete_tablee = QSqlDatabase::database("delete_table");
    QString sales_id;
    if(delete_tablee.open())
    {
        QSqlQuery query(delete_tablee);
        query.prepare("SELECT SALES_ID FROM SALES LIMIT 1");
        query.exec();
        while(query.next())
        {
            sales_id=query.value(0).toString();
        }
        query.clear();
        query.prepare("DELETE FROM SALES WHERE SALES_ID=:sales_id");
        query.bindValue(":sales_id",sales_id);
        query.exec();
        delete_tablee.commit();
        delete_tablee.close();
        sync();
        system("sync");
    }
    else
    {
        if(test)qDebug()<<"delete_table-Error Database";
        settings.setlog("DATABASE_DEBUG delete_table-Error Database");
    }
}


int Database::CountId(QString tablo)
{   //Herhangi bir tablonun kayıt sayısını döner.
    QSqlDatabase count_id = QSqlDatabase::database("count_id");
    int count=0;
    if(count_id.open())
    {
        QSqlQuery query(count_id);
        query.prepare("SELECT COUNT(*) FROM "+tablo);
        query.exec();
        while(query.next())
        {
            count=query.value(0).toInt();
        }
        count_id.commit();
        count_id.close();
        sync();
        system("sync");
    }
    else
    {
        if(test)qDebug()<<"CountId-Error Database";
        settings.setlog("DATABASE_DEBUG CountId-Error Database");

    }
    return count;
}

void Database::delete_table(QString Tname)
{   //tabloyu siler.
    QSqlDatabase delete_tablee = QSqlDatabase::database("delete_table");
    if(delete_tablee.open())
    {
        QSqlQuery query(delete_tablee);
        query.exec("DELETE FROM '"+Tname+"'");
        delete_tablee.commit();
        delete_tablee.close();
        sync();
        system("sync");
    }
    else
    {
        if(test)qDebug()<<"delete_table-Error Database";
        settings.setlog("DATABASE_DEBUG delete_table-Error Database");
    }
}


void Database::Open_Whitelist_Database()
{   //whitelist databasei açar.
    whitelist = QSqlDatabase::database("whitelist");
    if(!whitelist.isOpen())
        whitelist.open();
}

void Database::Close_Whitelist_Database()
{   //whitelist databasei kapatır.
    whitelist = QSqlDatabase::database("whitelist");
    if(whitelist.isOpen())
    {
        whitelist.commit();
        whitelist.close();
    }
}

void Database::OrderWhitelist()
{   //Whitelist i offline limite göre sıralar.
    QSqlQuery query(whitelist);
    if(whitelist.isOpen())
    {
        query.prepare("SELECT * FROM WHITELIST ORDER BY OFFLINE_LIMIT");
        if(test)qDebug()<<"OrderWhitelist"<<query.lastError();
        query.exec();
    }
}

void Database::updateBalanceLimit(QString id, QString balanceLmt)
{
    if(test)qDebug()<<"UPDATEE BALANCE LIMIT ";
    settings.setlog("CustomerID"+CustomerID);
    settings.setlog("balanceLmt"+balanceLmt);
    Open_Whitelist_Database();
    if(whitelist.isOpen()){
        QSqlQuery query(whitelist);
        if(plateDB==false){
        query.prepare("Update WHITELIST SET BALANCE_LIMIT=:balanceLmt Where WHITE_RFID=:id");
        query.bindValue(":balanceLmt",balanceLmt);
        query.bindValue(":id",id);
        } else {
            query.prepare("Update PLATES SET BalanceLimit=:balanceLmt Where Plate=:PLAKA");
            query.bindValue(":balanceLmt",balanceLmt);
            query.bindValue(":PLAKA",id);
        }
        if(query.exec()>0){
            if(test)qDebug()<<"Update Balance Limit is OK!";
        }
        else{
            if(test)qDebug()<<"Fail--"<<query.lastError();
            if(test)qDebug()<<"Fail--"<<query.lastQuery();
        }
    }
    else{
        if(test)qDebug()<<"Update balance limit cannot open db file !!";
    }
    Close_Whitelist_Database();
}

bool Database::kontrol_ServerId(QString server_id)
{   //server (usc ya da stawiz) id'sine göre kontrol.
    bool kontrol=false;
    if(whitelist.isOpen())
    {
        QSqlQuery query(whitelist);
        query.prepare("SELECT * FROM WHITELIST WHERE SERVER_ID=:server_id");
        query.bindValue(":server_id",server_id);
        query.exec();
        while(query.next())
        {
            kontrol=true;
        }
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
    return kontrol;
}

void Database::Delete_ServerId(QString server_id)
{  //server id ye göre silme.
    if(whitelist.isOpen())
    {
        QSqlQuery query(whitelist);
        query.prepare("DELETE FROM WHITELIST WHERE SERVER_ID=:server_id");
        query.bindValue(":server_id",server_id);
        query.exec();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}


void Database::InsertWhitelist(int id, QString server_id, QString white_rfid, QString white_plate, QString customer_name, QString white_products, QString card_type, QString list_type,QString offline_limit, QString central_price, QString discount_group, QString promotion_group, QString authorization_type, QString card_password, QString event, QString event_time, QString balance_limit, QString vehicle_km)//chn --vehicle km
{ //server id ye göre ekleme.
    if(whitelist.isOpen())
    {
        QSqlQuery query(whitelist);
        query.prepare("INSERT INTO WHITELIST VALUES(:id, :server_id, :white_rfid, :white_plate, :customer_name, :white_products, :card_type, :list_type, :offline_limit, :central_price, :discount_group, :promotion_group, :authorization_type, :card_password, :event, :event_time, :balance_limit, :vehicle_km)"); //30.05.2019
        query.bindValue(":id",id);
        query.bindValue(":server_id",server_id);
        query.bindValue(":white_rfid",white_rfid);
        query.bindValue(":white_plate",white_plate);
        query.bindValue(":customer_name",customer_name);
        query.bindValue(":white_products",white_products);
        query.bindValue(":card_type",card_type);
        query.bindValue(":list_type",list_type);
        query.bindValue(":offline_limit",offline_limit.toDouble());
        query.bindValue(":central_price",central_price);
        query.bindValue(":discount_group",discount_group);
        query.bindValue(":promotion_group",promotion_group);
        query.bindValue(":authorization_type",authorization_type);
        query.bindValue(":card_password",card_password);
        query.bindValue(":event",event);
        query.bindValue(":event_time",event_time);
        query.bindValue(":balance_limit",balance_limit);
        query.bindValue(":vehicle_km",vehicle_km);
        query.exec();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}

void Database::UpdateWhitelist(QString server_id, QString white_rfid, QString white_plate, QString customer_name, QString white_products, QString card_type, QString list_type, QString offline_limit, QString central_price, QString discount_group, QString promotion_group, QString authorization_type, QString card_password, QString event, QString event_time, QString vehicle_km)
{ //whitelist güncelleme.
    if(whitelist.isOpen())
    {
        QSqlQuery query(whitelist);
        query.prepare("UPDATE WHITELIST SET WHITE_RFID=:white_rfid, WHITE_PLATE=:white_plate, CUSTOMER_NAME=:customer_name, WHITE_PRODUCTS=:white_products, CARD_TYPE=:card_type, LIST_TYPE=:list_type, OFFLINE_LIMIT=:offline_limit, CENTRAL_PRICE=:central_price, DISCOUNT_GROUP=:discount_group, PROMOTION_GROUP=:promotion_group, AUTHORIZATION_TYPE=:authorization_type, CARD_PASSWORD=:card_password, EVENT=:event, EVENT_TIME=:event_time VEHICLE_KM=:vehicle_km WHERE SERVER_ID=:server_id ");
        query.bindValue(":server_id",server_id);
        query.bindValue(":white_rfid",white_rfid);
        query.bindValue(":white_plate",white_plate);
        query.bindValue(":customer_name",customer_name);
        query.bindValue(":white_products",white_products);
        query.bindValue(":card_type",card_type);
        query.bindValue(":list_type",list_type);
        query.bindValue(":offline_limit",offline_limit.toDouble());
        query.bindValue(":central_price",central_price);
        query.bindValue(":discount_group",discount_group);
        query.bindValue(":promotion_group",promotion_group);
        query.bindValue(":authorization_type",authorization_type);
        query.bindValue(":card_password",card_password);
        query.bindValue(":event",event);
        query.bindValue(":event_time",event_time);
        query.bindValue(":vehicle_km",vehicle_km);
        query.exec();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}


bool Database::kontrolSalesRfid(QString id)
{  //RFID nin whitelistten kontrol edildiği yer. usc_is_exist==false
    mts_flg=false;
    bool returnarray=false;
    QString deger="";
    QSqlDatabase sales_id = QSqlDatabase::database("sales_id");
    if (sales_id.open())
    {
        QSqlQuery query(sales_id);

        query.prepare("SELECT * FROM (SELECT * FROM WHITELIST ORDER BY OFFLINE_LIMIT DESC) WHERE WHITE_RFID = :id");
        query.bindValue(":id",id);
        query.exec();

        while(query.next())
        {
            if(query.value(16).toString()=="1" && query.value(5).toInt()==pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PrdctNo)
            {
                int ct=query.value(6).toInt();
                if(ct>=1 && ct<=4)
                {
                    deger="tts";
                    qDebug()<<deger<<"yuyu";
                    if(Driver_wait_flg!=true)
                    {
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                        for(int pl=0; pl<12;pl++)
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                        }
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                        TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                        }
                        if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                        settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                        daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));


                        if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                        settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                        {
                            // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                            // {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                            long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                            dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                            if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                            settings.setlog("Normal Preset"+QString::number(temp_PVAL));
                            QString tempString=QString::number(temp_PVAL);
                            tempString.insert(tempString.length()-2,",");
                            balanceLimit=tempString;
                            settings.setlog("DATABASE_DEBUG Database preset value "+tempString);

                            //}
                            returnarray=true;
                        }
                        else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                    }
                    else
                    {
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                        returnarray=true;
                    }
                    break;
                }
                else if(ct>=7 && ct<=10)
                {qDebug()<<deger<<"yuyu5"<<Driver_wait_flg;
                    deger="mts";
                    if(Driver_wait_flg!=true){
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                        for(int pl=0; pl<12;pl++)
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                        }
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                        TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                        }
                        if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                        settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                        long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                        if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                        settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                        {
                            // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                            // {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                            long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                            dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                            if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                            QString tempString=QString::number(temp_PVAL);
                            tempString.insert(tempString.length()-2,",");
                            balanceLimit=tempString;
                            qDebug()<<"TempsStringgg"<<tempString;
                            settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                            //}
                            returnarray=true;
                        }
                        else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                    }
                    else
                    {
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                        returnarray=true;
                    }
                    mts_flg=true;
                    break;
                }
                else if(ct>=14 && ct<=15)
                {
                    if(mts_flg==false)
                    {
                        deger="loyalty";
                        if(Driver_wait_flg!=true){
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                            for(int pl=0; pl<12;pl++)
                            {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                            }
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                            TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                            {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                            }
                            if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                            settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                            long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                            if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                            settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                            {
                                // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                // {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                                dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                //}
                                returnarray=true;
                            }
                            else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                        }
                        else
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                            returnarray=true;
                        }
                    }
                }
                else
                {
                    if(mts_flg==false)
                    {
                        deger="others";
                        if(Driver_wait_flg!=true){
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                            for(int pl=0; pl<12;pl++)
                            {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                            }
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                            TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                            {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                            }
                            if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                            settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                            long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                            if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                            settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                            {
                                // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                // {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                                dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                //}
                                returnarray=true;
                            }
                            else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                        }
                        else
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                            returnarray=true;
                        }
                    }

                }
            }
            else
            {
                if(query.value(16).toString()=="1")
                {
                    if(query.value(5).toInt()!=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PrdctNo)
                    {
                        wrong_product=true;
                    }

                }
                if(query.value(16).toString()=="0")
                {
                    passive_id=true;
                }
                else if(query.value(16).toString()=="2")
                {
                    deleted_id=true;
                }
                returnarray=false;
            }
        }
        sales_id.commit();
        sales_id.close();
    }
    if(test)qDebug()<<"CARD TYPE ="<<deger;
    if(test)qDebug()<<"RETURN ="<<returnarray;
    settings.setlog("DATABASE_DEBUG Database CARD TYPE "+deger);
    settings.setlog("DATABASE_DEBUG Database RETURN "+QString::number(returnarray));
    return returnarray;
}
bool Database::kontrolSales(QString id)
{       //satış verilecek id yi kontrol eder. Kullanılmıyor.
    bool returnarray=false;

    QSqlDatabase controlsales_id = QSqlDatabase::database("controlsales_id");
    if (controlsales_id.open())
    {
        QSqlQuery query(controlsales_id);
        query.prepare("SELECT * FROM WHITELIST WHERE WHITE_RFID = :id");
        query.bindValue(":id",id);
        query.exec();
        if(query.next())
        {
            if(query.value(7).toString()=="0")
            {
                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                if(test)qDebug()<<"kontrolSales"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP;
                settings.setlog("DATABASE_DEBUG kontrolSales "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP));
                returnarray=true;
            }
            else returnarray=false;
        }
        else returnarray=false;
        controlsales_id.commit();
        controlsales_id.close();
    }
    return returnarray;
}


QString Database::get_Unit_Price(int id)
{
    QSqlDatabase get_price = QSqlDatabase::database("get_price");
    QString price="";
    if(get_price.isOpen())
    {
        QSqlQuery query(get_price);
        query.prepare("SELECT PRODUCT_PRICE FROM NOZZLE WHERE NOZZLE_ID=:nozzle_id");
        query.bindValue(":nozzle_id",id);
        query.exec();
        while(query.next())
        {
            price=query.value(0).toString();
        }
        get_price.commit();
        get_price.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
    return price;
}
//-----databaseden km bilgisi al-----//
int Database::getKm(QString plate)//chn
{
    int plateKm;
    Open_Whitelist_Database();
//    if(askPlateSale==true){
//        if(plateDB==true){
//            plates = QSqlDatabase::database("plates");
//            if(!plates.isOpen())
//                plates.open();
//            if(plates.isOpen()){
//                QSqlQuery query(plates);
//                query.prepare("Select VehicleKM From PLATES Where Plate=:PLAKA");
//                query.bindValue(":PLAKA",plate);
//                query.exec();
//                qDebug()<<query.next()<<"have a nice day"<<query.value(0).toInt();
//                plateKm =query.value(0).toInt();
//            }
//        }
//    }
    if(customerCard==true){
        if(whitelist.isOpen()){
            QSqlQuery query(whitelist);
            if(plateDB==false){
                query.prepare("Select VEHICLE_KM From WHITELIST Where WHITE_RFID=:ID");
                query.bindValue(":ID",plate);
            } else {
                query.prepare("Select VehicleKM From PLATES Where Plate=:PLAKA");
                query.bindValue(":PLAKA",plate);
            }

            if(query.exec()){
                while (query.next()) {
                    plateKm=query.value(0).toInt();
                    //qDebug()<<plateKm<<"plaka.."<<plate;
                }
                
            }

        }
        
    }
    else{
        if(whitelist.isOpen()){
            QSqlQuery query(whitelist);
            if(plateDB==false){
                query.prepare("Select VEHICLE_KM From WHITELIST Where WHITE_PLATE=:ID");
                query.bindValue(":ID",plate);
            } else {
                query.prepare("Select VehicleKM From PLATES Where Plate=:PLAKA");
                query.bindValue(":PLAKA",plate);
            }

            if(query.exec()){
                while (query.next()) {
                    plateKm=query.value(0).toInt();
                    qDebug()<<plateKm<<"plaka.."<<plate;
                }
            }

        }

    }
//    plates = QSqlDatabase::database("plates");
//    if(plates.isOpen()){
//        plates.commit();
//        plates.close();
//    }

    Close_Whitelist_Database();
    return plateKm;
}//chn
//-----databaseden km bilgisi al-----//

QString Database::getBalanceLimit(QString rfid)
{
    QString balance;
    Open_Whitelist_Database();
    if(whitelist.isOpen()){
        QSqlQuery query(whitelist);
        if(plateDB==false){
        query.prepare("Select BALANCE_LIMIT From WHITELIST Where WHITE_RFID=:rfid");
        query.bindValue(":rfid",rfid);
        }
        else {
            query.prepare("Select BalanceLimit From PLATES Where Plate=:PLAKA");
            query.bindValue(":PLAKA",rfid);
        }
        if(query.exec()){
            while (query.next()) {
                balance=query.value(0).toString();
            }
        }
        else{
            if(test)qDebug()<<"GET BALANCE LIMIT FAIL--"<<query.lastError();
            if(test)qDebug()<<"GET BALANCE LIMIT FAIL--"<<query.lastQuery();
        }
    }


    Close_Whitelist_Database();
    return balance;
}
//------database'e yeni km bilgisi aktarılır------//
bool Database::updateKm(QString kilometer)//chn
{
    bool queryStatus =false;
    Open_Whitelist_Database();
    if(whitelist.isOpen()){

        qDebug()<<"custplate:"<<Customer_plate<<CustomerID<<kilometer;
        if(customerCard==true){
            QSqlQuery query(whitelist);
            if(plateDB==false){
            query.prepare("UPDATE WHITELIST SET VEHICLE_KM=:km WHERE WHITE_RFID=:ID");
            query.bindValue(":km",kilometer);
            query.bindValue(":ID",CustomerID);
            } else {
                QString cust_plate(QString::fromUtf8(Customer_plate));
                query.prepare("UPDATE PLATES SET VehicleKM=:km WHERE Plate=:ID");
                query.bindValue(":km",kilometer);
                query.bindValue(":ID",cust_plate);
            }
            queryStatus= query.exec();
            if(queryStatus>0){
                queryStatus=true;
            } else {
                queryStatus=false;
            }

        }
        else{
            QSqlQuery query(whitelist);
            QString cust_plate(QString::fromUtf8(Customer_plate));
            qDebug()<<"custplate:"<<cust_plate<<kilometer;

            if(plateDB==false){
                query.prepare("UPDATE WHITELIST SET VEHICLE_KM=:km WHERE WHITE_PLATE=:PLATE");
                query.bindValue(":km",kilometer);
                query.bindValue(":PLATE",cust_plate);
            } else {
                query.prepare("UPDATE PLATES SET VehicleKM=:km WHERE Plate=:ID");
                query.bindValue(":km",kilometer);
                query.bindValue(":ID",cust_plate);
            }

            queryStatus= query.exec();
            if(queryStatus>0){
                queryStatus=true;
            } else {
                queryStatus=false;
            }


        }

    }
    return queryStatus;
    Close_Whitelist_Database();
}
//------database'e yeni km bilgisi aktarılır------//

void Database::updateAllBalanceLimit()
{
    Open_Whitelist_Database();
    if(whitelist.isOpen()){
        QSqlQuery query(whitelist);
        query.prepare("UPDATE WHITELIST SET BALANCE_LIMIT=OFFLINE_LIMIT");
        if(query.exec()){
            query.prepare("UPDATE PLATES SET BalanceLimit=OfflineLimit");
            if(query.exec()){
                if(test) qDebug()<<"All balance limites updated!";
            }
        }

        else{
            if(test)qDebug()<<"Update All Balance Limit fail -- "<<query.lastError();
            if(test)qDebug()<<"Update All Balance Limit fail -- "<<query.lastQuery();
        }

    }

    Close_Whitelist_Database();
}


bool Database::kontrolSalesPlate(QString plate)
{       //Plaka kontrol.
    mts_flg=false;
    bool returnarray=false;
    QString deger="";
    QSqlDatabase sales_plate = QSqlDatabase::database("sales_plate");
    if (sales_plate.open())
    {
        QSqlQuery query(sales_plate);
        query.prepare("SELECT * FROM (SELECT * FROM WHITELIST ORDER BY OFFLINE_LIMIT DESC) WHERE WHITE_PLATE = :plate");
        query.bindValue(":plate",plate);
        query.exec();
        next=query.next();
        qDebug()<<next<<"yebadeba";
        if(test)qDebug()<<"Database Plaka:"<<query.lastError()<<plate<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP;
//******************plate sale begin***************
        if(next==false){
            if(askPlateSale==true){

            query.prepare("SELECT * FROM (SELECT * FROM PLATES ORDER BY OfflineLimit DESC) WHERE Plate = :plate");
            query.bindValue(":plate",plate);
            query.exec();

            while(query.next())
            {
                plateDB=true;
                settings.setlog("DATABASE_DEBUG Database Plaka: "+plate);
                if(test)qDebug()<<"Database Plaka:"<<query.value(3).toString()<<plate;
                qDebug()<<query.value(5).toString()<<"yeey";
                qDebug()<<query.value(1).toString()<<"yooy";
                customerPlate=QString::fromUtf8(Customer_plate);
                if((query.value(5).toString()=="1") && (query.value(1).toString()!="") && query.value(3).toInt()==pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PrdctNo)
                {
                        if(mts_flg==false)
                        {
                            deger="others";
                            qDebug()<<deger;
                            if(Driver_wait_flg!=true){
                                if(USC_is_exist==true)
                                {   qDebug()<<"deger";
                                    if(Person_is_Ok==true)
                                    {
                                        qDebug()<<"deger1";
                                        Temp_ID[1]=query.value(2).toString();
                                    }
                                    returnarray=true;
                                }
                                else
                                {qDebug()<<"deger2";
                                    if(Person_is_Ok==true)
                                    {qDebug()<<"deger3";
                                        Temp_ID[1]=query.value(2).toString();
                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                                    for(int pl=0; pl<12;pl++)
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(1).toByteArray();
                                    TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(4).toByteArray();
                                    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                                    }
                                    if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                                    settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                                    long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                                    if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                                    settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                                    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                                    {
                                        // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                        // {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                        long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                                        dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                        if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                        settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                        //}
                                        returnarray=true;
                                    }
                                    else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                                }
                            }
                            else
                            {   qDebug()<<"deger4";
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                                returnarray=true;
                            }
                        }



                }
                else
                {
                    if(query.value(3).toInt()!=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PrdctNo)
                    {qDebug()<<"deger5";
                        wrong_product=true;
                    }
                    returnarray=false;
                }
            }
        }
    }
//****************plate sale end ***********
        else {
            while(next)
            {
                settings.setlog("DATABASE_DEBUG Database Plaka: "+plate);
                if(test)qDebug()<<"Database Plaka:"<<query.value(3).toString()<<plate;
                qDebug()<<query.value(16).toString();
                qDebug()<<query.value(3).toString();
                if((query.value(16).toString()=="1") && (query.value(3).toString()!="") && query.value(5).toInt()==pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PrdctNo)
                {
                    int ct=query.value(6).toInt();
                    qDebug()<<ct<<"yes";
                    if(ct>=1 && ct<=4)
                    {
                        deger="tts";
                        qDebug()<<deger<<"yuyu"<<Driver_wait_flg<<USC_is_exist;
                        if(Driver_wait_flg!=true){
                            if(USC_is_exist==true)
                            {
                                settings.setlog("DATABASE_DEBUG USC-FALSE-tts-Person_is_Ok: "+QString::number(Person_is_Ok));
                                if(test)qDebug()<<"USC-FALSE-tts-Person_is_Ok"<<Person_is_Ok;
                                if(Person_is_Ok==true)
                                {
                                    Temp_ID[1]=query.value(2).toString();

                                }
                                returnarray=true;
                            }
                            else
                            {
                                settings.setlog("DATABASE_DEBUG USC-FALSE-tts-Person_is_Ok: "+QString::number(Person_is_Ok));
                                if(test)qDebug()<<"USC-OK-Person_is_Ok"<<Person_is_Ok;
                                if(Person_is_Ok==true)
                                {
                                    qDebug()<<"yestts";
                                    Temp_ID[1]=query.value(2).toString();
                                }
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                                for(int pl=0; pl<12;pl++)
                                {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                                }
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                                TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                                {qDebug()<<"yomgtts";
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                                }
                                if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                                settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                                long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                                if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                                settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                                {qDebug()<<"yusriyur";
                                    // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                    // {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                    long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                                    dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                    if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                    settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                    //}
                                    returnarray=true;
                                }
                                else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                            }
                        }

                        else
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                            returnarray=true;
                        }
                        break;

                    }
                    else if(ct>=7 && ct<=10)
                    {
                        deger="mts";
                        qDebug()<<deger<<"yuyu"<<Driver_wait_flg<<USC_is_exist;
                        if(Driver_wait_flg!=true)
                        {
                            if(USC_is_exist==true)
                            {
                                settings.setlog("DATABASE_DEBUG USC-ok-mts-Person_is_Ok: "+QString::number(Person_is_Ok));
                                if(test)qDebug()<<"USC-ok-Person_is_Ok-mts"<<Person_is_Ok;
                                if(Person_is_Ok==true)
                                {

                                    Temp_ID[1]=query.value(2).toString();

                                }
                                returnarray=true;
                            }
                            else
                            {
                                settings.setlog("DATABASE_DEBUG USC-ok-mts-Person_is_Ok: "+QString::number(Person_is_Ok));
                                if(test)qDebug()<<"USC-false-Person_is_Ok-mts"<<Person_is_Ok;
                                if(Person_is_Ok==true)
                                {
                                    qDebug()<<"yes";
                                    Temp_ID[1]=query.value(2).toString();
                                }
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();

                                for(int pl=0; pl<12;pl++)
                                {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                                }
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                                TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                                {qDebug()<<"yomg";
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                                }
                                if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                                settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                                long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                                if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                                settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                                {qDebug()<<"yusriyur";
                                    // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                    // {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                    long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                                    dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                    if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                    settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                    //}
                                    returnarray=true;
                                }
                                else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                            }
                        }

                        else
                        {
                            qDebug()<<"flase";
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                            returnarray=true;
                        }
                        mts_flg=true;

                    }
                    else if(ct>=14 && ct<=15)
                    {
                        if(mts_flg==false)
                        {
                            deger="loyalty";
                            if(Driver_wait_flg!=true){
                                if(USC_is_exist==true)
                                {
                                    settings.setlog("DATABASE_DEBUG USC-ok-loyalty-Person_is_Ok: "+QString::number(Person_is_Ok));
                                    if(test)qDebug()<<"USC-ok-Person_is_Ok-loyalty"<<Person_is_Ok;
                                    if(Person_is_Ok==true)
                                    {

                                        Temp_ID[1]=query.value(2).toString();
                                        if(test)qDebug()<<"USC-ok-Person_is_Ok-lTemp_ID[1]"<<Temp_ID[1];
                                    }
                                    returnarray=true;
                                }
                                else
                                {
                                    settings.setlog("DATABASE_DEBUG USC-ok-loyalty-Person_is_Ok: "+QString::number(Person_is_Ok));
                                    if(test)qDebug()<<"USC-false-Person_is_Ok-loyalty"<<Person_is_Ok;
                                    if(Person_is_Ok==true)
                                    {
                                        Temp_ID[1]=query.value(2).toString();
                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                                    for(int pl=0; pl<12;pl++)
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                                    TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                                    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                                    }
                                    if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt();
                                    settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                                    long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                                    if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                                    settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                                    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                                    {
                                        // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                        // {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                        long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);
                                        if(test)qDebug()<<"temp_PVAL"<<temp_PVAL;
                                        dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                        if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                        settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                        //}
                                        returnarray=true;
                                    }
                                    else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                                }
                            }
                        }
                        else
                        {
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                            returnarray=true;
                        }
                    }
                    else
                    {
                        if(mts_flg==false)
                        {
                            deger="others";
                            if(Driver_wait_flg!=true){
                                if(USC_is_exist==true)
                                {
                                    if(Person_is_Ok==true)
                                    {
                                        Temp_ID[1]=query.value(2).toString();
                                    }
                                    returnarray=true;
                                }
                                else
                                {
                                    if(Person_is_Ok==true)
                                    {
                                        Temp_ID[1]=query.value(2).toString();
                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=query.value(6).toInt();
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=query.value(6).toInt();
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=query.value(12).toInt();
                                    for(int pl=0; pl<12;pl++)
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=query.value(3).toByteArray();
                                    TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL=query.value(8).toByteArray();
                                    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf(".")>0)
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.remove(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."),(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.length()-(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.lastIndexOf("."))));
                                    }
                                    if(test)qDebug()<<"pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL;
                                    settings.setlog("DATABASE_DEBUG DATABASE-PRESET "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex()));
                                    long int daily_sold_volume=totalsales(QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE));
                                    if(test)qDebug()<<"daily_sold_volume"<<daily_sold_volume;
                                    settings.setlog("DATABASE_DEBUG DATABASE-daily_sold_volume "+QString::number(daily_sold_volume));
                                    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)>daily_sold_volume)
                                    {
                                        // if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()-daily_sold_volume<300)
                                        // {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=2; online_flg=true;

                                        long int temp_PVAL=(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toInt()*qPow(10,volume_dot_index)-daily_sold_volume);

                                        dec_to_bcd(temp_PVAL); // 4 byte 00 00 12 34
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[0]=bcd[3];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[1]=bcd[2];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[2]=bcd[1];
                                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL[3]=bcd[0];
                                        if(test)qDebug()<<"Database preset value"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PVAL.toHex();
                                        settings.setlog("DATABASE_DEBUG Database preset value "+QString::number(temp_PVAL));
                                        //}
                                        returnarray=true;
                                    }
                                    else{ limit_error=true; StoredScreenRFID_fromMaster=8; returnarray=false;}
                                }
                            }
                            else
                            {
                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP=query.value(6).toInt();
                                returnarray=true;
                            }
                        }


                    }
                }
                else
                {
                    if(query.value(5).toInt()!=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PrdctNo)
                    {
                        qDebug()<<"hatalı ürün true";
                        wrong_product=true;
                    }
                    returnarray=false;
                }
                next=false;
            }
        }
        sales_plate.commit();
        sales_plate.close();
    }
    if(test)qDebug()<<"CARD TYPE ="<<deger;
    if(test)qDebug()<<"RETURN ="<<returnarray;
    settings.setlog("DATABASE_DEBUG Database CARD TYPE "+deger);
    settings.setlog("DATABASE_DEBUG Database RETURN "+QString::number(returnarray));
    qDebug()<<"nope"<<returnarray;
    return returnarray;
}


void Database::Open_Person_Database()
{
    person = QSqlDatabase::database("person");
    if(!person.isOpen())
        person.open();
}

void Database::Close_Person_Database()
{
    person = QSqlDatabase::database("person");
    if(person.isOpen())
    {
        person.commit();
        person.close();
    }
}

void Database::InsertPersonRfid(int id, QString rfid, QString name, int dgroupname)
{
    if (person.isOpen())
    {
        QSqlQuery query(person);
        query.prepare("INSERT INTO PERSON VALUES(:id, :rfid, :name, :dgroupname)");
        query.bindValue(":id",id);
        query.bindValue(":rfid",rfid);
        query.bindValue(":name",name);
        query.bindValue(":dgroupname",dgroupname);
        query.exec();
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}


bool Database::kontrolPersonRfid(QString id)
{   //Pompacı kartı kontrol.
    bool return_exec=false;
    bool return_database=false;
    QSqlDatabase control_person = QSqlDatabase::database("control_person");
    if(test)qDebug()<<"DATABASE_DEBUG control_person "<<id;
    settings.setlog("DATABASE_DEBUG control_person "+id);
    qDebug()<<"ID"<<id;
    if (control_person.open())
    {
        QSqlQuery query(control_person);
        query.prepare("SELECT * FROM PERSON WHERE PERSON_RFID = :id");
        query.bindValue(":id",id);
        query.exec();
        while(query.next())
        {
            if((query.value(4).toString()=="1"))
            {
                return_exec=true;
            }
            else return_exec=false;
        }
        control_person.commit();
        control_person.close();
        return return_exec;
    }
    else
    {
        return_database=false;
    }
    return return_database;
}

QSqlQuery Database::Show_Person()
{
    QSqlDatabase show_person = QSqlDatabase::database("show_person");
    QSqlQuery tempquery(show_person);
    if (show_person.open())
    {
        QSqlQuery persons(show_person);
        persons.prepare("select PERSON_RFID as ID,"\
                        "PERSON_NAME as ISIM from PERSON");
        persons.exec();
        while(persons.next())
        {
            tempquery=persons;
        }
        if(test)qDebug()<<"Show_Person"<<persons.lastError();
        show_person.close();

    }
    else
    {
        if(test) qDebug()<<"show_person_error";
    }
    return tempquery;
}

QSqlQuery Database::Show_Whitelist()
{
    QSqlDatabase show_whitelist = QSqlDatabase::database("show_whitelist");
    QSqlQuery tempquery(show_whitelist);
    if (show_whitelist.open())
    {
        QSqlQuery white(show_whitelist);
        white.prepare("select WHITE_RFID as ID,"\
                      "WHITE_PLATE as PLAKA,"  \
                      "CUSTOMER_NAME as MÜŞTERİ,"\
                      "CARD_TYPE as KART_TİPİ," \
                      "OFFLINE_LIMIT as  LİTRE_LİMİT,"\
                      "AUTHORIZATION_TYPE as AUTHORIZE_TİPİ,"\
                      "EVENT as EVENT from WHITELIST");
        white.exec();
        if(test)qDebug()<<"show_whitelist"<<white.lastError();
        while(white.next())
        {tempquery=white;}
        show_whitelist.close();
    }
    else
    {
        if(test) qDebug()<<"show_whitelist_error";
    }
    return tempquery;
}


QSqlQuery Database::Show_Nozzle()
{
    QSqlDatabase show_nozzle = QSqlDatabase::database("show_nozzle");
    QSqlQuery tempquery(show_nozzle);
    if (show_nozzle.open())
    {
        QSqlQuery nzzl(show_nozzle);
        nzzl.prepare("select * from NOZZLE");
        nzzl.exec();
        if(test)qDebug()<<"show_nozzle"<<nzzl.lastError();
        while(nzzl.next())
        {tempquery=nzzl;}
        show_nozzle.close();
    }
    else
    {
        if(test) qDebug()<<"show_nozzle_error";

    }

    return tempquery;
}


QSqlQuery Database::Show_Sales()
{
    QSqlDatabase show_sales = QSqlDatabase::database("show_sales");
    QSqlQuery tempquery(show_sales);
    if (show_sales.open())
    {
        QSqlQuery sls(show_sales);
        sls.prepare("select SALES_DATE, SALES_CLOCK, PUMP, NOZZLE, PRODUCT_N0, UNIT_PRC, SALES_LITER, SALES_AMOUNT, BEGIN_TOTAL_AMOUNT, END_TOTAL_AMOUNT, PLATE from SALES '"+table_name+"'");
        sls.exec();
        while(sls.next())
        {
            tempquery=sls;
        }
        qDebug()<<"show_sales"<<sls.lastError();
        show_sales.close();
    }
    else
    {
        if(test) qDebug()<<"show_sales_error";

    }
    return tempquery;
}


void Database::Open_Nozzles_Database()
{
    nozzle = QSqlDatabase::database("nozzle");
    if(!nozzle.isOpen())
        nozzle.open();
}

void Database::Close_Nozzle_Database()
{
    nozzle = QSqlDatabase::database("nozzle");
    if(nozzle.isOpen())
    {
        nozzle.commit();
        nozzle.close();
    }
}


void Database::InsertNozzle(int nozzle_id, int product_no, int tank_no, QString unit_price, QString max_unit_price, QString min_unit_price)
{   //nozzle ekleme
    if(test)qDebug()<<"DATABASE_DEBUG InsertNozzle "<<nozzle_id;
    settings.setlog("DATABASE_DEBUG InsertNozzle "+QString::number(nozzle_id));
    if(nozzle.isOpen())
    {
        QSqlQuery query(nozzle);
        query.prepare("INSERT INTO NOZZLE VALUES(:nozzle_id, :product_no, :tank_no, :unit_price, :max_unit_price, :min_unit_price)");
        query.bindValue(":nozzle_id",nozzle_id);
        query.bindValue(":product_no",product_no);
        query.bindValue(":tank_no",tank_no);
        query.bindValue(":unit_price",unit_price);
        query.bindValue(":max_unit_price",max_unit_price);
        query.bindValue(":min_unit_price",min_unit_price);
        query.exec();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}

void Database::UpdateNozzle(int id, QString product_id, QString product_name, QString product_price, QString total_amount, QString total_volume, QString total_sales)
{  //nozzle update
    if(test)qDebug()<<"DATABASE_DEBUG UpdateNozzle"<<id;
    settings.setlog("DATABASE_DEBUG UpdateNozzle "+QString::number(id));
    if(nozzle.isOpen())
    {
        QSqlQuery query(nozzle);
        query.prepare("UPDATE NOZZLE SET PRODUCT_ID=:product_id, PRODUCT_NAME=:product_name, PRODUCT_PRICE=:product_price, TOTAL_AMOUNT=:total_amount, TOTAL_VOLUME=:total_volume, TOTAL_SALES=:total_sales WHERE NOZZLE_ID=:id ");
        query.bindValue(":id",id);
        query.bindValue(":product_id",product_id);
        query.bindValue(":product_name",product_name);
        query.bindValue(":product_price",product_price);
        query.bindValue(":total_amount",total_amount);
        query.bindValue(":total_volume",total_volume);
        query.bindValue(":total_sales",total_sales);
        query.exec();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzle-Error Database";
    }
}

void Database::UpdateNozzleProduct(int id, QString product_id, QString product_name)
{
    QSqlDatabase update_nozzle_product = QSqlDatabase::database("update_nozzle_product");
    if(update_nozzle_product.open())
    {
        QSqlQuery query(update_nozzle_product);
        query.prepare("UPDATE NOZZLE SET PRODUCT_ID=:product_id, PRODUCT_NAME=:product_name WHERE NOZZLE_ID=:id ");
        query.bindValue(":id",id);
        query.bindValue(":product_id",product_id);
        query.bindValue(":product_name",product_name);
        query.exec();
        update_nozzle_product.commit();
        update_nozzle_product.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzleProduct-Error Database";
    }
}


void Database::UpdateNozzlePrice(int id, QString product_price)
{
    QSqlDatabase update_nozzle_price = QSqlDatabase::database("update_nozzle_price");
    if(update_nozzle_price.open())
    {
        QSqlQuery query(update_nozzle_price);
        query.prepare("UPDATE NOZZLE SET PRODUCT_PRICE=:product_price WHERE NOZZLE_ID=:id");
        query.bindValue(":id",id);
        query.bindValue(":product_price",product_price);
        query.exec();
        update_nozzle_price.commit();
        update_nozzle_price.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzlePrice-Error Database";
    }
}

void Database::UpdateNozzleTotal(int id, QString total_amount, QString total_volume, QString total_sales)
{
    QSqlDatabase update_nozzle_total = QSqlDatabase::database("update_nozzle_total");
    if(update_nozzle_total.open())
    {
        QSqlQuery query(update_nozzle_total);
        query.prepare("UPDATE NOZZLE SET TOTAL_AMOUNT=:total_amount, TOTAL_VOLUME=:total_volume, TOTAL_SALES=:total_sales WHERE NOZZLE_ID=:id ");
        query.bindValue(":id",id);
        query.bindValue(":total_amount",total_amount);
        query.bindValue(":total_volume",total_volume);
        query.bindValue(":total_sales",total_sales);
        query.exec();
        update_nozzle_total.commit();
        update_nozzle_total.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzleTotal-Error Database";
    }
}

QString Database::NozzleProductId(int id)
{
    QSqlDatabase nozzle_product_id = QSqlDatabase::database("nozzle_product_id");
    QString returnarray="false";
    if(nozzle_product_id.open())
    {
        QSqlQuery query(nozzle_product_id);
        query.prepare("SELECT PRODUCT_ID FROM NOZZLE WHERE NOZZLE_ID = :id");
        query.bindValue(":id",id);
        query.exec();
        if(query.next())
        {
            returnarray=query.value(0).toString();
        }
        else returnarray="false";
        nozzle_product_id.commit();
        nozzle_product_id.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleProductId-Error Database";
    }
    return returnarray;
}

QString Database::getProductname()
{   //görsel tarafında product isimleri için.
    QSqlDatabase get_product_name = QSqlDatabase::database("update_nozzle_price");
    QString return_data="";
    if(get_product_name.open())
    {
        QSqlQuery query(get_product_name);
        query.prepare("SELECT PRODUCT_NAME,PRODUCT_ID,NEW_UNIT_PRICE,TANK_NO FROM PRODUCT");
        query.exec();
        int count=1;
        while(query.next())
        {
            return_data+=query.value(0).toString()+"#";
            product_id[count]=query.value(1).toInt();
            new_unit_price[count]=query.value(2).toString();
            tank_no[count]=query.value(3).toInt();
            count++;
        }

        get_product_name.commit();
        get_product_name.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzlePrice-Error Database";
    }
    return return_data;
}

void Database::updateSettings(int card_adr,int unit_price_index,int amount_index,int volume_index
                              ,QString receipt_1,QString receipt_2,QString receipt_3,QString receipt_4
                              ,bool km,bool sell,bool plates)
{   //setting tablosunu güncelliyor.
    QSqlDatabase update_setting = QSqlDatabase::database("update_nozzle_price");

    if(update_setting.open())
    {
        QSqlQuery query(update_setting);
        query.prepare("UPDATE SETTING SET CARD_ADDRESS=:card_adr, AMOUNT=:amount_index, VOLUME=:volume_index, UNIT_PRICE=:unit_price_index, LINE_BREAK=:receipt_3, LINE_BREAK2=:receipt_4, TITLE=:receipt_1, TITLE2=:receipt_2, KM_REPORT=:km, SELLING_STYLE=:sell, PLATE_CONTROL=:plates WHERE ID=:id");
        query.bindValue(":card_adr",card_adr);
        query.bindValue(":amount_index",amount_index);
        query.bindValue(":volume_index",volume_index);
        query.bindValue(":unit_price_index",unit_price_index);
        query.bindValue(":receipt_3",receipt_3);
        query.bindValue(":receipt_4",receipt_4);
        query.bindValue(":receipt_2",receipt_2);
        query.bindValue(":receipt_1",receipt_1);
        if(km)  query.bindValue(":km",1);
        else    query.bindValue(":km",0);
        if (sell) query.bindValue(":sell",1);//CHN
        else    query.bindValue(":sell",0);
        if (plates) query.bindValue(":plates",1);//CHN
        else    query.bindValue(":plates",0);
        query.bindValue(":id",1);
        query.exec();
        update_setting.commit();
        update_setting.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzlePrice-Error Database";
    }
}


void Database::updateProductname(int nozzle_id,int product_id,QString price,int tank)
{   //menüden nozzle ürünleri güncelleme
    QSqlDatabase update_product_name = QSqlDatabase::database("update_nozzle_price");

    if(update_product_name.open())
    {
        QSqlQuery query(update_product_name);
        query.prepare("UPDATE NOZZLE SET PRODUCT_ID=:product_id,PRODUCT_PRICE=:price,TANK_NO=:tank  WHERE NOZZLE_ID=:nozzle_id");
        query.bindValue(":nozzle_id",nozzle_id);
        query.bindValue(":product_id",product_id);
        query.bindValue(":price",price);
        query.bindValue(":tank",tank);
        query.exec();
        update_product_name.commit();
        update_product_name.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateNozzlePrice-Error Database";
    }
}

bool Database::NozzleValues()
{   //nozzle ayarlarını çektiğimiz yer.
    QSqlDatabase nozzle_values = QSqlDatabase::database("nozzle_values");
    bool returnarray=false;
    pumpNo=1;
    if(nozzle_values.open())
    {
        QSqlQuery query(nozzle_values);
        query.prepare("SELECT * FROM NOZZLE ");
        query.exec();
        int nozzle_count=1;
        while(query.next())
        {

            pump[pumpNo].nozzle[nozzle_count].nozzle_id=query.value(0).toInt();

            pump[pumpNo].nozzle[nozzle_count].PrdctNo=query.value(1).toInt();

            pump[pumpNo].nozzle[nozzle_count].TankNo=query.value(2).toInt();


            QString string_uprice;
            string_uprice=query.value(3).toString();
            Unit_price_for_save[nozzle_count]=string_uprice.toLatin1();
            Lastprice[nozzle_count]=Unit_price_for_save[nozzle_count];
            Unit_price=convert_format(Unit_price_for_save[nozzle_count],unit_price_dot_index);
            qDebug()<<Unit_price;
            dec_to_bcd(Unit_price.toInt()); // 4 byte 00 00 12 34
            pump[pumpNo].nozzle[nozzle_count].unitprice[0]=bcd[2];
            pump[pumpNo].nozzle[nozzle_count].unitprice[1]=bcd[1];
            pump[pumpNo].nozzle[nozzle_count].unitprice[2]=bcd[0];
            QSqlQuery query2(nozzle_values);
            query2.prepare("SELECT PRODUCT_NAME FROM PRODUCT WHERE PRODUCT_ID = :id ");
            query2.bindValue(":id",pump[pumpNo].nozzle[nozzle_count].PrdctNo);
            query2.exec();

            if(query2.next())
            {
                pump[pumpNo].nozzle[nozzle_count].Product_name=query2.value(0).toString();
            }


            returnarray=true;

            nozzle_count++;
        }
        Number_of_Nozzle=nozzle_count-1;
        nozzle_values.commit();
        nozzle_values.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleProductName-Error Database";
    }
    return returnarray;
}


bool Database::ProbeValues()
{   //probe değerlerini çektiğimiz yer.
    QSqlDatabase probe_values = QSqlDatabase::database("nozzle_values");
    bool returnarray=false;
    bool ok;
    if(probe_values.open())
    {
        QSqlQuery query(probe_values);
        query.prepare("SELECT * FROM PROBE");
        bool queryResult=query.exec();
        if(queryResult>0){
            int count=0;

            while(query.next())
            {
                Probe[count].adres[0]=query.value(2).toString().toInt(&ok,16);
                Probe[count].status="1";
                Probe[count].fuel_level=query.value(4).toString();
                Probe[count].water_level=query.value(5).toString();
                Probe[count].temperature=query.value(6).toString();
                Probe[count].tankSize=query.value(7).toString();
                Probe[count].probeOffset=query.value(8).toString();
                Probe[count].minLevel=query.value(9).toString();
                Probe[count].maxLevel=query.value(10).toString();
                Probe[count].minTemperature=query.value(11).toString();
                Probe[count].maxTemperature=query.value(12).toString();
                Probe[count].probe_maxWaterLevel=query.value(13).toString();
                Probe[count].probe_minWaterLevel=query.value(14).toString();
                Probe[count].probe_waterOffset=query.value(15).toString();
                Probe[count].unsuccess_answer=0;
                returnarray=true;
                count++;
            }
            probe_count=count;
            probe_values.commit();
            probe_values.close();

            //        sync();
            //        system("sync");
        }
    }
    else
    {
        if(test) qDebug()<<"NozzleProductName-Error Database";
    }
    return returnarray;
}

bool Database::SettingsValues()
{   //settings i çektiğimiz yer
    QSqlDatabase setting_values = QSqlDatabase::database("nozzle_values");
    bool returnarray=false;
    if(setting_values.open())
    {
        QSqlQuery query(setting_values);
        query.prepare("SELECT * FROM SETTING ");
        query.exec();
        while(query.next())
        {
            tempcard_reader_address=query.value(1).toInt();
            if(tempcard_reader_address>99){tempcard_reader_address=1;}
            CardReader_Addr[0]=(tempcard_reader_address/10)+0x30;
            CardReader_Addr[1]=(tempcard_reader_address%10)+0x30;

            printing_header_data[0]=query.value(7).toString();
            printing_header_data[1]=query.value(8).toString();
            End_info[0]=query.value(5).toString();
            End_info[1]=query.value(6).toString();

            //qDebug()<<query.value(10).toString().indexOf("1")<<"bilmemki";//chn checkbox
            if(query.value(10).toString().indexOf("1")>=0)
            {
                askSellingStyle=true;
            }
            else
            {
                askSellingStyle=false;
            }
            qDebug()<<query.value(11).toString().indexOf("1")<<"bilmemkirr";//chn checkbox
            if(query.value(11).toString().indexOf("1")>=0)
            {
                askPlateSale=true;
            }
            else
            {
                askPlateSale=false;
            }


            if(query.value(9).toString().indexOf("1")>=0)
            {
                ask_km_after=false; // after ve before olarak değiştirilir. CHN
                ask_km=true;
            }
            else
            {
                ask_km_after=false;
                ask_km=false;
            }

            unit_price_dot_index=query.value(4).toInt();
            amount_dot_index=query.value(2).toInt();
            volume_dot_index=query.value(3).toInt();
            returnarray=true;

        }
        setting_values.commit();
        setting_values.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleProductName-Error Database";
    }
    return returnarray;
}


QString Database::NozzleProductPrice(int id)
{
    QSqlDatabase nozzle_product_price = QSqlDatabase::database("nozzle_product_price");
    QString returnarray="false";
    if(nozzle_product_price.open())
    {
        QSqlQuery query(nozzle_product_price);
        query.prepare("SELECT PRODUCT_PRICE FROM NOZZLE WHERE NOZZLE_ID = :id");
        query.bindValue(":id",id);
        query.exec();
        if(query.next())
        {
            returnarray=query.value(0).toString();
        }
        else returnarray="false";
        nozzle_product_price.commit();
        nozzle_product_price.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleProductPrice-Error Database";
    }
    return returnarray;
}


QString Database::NozzleTotalAmount(int id)
{
    QSqlDatabase nozzle_total_amount = QSqlDatabase::database("nozzle_total_amount");
    QString returnarray="false";
    if(nozzle_total_amount.open())
    {
        QSqlQuery query(nozzle_total_amount);
        query.prepare("SELECT TOTAL_AMOUNT FROM NOZZLE WHERE NOZZLE_ID = :id");
        query.bindValue(":id",id);
        query.exec();
        if(query.next())
        {
            returnarray=query.value(0).toString();
        }
        else returnarray="false";
        nozzle_total_amount.commit();
        nozzle_total_amount.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleTotalAmount-Error Database";
    }
    return returnarray;
}


QString Database::NozzleTotalVolume(int id)
{
    QSqlDatabase nozzle_total_volume = QSqlDatabase::database("nozzle_total_volume");
    QString returnarray="false";
    if(nozzle_total_volume.open())
    {
        QSqlQuery query(nozzle_total_volume);
        query.prepare("SELECT TOTAL_VOLUME FROM NOZZLE WHERE NOZZLE_ID = :id");
        query.bindValue(":id",id);
        query.exec();
        if(query.next())
        {
            returnarray=query.value(0).toString();
        }
        else returnarray="false";
        nozzle_total_volume.commit();
        nozzle_total_volume.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleTotalVolume-Error Database";
    }
    return returnarray;
}

QString Database::NozzleTotalSales(int id)
{
    QSqlDatabase nozzle_total_sales = QSqlDatabase::database("nozzle_total_sales");
    QString returnarray="false";
    if(nozzle_total_sales.open())
    {
        QSqlQuery query(nozzle_total_sales);
        query.prepare("SELECT TOTAL_SALES FROM NOZZLE WHERE NOZZLE_ID = :id");
        query.bindValue(":id",id);
        query.exec();
        if(query.next())
        {
            returnarray=query.value(0).toString();
        }
        else returnarray="false";
        nozzle_total_sales.commit();
        nozzle_total_sales.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"NozzleTotalSales-Error Database";
    }
    return returnarray;
}


void Database::InsertSales(QString id,int TSNO, int RNO, QString Tdate, QString Tclock, int PMPN, quint8 nozzle,quint8 prdctNo,quint8 tankNo, QByteArray unitprice, QByteArray liter, QByteArray amount, QByteArray bamount,QByteArray eamount, QByteArray bvolume, QByteArray evolume,quint8 Ftype, quint8 Ctype, quint8 Atype,quint8 Stype,quint8 Ptype, QString rfidCustomer, QString rfidDriver, QString rfidPerson, QByteArray plate, QString vehiclekm,QString vehicleruntime, QString NZLUT, QString NZLDT,QString RFRT,QString RFRET,QString AUTHT,QString FILLT,QString PCT, QString PFT, quint8 status)
{
    //Satışları database e eklediğimiz yer.
    QSqlDatabase insert_sales = QSqlDatabase::database("insert_sales");
    if(insert_sales.open())
    {
        QSqlQuery query(insert_sales);
        query.prepare("INSERT INTO '"+table_name+"' VALUES(:id, :TSNO, :RNO,:Tdate, :Tclock, :PMPN, :nozzle, :prdctNo, :tankNo, :unitprice, :liter, :amount, :bamount,:eamount, :bvolume, :evolume, :Ftype, :Ctype,:Atype,:Stype,:Ptype, :rfidT, :rfidC, :rfidP, :plate, :vehiclekm, :vehicleruntime, :NZLUT, :NZLDT, :RFRT, :RFRET, :AUTHT, :FILLT, :PCT, :PFT, :status)");
        query.bindValue(":id",id);
        query.bindValue(":TSNO",TSNO);
        query.bindValue(":RNO",RNO); //receip_no
        query.bindValue(":Tdate",Tdate);
        query.bindValue(":Tclock",Tclock);
        query.bindValue(":PMPN",PMPN);
        query.bindValue(":nozzle",nozzle);
        query.bindValue(":prdctNo",prdctNo);
        query.bindValue(":tankNo",tankNo);
        query.bindValue(":unitprice",QString::fromUtf8(unitprice.toHex()));
        query.bindValue(":liter",QString::fromUtf8(liter.toHex()));
        query.bindValue(":amount",QString::fromUtf8(amount.toHex()));
        query.bindValue(":bamount",QString::fromUtf8(bamount.toHex()));
        query.bindValue(":eamount",QString::fromUtf8(eamount.toHex()));
        query.bindValue(":bvolume",QString::fromUtf8(bvolume.toHex()));
        query.bindValue(":evolume",QString::fromUtf8(evolume.toHex()));
        query.bindValue(":Ftype",Ftype);
        query.bindValue(":Ctype",Ctype);
        query.bindValue(":Atype",Atype);
        query.bindValue(":Stype",Stype);
        query.bindValue(":Ptype",Ptype);
        query.bindValue(":rfidT",rfidCustomer);
        query.bindValue(":rfidC",rfidDriver);
        query.bindValue(":rfidP",rfidPerson);
        query.bindValue(":plate",QString::fromUtf8(plate));
        query.bindValue(":vehiclekm",vehiclekm);
        query.bindValue(":vehicleruntime",vehicleruntime);
        query.bindValue(":NZLUT",NZLUT);
        query.bindValue(":NZLDT",NZLDT);
        query.bindValue(":RFRT",RFRT);
        query.bindValue(":RFRET",RFRET);
        query.bindValue(":AUTHT",AUTHT);
        query.bindValue(":FILLT",FILLT);
        query.bindValue(":PCT",PCT);
        query.bindValue(":PFT",PFT);
        query.bindValue(":status",status);
        if(query.exec()){

            qDebug()<<"Sales is ok!!";
            insert_sales.commit();
            insert_sales.close();
            sync();
            system("sync");
        }
        else{
            if(test) qDebug()<<"InsertSales-Error Database:" +id<<query.lastError();
            qDebug()<<"InsertSales-Error Database:" +id<<query.lastError()<<query.lastQuery();
        }
    }
    else
    {
        if(test) qDebug()<<"InsertSales-Error Database";
        settings.setlog("DATABASE_DEBUG InsertSales-Error Database");
    }
}



void Database::delete_zeros(QString Tname)
{
    QSqlDatabase delete_tablee = QSqlDatabase::database("delete_table");
    if(delete_tablee.open())
    {
        QSqlQuery query(delete_tablee);
        query.exec("DELETE FROM '"+Tname+"' WHERE SALES_DATE=''");
        delete_tablee.commit();
        delete_tablee.close();
        sync();
        system("sync");
    }
    else
    {
        if(test)qDebug()<<"delete_zeros-Error Database";
        settings.setlog("DATABASE_DEBUG delete_zeros-Error Database");
    }
    
}

int Database::check_salesDate(QString tablo)
{
    QSqlDatabase sales_date = QSqlDatabase::database("sales_date");
    bool ok;
    QString count;
    if(sales_date.open())
    {
        QSqlQuery query(sales_date);
        query.prepare("SELECT COUNT(*) FROM "+tablo+" WHERE SALES_DATE=''");
        query.exec();
        if(query.isSelect())
        {
            query.first();
            count=query.value(0).toString();
        }
        sales_date.commit();
        sales_date.close();
        sync();
        system("sync");
    }
    else
    {
        if(test)qDebug()<<"check_salesDate-Error Database";
        settings.setlog("DATABASE_DEBUG check_salesDate-Error Database");
    }
    return count.toInt(&ok,10);
    
}


QJsonObject Database::kontrolSalesStatus()
{       //Status ü 0 olan satışları kontrol eder. JSON'ı oluşturur.
    QSqlDatabase sales_status = QSqlDatabase::database("sales_status");
    QJsonObject obj;
    QJsonArray sales;
    if(test)qDebug()<<"DATABASE_DEBUG kontrolSalesStatus";
    settings.setlog("DATABASE_DEBUG kontrolSalesStatus");
    if(sales_status.open())
    {
        QSqlQuery query(sales_status);
        query.prepare("SELECT * FROM '"+table_name+"' WHERE STATUS = '0'");
        query.exec();
        if(test) qDebug()<<query.lastError();
        int count=0;
        while(query.next())
        {
            QJsonObject sales_data;
            if(query.value(0).toString()!="")
            {
                sales_data["SLS_ID"]=query.value(0).toString();
            }
            else
            {
                sales_data["SLS_ID"]=QJsonValue::Null;
            }
            if(query.value(18).toString()!="")
            {
                sales_data["SLS_AUTTYPE"]=query.value(19).toInt();
            }
            else
            {
                sales_data["SLS_AUTTYPE"]=QJsonValue::Null;
            }
            sales_data["SLS_DEVICETYPE"]=8;
            sales_data["SLS_PRESETTYPE"]=2;// preset type 2 => litre
            sales_data["SLS_RECORDSTTS"]=3;
            sales_data["SLS_TRANSFERSTTS"]=1;
            sales_data["SLS_SHIFTID"]=QJsonValue::Null;
            if(query.value(17).toString()!="")
            {
                sales_data["SLS_CARDTPYE"]=query.value(17).toInt();
            }
            else
            {
                sales_data["SLS_CARDTPYE"]=QJsonValue::Null;
            }
            if(query.value(23).toString()!="")
            {
                sales_data["SLS_PERSONRFID"]=query.value(23).toString();
            }
            else
            {
                sales_data["SLS_PERSONRFID"]=QJsonValue::Null;
            }
            if(query.value(21).toString()!="")
            {
                sales_data["SLS_CUSTOMERRFID"]=query.value(21).toString();
            }
            else
            {
                sales_data["SLS_CUSTOMERRFID"]=QJsonValue::Null;
            }
            if(query.value(22).toString()!="")
            {
                sales_data["SLS_CHAFFEURRFID"]=query.value(22).toString();
            }
            else
            {
                sales_data["SLS_CHAFFEURRFID"]=QJsonValue::Null;
            }
            if(query.value(24).toString()!="")
            {
                sales_data["SLS_PLATE"]=query.value(24).toString();
            }
            else
            {
                sales_data["SLS_PLATE"]=QJsonValue::Null;
            }
            sales_data["SLS_PROVISIONID"]=QJsonValue::Null;
            sales_data["SLS_DCRNO"]=pumpNo; // DEĞİŞKEN 2
            if(query.value(5).toString()!="")
            {
                sales_data["SLS_PUMPNO"]=query.value(5).toInt();
            }
            else
            {
                sales_data["SLS_PUMPNO"]=QJsonValue::Null;
            }
            if(query.value(6).toString()!="")
            {
                sales_data["SLS_NOZZLENO"]=query.value(6).toInt();
            }
            else
            {
                sales_data["SLS_NOZZLENO"]=QJsonValue::Null;
            }
            sales_data["SLS_VIRTUALPUMPNO"]=virtual_pumpno;
            sales_data["SLS_VIRTUALNOZZLENO"]=settings.value("Virtual_nozzle"+QString::number(query.value(6).toInt())).toInt();
            if(query.value(7).toString()!="")
            {
                sales_data["SLS_PRODUCTNO"]=query.value(7).toInt();
            }
            else
            {
                sales_data["SLS_PRODUCTNO"]=QJsonValue::Null;
            }
            if(query.value(8).toString()!="")
            {
                sales_data["SLS_TANKNO"]=query.value(8).toInt();
            }
            else
            {
                sales_data["SLS_TANKNO"]=QJsonValue::Null;
            }
            if(query.value(11).toString()!="")
            {
                double salesamount=query.value(11).toDouble();
                salesamount/=qPow(10,amount_dot_index);
                sales_data["SLS_AMOUNT"]=QString::number(salesamount,'f',amount_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_AMOUNT"]=QJsonValue::Null;
            }

            if(query.value(10).toString()!="")
            {
                double salesliter=query.value(10).toDouble();
                salesliter/=qPow(10,volume_dot_index);
                sales_data["SLS_VOLUME"]=QString::number(salesliter,'f',volume_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_VOLUME"]=QJsonValue::Null;
            }

            if(query.value(9).toString()!="")
            {
                double unitprice=query.value(9).toDouble();
                unitprice/=qPow(10,unit_price_dot_index);
                sales_data["SLS_UNITPRICE"]=QString::number(unitprice,'f',unit_price_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_UNITPRICE"]=QJsonValue::Null;
            }

            sales_data["SLS_DFLTUNITPRICE"]=pump[pumpNo].nozzle[query.value(6).toInt()].Defaultunitprice.toDouble() ;// DEĞİŞKEN 3

            if(query.value(16).toString()!="")
            {
                sales_data["SLS_FILLINGTYPE"]=query.value(16).toInt();
            }
            else
            {
                sales_data["SLS_FILLINGTYPE"]=QJsonValue::Null;
            }

            if(query.value(20).toString()!="")
            {
                sales_data["SLS_PAYMENTTYPE"]=query.value(20).toInt();
            }
            else
            {
                sales_data["SLS_PAYMENTTYPE"]=QJsonValue::Null;
            }

            sales_data["SLS_FISCALID"]=QString::fromStdString("0");
            if(query.value(2).toString()!="")
            {
                sales_data["SLS_FISCALRECEIPTNO"]=query.value(2).toInt();
            }
            else
            {
                sales_data["SLS_FISCALRECEIPTNO"]=QJsonValue::Null;
            }
            if(query.value(25).toString()!="")
            {
                sales_data["SLS_KILOMETER"]=query.value(25).toInt();
            }
            else
            {
                sales_data["SLS_KILOMETER"]=0;
            }
            sales_data["SLS_PROCESS_STTS"]=QJsonValue::Null;

            if(query.value(12).toString()!="")
            {
                double unitprice=query.value(12).toDouble();
                unitprice/=qPow(10,amount_dot_index);
                sales_data["SLS_BGN_AMOUNTTOTALS"]=QString::number(unitprice,'f',amount_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_BGN_AMOUNTTOTALS"]=QJsonValue::Null;
            }

            if(query.value(13).toString()!="")
            {
                double unitprice=query.value(13).toDouble();
                unitprice/=qPow(10,amount_dot_index);
                sales_data["SLS_END_AMOUNTTOTALS"]=QString::number(unitprice,'f',amount_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_END_AMOUNTTOTALS"]=QJsonValue::Null;
            }

            if(query.value(14).toString()!="")
            {
                double unitprice=query.value(14).toDouble();
                unitprice/=qPow(10,volume_dot_index);
                sales_data["SLS_BGN_VOLUMETOTALS"]=QString::number(unitprice,'f',volume_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_BGN_VOLUMETOTALS"]=QJsonValue::Null;
            }

            if(query.value(15).toString()!="")
            {
                double unitprice=query.value(15).toDouble();
                unitprice/=qPow(10,volume_dot_index);
                sales_data["SLS_END_VOLUMETOTALS"]=QString::number(unitprice,'f',volume_dot_index).toDouble();
            }
            else
            {
                sales_data["SLS_END_VOLUMETOTALS"]=QJsonValue::Null;
            }

            sales_data["SLS_TANKFUELLEVEL"]=0;
            sales_data["SLS_TANKFUELVOLUME"]=0;
            if(query.value(33).toString()!="")
            {
                sales_data["SLS_RECORDTIME"]=query.value(33).toString();
            }
            else
            {
                sales_data["SLS_RECORDTIME"]=QJsonValue::Null;
            }
            sales_data["SLS_DCRTIME"]=QJsonValue::Null;
            sales_data["SLS_REQUESTID"]=QJsonValue::Null;
            sales_data["SLS_SEQUENCENO"]=QJsonValue::Null;
            sales_data["SLS_QUERYID"]=QJsonValue::Null;
            sales_data["SLS_VEHICLETIME"]=QString::fromStdString("1");
            sales_data["SLS_TANKTEMPERATURE"]=0;
            sales_data["SLS_TANKWATERLEVEL"]=0;
            sales_data["SLS_TANKWATERVOLUME"]=0;
            if(query.value(19).toInt()==21)
            {
                sales_data["SLS_CONNSTATUS"]=3;
            }
            else
            {
                sales_data["SLS_CONNSTATUS"]=1;
            } //DEĞİŞKEN}
            sales_data["SLS_DCRSERIALNO"]=serial_No.toInt();//DEĞİŞKEN
            sales_data["SLS_DCRERRORNO"]=2;//DEĞİŞKEN
            sales_data["SLS_PRINTERRCPTNO"]=0;
            sales_data["SLS_ISDELETED"]=QJsonValue::Null;
            sales_data["SLS_VEHICLEFIRM"]=QString::fromStdString("IETT");
            sales_data["SLS_INTEGRATIONID"]=QString::fromStdString("");
            sales_data["SLS_TERMINALINFO"]=QJsonValue::Null;
            sales_data["SLS_REALSALETYPE"]=QJsonValue::Null;
            sales_data["SLS_POSTXNID"]=QJsonValue::Null;
            sales_data["SLS_PLUSTXNID"]=QJsonValue::Null;

            if(query.value(26).toString()!="")
            {
                sales_data["SPT_NOZZLEUPTIME"]=query.value(26).toString();
            }
            else
            {
                sales_data["SPT_NOZZLEUPTIME"]=QJsonValue::Null;
            }

            if(query.value(27).toString()!="")
            {
                sales_data["SPT_NOZZLEDOWNTIME"]=query.value(27).toString();
            }
            else
            {
                sales_data["SPT_NOZZLEDOWNTIME"]=QJsonValue::Null;
            }

            if(query.value(28).toString()!="")
            {
                sales_data["SPT_RFIDREADTIME"]=query.value(28).toString();
            }
            else
            {
                sales_data["SPT_RFIDREADTIME"]=QJsonValue::Null;
            }

            if(query.value(29).toString()!="")
            {
                sales_data["SPT_RFIDRESULTTIME"]=query.value(29).toString();
            }
            else
            {
                sales_data["SPT_RFIDRESULTTIME"]=QJsonValue::Null;
            }

            if(query.value(30).toString()!="")
            {
                sales_data["SPT_AUTHORIZETIME"]=query.value(30).toString();
            }
            else
            {
                sales_data["SPT_AUTHORIZETIME"]=QJsonValue::Null;
            }

            if(query.value(31).toString()!="")
            {
                sales_data["SPT_FILLINGSTARTTIME"]=query.value(31).toString();
            }
            else
            {
                sales_data["SPT_FILLINGSTARTTIME"]=QJsonValue::Null;
            }

            if(query.value(32).toString()!="")
            {
                sales_data["SPT_PAIDCONFIGTIME"]=query.value(32).toString();
            }
            else
            {
                sales_data["SPT_PAIDCONFIGTIME"]=QJsonValue::Null;
            }


            if(query.value(33).toString()!="")
            {
                sales_data["SPT_PAIDFISCALTIME"]=query.value(33).toString();
            }
            else
            {
                sales_data["SPT_PAIDFISCALTIME"]=QJsonValue::Null;
            }


            sales.append(sales_data);
            count++;
            if(count==50) break;
        }
        obj.insert("Command",QString::fromStdString("LOSTSALES"));
        obj["SALES"]=sales;
        sales_status.commit();
        sales_status.close();
        sync();
        system("sync");
    }
    else
    {
        if(test)qDebug()<<"DATABASE_DEBUG Error-kontrolSalesStatus";
        settings.setlog("DATABASE_DEBUG  Error-kontrolSalesStatus");
    }
    return obj;
}

void Database::UpdateStatus2(QString id)
{   //usc satışı alamazsa database de o satış statusü 2 olur.
    QSqlDatabase update_status = QSqlDatabase::database("update_status");
    if(update_status.open())
    {
        QSqlQuery query(update_status);
        query.prepare("UPDATE '"+table_name+"' SET STATUS='2' WHERE SALES_ID=:id ");
        query.bindValue(":id",id);
        query.exec();
        update_status.commit();
        update_status.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}
void Database::UpdateStatus(QString id)
{   //satış statusünü günceller. Eğer usc satışı alırsa status 1 olur
    QSqlDatabase update_status = QSqlDatabase::database("update_status");
    if(update_status.open())
    {
        QSqlQuery query(update_status);
        query.prepare("UPDATE '"+table_name+"' SET STATUS='1' WHERE SALES_ID=:id ");
        query.bindValue(":id",id);
        query.exec();
        update_status.commit();
        update_status.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Error Database";
    }
}


void Database::Updatedateclock(QString id,QString date, QString clock)
{
    QSqlDatabase update_dateclock = QSqlDatabase::database("update_dateclock");
    bool ok;
    if(update_dateclock.open())
    {
        QSqlQuery query(update_dateclock);
        query.prepare("UPDATE '"+table_name+"' SET SALES_DATE=:date, SALES_CLOCK=:clock WHERE SALES_ID=:id ");
        query.bindValue(":id",id.toInt(&ok,10));
        query.bindValue(":date",date);
        query.bindValue(":clock",clock);
        query.exec();
        update_dateclock.commit();
        update_dateclock.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"Updatedateclock-Error Database";
    }
}

bool Database::UpdateSales(int id, QString date, QString clock, QString liter, QString price,QString amount)
{
    QSqlDatabase update_sales = QSqlDatabase::database("update_sales");
    bool return_exec=false;
    if(update_sales.open())
    {
        QSqlQuery query(update_sales);
        query.prepare("UPDATE '"+table_name+"' SET SALES_DATE=:date, SALES_CLOCK=:clock, SALES_LITER=:liter, UNIT_PRC=:pricem ,SALES_AMOUNT=:amount WHERE SALES_ID=:id ");
        query.bindValue(":id",id);
        query.bindValue(":date",date);
        query.bindValue(":clock",clock);
        query.bindValue(":liter",liter);
        query.bindValue(":pricem",price);
        query.bindValue(":amount",amount);
        return_exec=query.exec();
        update_sales.commit();
        update_sales.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdateSales-Error Database";
    }
    return return_exec;
}

void Database::UpdatePlate(int id, QString plate, QString vehiclekm, QString type)
{
    QSqlDatabase update_plate = QSqlDatabase::database("update_plate");
    if(update_plate.open())
    {
        QSqlQuery query(update_plate);
        query.prepare("UPDATE '"+table_name+"' SET PLATE=:plate, VEHICLEKM=:vehiclekm, SALES_TYPE=:type WHERE SALES_ID=:id ");
        query.bindValue(":id",id);
        query.bindValue(":plate",plate);
        query.bindValue(":vehiclekm",vehiclekm);
        query.bindValue(":type",type);
        query.exec();
        update_plate.commit();
        update_plate.close();
        sync();
        system("sync");
    }
    else
    {
        if(test) qDebug()<<"UpdatePlate-Error Database";
    }
}

int Database::totalsales(QString plate)
{  //satış yapılan plakanın günlük limitini kontrol eder.
    int total=0;
    QString date=QDate::currentDate().toString("yyMMdd");
    QSqlDatabase total_sales = QSqlDatabase::database("total_sales");
    if(test) qDebug()<<"totalsales"<<plate<<table_name<<date;
    if(total_sales.open())
    {
        QSqlQuery query(total_sales);
        query.prepare("SELECT SUM(SALES_LITER) FROM '"+table_name+"' WHERE PLATE = :plate AND SALES_DATE = :date");
        query.bindValue(":plate",plate);
        query.bindValue(":date",date);
        query.exec();
        if(test) qDebug()<<query.lastError();
        while(query.next())
        {
            total=query.value(0).toInt();
            if(test) qDebug()<<"totalsalestotal"<<QString::number(total);
        }
        total_sales.commit();
        total_sales.close();
    }
    return total;
}

