#include "mainwindow.h"
#include <QApplication>
#include <QtSerialPort/QtSerialPort>
#include "serial.h"
#include "scproutine.h"
#include "clrc.h"
#include "customerscreen.h"
#include "global.h"
#include "main_routine.h"
#include <QNetworkInterface>
#include <QCryptographicHash>
#include <stdio.h>
#include <stdlib.h>
#include "mytcpsocket.h"
#include <QJsonObject>
#include"syslog.h"
#include "savetofile.h"

QThread *serialThread[5];
QTimer *serialTimer[5];
int dec_to_bcd(int dec);
Database *database;
QByteArray convert_format(QByteArray data,unsigned char dot_ind);
void initserial(int portnumber)
{

    SCPArray[portnumber] = new serialport(portnumber);
    RoutineArray[portnumber] = new ScpRoutine();
    //RoutineArray[portnumber]->setPriority(QThread::HighestPriority);
    serialTimer[portnumber] = new QTimer;
    serialThread[portnumber] = new QThread;
    
    if(portnumber==2) //Pompa
    {
        QObject::connect(serialTimer[portnumber],SIGNAL(timeout()),RoutineArray[portnumber],SLOT(routine_2()));
    }
    else if(portnumber==1) //Printer
    {
        QObject::connect(serialTimer[portnumber],SIGNAL(timeout()),RoutineArray[portnumber],SLOT(routine()));
    }
    else if(portnumber==3) //RFID
    {
        QObject::connect(serialTimer[portnumber],SIGNAL(timeout()),RoutineArray[portnumber],SLOT(routine_3()));
    }
    else if(portnumber==4) //USC || PROBE
    {
        QObject::connect(serialTimer[portnumber],SIGNAL(timeout()),RoutineArray[portnumber],SLOT(routine_4()));
    }
    SCPArray[portnumber]->moveToThread(serialThread[portnumber]);
    RoutineArray[portnumber]->moveToThread(serialThread[portnumber]);
    
    if(portnumber==2)
    {
        serialTimer[portnumber]->start(80);
    }
    else if(portnumber==1)
    {
        serialTimer[portnumber]->start(250);
    }
    else if(portnumber==3)
    {
        serialTimer[portnumber]->start(500);
    }
    else if(portnumber==4)
    {
        if(upc_locvis==false)
            serialTimer[portnumber]->start(5);
        else  serialTimer[portnumber]->start(100);
    }
    serialTimer[portnumber]->moveToThread(serialThread[portnumber]);
    serialThread[portnumber]->start();

}


void convert_bytearray_to_string()
{
    int ll;
    QSettings settings(SettingsFile, QSettings::IniFormat);
    DCR_par.DCR_Nozzle_Number=settings.value("DCR_par.DCR_Nozzle_Number", "").toInt(&ok);
    DCR_par.Active_Nozzle=DCR_par.DCR_Nozzle_Number;
    Number_of_Nozzle=settings.value("Number_of_Nozzle", "").toInt(&ok);

    for(ll=1; ll<=Number_of_Nozzle;ll++)
    {
        pump[pumpNo].nozzle[ll].BT_Volume=settings.value("Begin_total_volume_"+QString::number(ll), "").toByteArray();
        settings.setValue("Begin_total_volume_"+QString::number(ll),pump[pumpNo].nozzle[ll].BT_Volume.toHex());

        pump[pumpNo].nozzle[ll].T_Volume=settings.value("End_total_volume_"+QString::number(ll), "").toByteArray();
        settings.setValue("End_total_volume_"+QString::number(ll),pump[pumpNo].nozzle[ll].T_Volume.toHex());


        pump[pumpNo].nozzle[ll].BT_Amount=settings.value("Begin_total_amount_"+QString::number(ll), "").toByteArray();
        settings.setValue("Begin_total_amount_"+QString::number(ll),pump[pumpNo].nozzle[ll].BT_Amount.toHex());


        pump[pumpNo].nozzle[ll].T_Amount=settings.value("End_total_amount_"+QString::number(ll), "").toByteArray();
        settings.setValue("End_total_amount_"+QString::number(ll),pump[pumpNo].nozzle[ll].T_Amount.toHex());


        pump[pumpNo].nozzle[ll].USC_T_Volume=settings.value("End_total_volume_USC_"+QString::number(ll), "").toByteArray();
        settings.setValue("End_total_volume_USC_"+QString::number(ll),pump[pumpNo].nozzle[ll].USC_T_Volume.toHex());

        pump[pumpNo].nozzle[ll].USC_T_Amount=settings.value("End_total_amount_USC_"+QString::number(ll), "").toByteArray();
        settings.setValue("End_total_amount_USC_"+QString::number(ll),pump[pumpNo].nozzle[ll].USC_T_Amount.toHex());


        pump[pumpNo].nozzle[ll].filling_price_for_USC=settings.value("filling_price_for_USC_"+QString::number(ll), "").toByteArray();
        settings.setValue("filling_price_for_USC_"+QString::number(ll),pump[pumpNo].nozzle[ll].filling_price_for_USC.toHex());

        pump[pumpNo].nozzle[ll].filling_volume_for_USC=settings.value("filling_volume_for_USC_"+QString::number(ll), "").toByteArray();
        settings.setValue("filling_volume_for_USC_"+QString::number(ll),pump[pumpNo].nozzle[ll].filling_volume_for_USC.toHex());

        pump[pumpNo].nozzle[ll].filling_amount_for_USC=settings.value("filling_amount_for_USC_"+QString::number(ll), "").toByteArray();
        settings.setValue("filling_amount_for_USC_"+QString::number(ll),pump[pumpNo].nozzle[ll].filling_amount_for_USC.toHex());

    }

    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price=settings.value("saved_unit_price", "").toByteArray();
    settings.setValue("saved_unit_price",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price.toHex());

    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=settings.value("last_customer_plate", "").toByteArray();
    settings.setValue("last_customer_plate",pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex());

    QByteArray Bytearraysaved_day=settings.value("saved_day", "").toByteArray();
    settings.setValue("saved_day",Bytearraysaved_day.toHex());

    settings.setValue("change_file","1");

    settings.setValue("Version_config", 0);

}

void convert_setting()
{

    QSettings settingconfig(SettingsFile, QSettings::IniFormat);
    for(int i=0;i<settingconfig.childKeys().length();i++)
    {
        settings.setValue(settingconfig.childKeys().at(i),settingconfig.value(settingconfig.childKeys().at(i)));
        //        sync();
        //        system("sync");
    }
    //    QThread::msleep(2000);
    //    system(command_reboot.toStdString().c_str());
}


void loadSettings()
{
    QString str;
    using namespace std;
    QVariant  cv;
    // QSettings settings(SettingsFile, QSettings::IniFormat);
    unsigned ll;


    pump[pumpNo].status=0;

    settings.setValue("upc_locvis",true);
    upc_locvis=settings.value("upc_locvis").toBool();
    select_version=settings.value("select_version").toInt(&ok);
    select_version=1;
    switch (select_version)
    {
    case 1:Myversion="1.1.0";  //not crypted version
        break;
    case 2:Myversion="1.1.41";  //different CRC
        break;
    case 3:Myversion="1.1.42";  //Crypted
        break;
    default:
        break;
    }


    settings.setValue("Myversion",Myversion);
    sync();
    system("sync");
    tts_pause_count=settings.value("tts_pause_count").toInt(&ok);
    if(tts_pause_count==0)
    {
        tts_pause_count=2;
        settings.setValue("tts_pause_count",tts_pause_count);
    }
    ttspause=settings.value("ttspause").toBool();


    DCR_par.DCR_Nozzle_Number=settings.value("DCR_par.DCR_Nozzle_Number").toInt(&ok);
    DCR_par.Active_Nozzle=DCR_par.DCR_Nozzle_Number;
    settings.setlog("LOAD_SETTING DCR_par.DCR_Nozzle_Number "+QString::number(DCR_par.DCR_Nozzle_Number));
    for(ll=1;ll<=Number_of_Nozzle;ll++)
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount[0]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount[1]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount[2]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount[3]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume[0]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume[1]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume[2]=0;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume[3]=0;
        for(int bb=0; bb<16;bb++)
        {
            TAGID[ll][bb]=0;
        }
    }
    for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
    {
        TEXT_fromMaster[jj]=0x20;
    }

    if(upc_locvis)
    {
        USC_is_exist=false;
        system_started=false;
    }
    else
    {
        USC_is_exist=true;
        system_started=true;
    }

    Nozzle_up_flg=settings.value("Nozzle_up_flg").toInt();
    if(Nozzle_up_flg) clrc_up_flg=1;
    Pump_Filling_status=settings.value("Pump_Filling_status").toBool();
    //qDebug()<<"Pump_Filling_status"<<Pump_Filling_status;
    TS_no=settings.value("TS_no").toInt(&ok);
    fisno=TS_no;

    //saha için şimdilik kapalı
    wait_km_second=settings.value("wait_km_second").toInt(&ok);
    dec_to_bcd(fisno);
    pump[pumpNo].Fiscal_R_Number[1]=bcd[1];
    pump[pumpNo].Fiscal_R_Number[0]=bcd[0];


    commokpswrd=settings.value("commokpswrd").toString();
    otopswrd=settings.value("otopswrd").toString();
    usc_ip=settings.value("usc_ip").toString();
    usc_port=settings.value("usc_port").toString();

    Product_KDV=settings.value("Product_KDV").toInt(&ok);
    Card_reader_chose=settings.value("Card_reader_chose").toBool();
    Preset_Control_flg=settings.value("Preset_Control_flg").toBool();
    table_name="SALES";
    pump_sta=settings.value("pump_sta").toBool();
    pump_sta_2=settings.value("pump_sta_2").toBool();
    auttype_3_4=settings.value("auttype_3_4").toBool();
    allow_button_plate=settings.value("allow_button_plate").toBool();
    serial_debug_flg=settings.value("serial_debug_flg").toBool();
    //qDebug()<<"pump_sta"<<pump_sta<<pump_sta_2;
    if((pump_sta==true) || (pump_sta_2==true)) //satışta program kapandı ve databaseden önceki bilgileri alma buradan al
    {
        if(pump_sta_2==true){pump_sta=true;}
        system_shutdown_count=settings.value("system_shutdown_count").toInt();
        system_shutdown_count++;
        if(system_shutdown_count>=30000){system_shutdown_count=0;}
        settings.setValue("system_shutdown_count",system_shutdown_count);
        //qDebug()<<"Program satışdayken gitti"<<system_shutdown_count;
        settings.setlog(" LOAD_SETTING Program satışdayken gitti");
        Nozzle_Down_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
        settings.setValue("system_shutdown_date",Nozzle_Down_Time);
        Nozzle_Down_Time.prepend("20");
        if(test)qDebug()<<"Nozzle_Down_Time"<<Nozzle_Down_Time;
        settings.setlog(" LOAD_SETTING Nozzle_Down_Time "+Nozzle_Down_Time);
        reboot_ok=true; //palaka girildiğinde değeri true olan değişken program kapandığında 30 sn sonra false olmasını engeller.
    }
    TS_date=settings.value("TS_date").toString();
    if(test)qDebug() <<"TS_date"<<TS_date;
    TS_time=settings.value("TS_time").toString();
    if(test)qDebug() <<"TS_time"<<TS_time;
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price=QByteArray::fromHex(settings.value("saved_unit_price").toByteArray());
    //qDebug()<<"deneme-filling_price"<<QByteArray::fromHex(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price).toHex()<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price;
    Sale_Error_code=settings.value("Sale_Error_code").toInt(&ok);
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].BT_Volume=QByteArray::fromHex(settings.value("Begin_total_volume_"+QString::number(DCR_par.DCR_Nozzle_Number)).toByteArray());
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Volume=QByteArray::fromHex(settings.value("End_total_volume_"+QString::number(DCR_par.DCR_Nozzle_Number)).toByteArray());
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].BT_Amount=QByteArray::fromHex(settings.value("Begin_total_amount_"+QString::number(DCR_par.DCR_Nozzle_Number)).toByteArray());
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Amount=QByteArray::fromHex(settings.value("End_total_amount_"+QString::number(DCR_par.DCR_Nozzle_Number)).toByteArray());
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=settings.value("FTYP").toInt(&ok);
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].CTYP=settings.value("CTYP").toInt(&ok);
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP=settings.value("ATYP").toInt(&ok);
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP=settings.value("STYP").toInt(&ok);
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PATYP=settings.value("PATYP").toInt(&ok);
    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=QByteArray::fromHex(settings.value("last_customer_plate").toByteArray());
    Nozzle_Up_Time=settings.value("Nozzle_Up_Time").toString();
    RFID_Read_Time=settings.value("RFID_Read_Time").toString();
    RFID_Result_Time=settings.value("RFID_Result_Time").toString();
    if((pump_sta==true) || (pump_sta_2==true))
    {
        RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.indexOf(":")+1,2,"00");
        //qDebug()<<"RFID_Read_Time"<<RFID_Read_Time;
    }
    Authorize_Time=settings.value("Authorize_Time").toString();
    Filling_Start_Time=settings.value("Filling_Start_Time").toString();
    Paid_Confrim_Time=settings.value("Paid_Confrim_Time").toString();
    Paid_Fiscal_Time=settings.value("Paid_Fiscal_Time").toString();
    pump[pumpNo].KM=settings.value("KM").toString();
    Person_is_Ok=settings.value("Person_is_Ok").toBool();

    number_of_salessave=settings.value("number_of_salessave").toInt();
    //***ask_km_after=settings.value("ask_km").toBool();
    //ask_km=false;
    plate_ini_flg=settings.value("plate_ini_flg").toBool();
    Psifre=settings.value("Psifre").toString();
    Float_setting=settings.value("Float_setting").toString();
    Metre=settings.value("Metre").toString();
    ABU_address=settings.value("ABU_address").toInt();
    if(test)qDebug() <<"ABU_address"<<ABU_address;

    cash_register_pump_no=ABU_address;
    ABU_addr=79 + ABU_address;
    if(test)qDebug() <<"ABU_address"<<ABU_addr;
    settings.setlog(" LOAD_SETTING ABU_address "+QString::number(ABU_address));
    serial_No.clear();
    serial_No.append(QString::number(ABU_address).rightJustified(8,'0'));
    pump[pumpNo].Fiscal_ID=serial_No;
    if(test)qDebug() <<"pump[pumpNo].Fiscal_ID"<<pump[pumpNo].Fiscal_ID;
    settings.setlog(" LOAD_SETTING Fiscal_ID "+QString::fromUtf8(pump[pumpNo].Fiscal_ID.toHex()));
    virtual_pumpno=settings.value("virtual_pumpno").toInt();
    lock_day=settings.value("number_of_version").toInt();
    if(lock_day<3){lock_day=3;}
    if(lock_day>10){lock_day=10;}
    if(test)qDebug() <<"number_of_version"<<lock_day;
    /*******if(Float_setting=="EVET")
    {
        unit_price_dot_index=settings.value("Unit_price_dot_index").toInt(&ok);
        amount_dot_index=settings.value("Amount_dot_index").toInt(&ok);
        volume_dot_index=settings.value("Volume_dot_index").toInt(&ok);
    }
    else
    {
        if(Metre=="BARANSAY")
        {
            unit_price_dot_index=3;
            amount_dot_index=3;
            volume_dot_index=3;
        }
        else if(Metre=="IPT")
        {
            unit_price_dot_index=3;
            amount_dot_index=2;
            volume_dot_index=2;
        }
        else if(Metre=="MITES")
        {
            unit_price_dot_index=3;
            amount_dot_index=2;
            volume_dot_index=2;
        }

    }*/


    for(ll=1; ll<=Number_of_Nozzle;ll++)
    {
        pump[pumpNo].nozzle[ll].BT_Volume=QByteArray::fromHex(settings.value("Begin_total_volume_"+QString::number(ll)).toByteArray());
        if(test)qDebug() <<"Begin_total_volume_"+QString::number(ll)<<pump[pumpNo].nozzle[ll].BT_Volume.toHex();
        settings.setlog(" LOAD_SETTING Begin_total_volume_ "+QString::number(ll)+"  "+QString::fromUtf8(pump[pumpNo].nozzle[ll].BT_Volume.toHex()));
        pump[pumpNo].nozzle[ll].T_Volume=QByteArray::fromHex(settings.value("End_total_volume_"+QString::number(ll)).toByteArray());
        if(test)qDebug() <<"End_total_volume_"+QString::number(ll)<<pump[pumpNo].nozzle[ll].T_Volume.toHex();
        settings.setlog(" LOAD_SETTING End_total_volume_ "+QString::number(ll)+"  "+QString::fromUtf8(pump[pumpNo].nozzle[ll].T_Volume.toHex()));
        pump[pumpNo].nozzle[ll].BT_Amount=QByteArray::fromHex(settings.value("Begin_total_amount_"+QString::number(ll)).toByteArray());
        if(test)qDebug() <<"Begin_total_amount_"+QString::number(ll)<<pump[pumpNo].nozzle[ll].BT_Amount.toHex();
        settings.setlog(" LOAD_SETTING Begin_total_amount_ "+QString::number(ll)+"  "+QString::fromUtf8(pump[pumpNo].nozzle[ll].BT_Amount.toHex()));
        pump[pumpNo].nozzle[ll].T_Amount=QByteArray::fromHex(settings.value("End_total_amount_"+QString::number(ll)).toByteArray());
        if(test)qDebug() <<"End_total_amount_"+QString::number(ll)<<pump[pumpNo].nozzle[ll].T_Amount.toHex();
        settings.setlog(" LOAD_SETTING Begin_total_amount_ "+QString::number(ll)+"  "+QString::fromUtf8(pump[pumpNo].nozzle[ll].T_Amount.toHex()));
        pump[pumpNo].nozzle[ll].USC_T_Volume=QByteArray::fromHex(settings.value("End_total_volume_USC_"+QString::number(ll)).toByteArray());
        if(test)qDebug() <<"End_total_volume_USC_"+QString::number(ll)<<pump[pumpNo].nozzle[ll].USC_T_Volume.toHex();
        settings.setlog(" LOAD_SETTING End_total_volume_USC "+QString::number(ll)+"  "+QString::fromUtf8(pump[pumpNo].nozzle[ll].USC_T_Volume.toHex()));
        pump[pumpNo].nozzle[ll].USC_T_Amount=QByteArray::fromHex(settings.value("End_total_amount_USC_"+QString::number(ll)).toByteArray());
        if(test)qDebug() <<"End_total_amount_USC_"+QString::number(ll)<<pump[pumpNo].nozzle[ll].USC_T_Amount.toHex();
        settings.setlog(" LOAD_SETTING End_total_amount_USC_ "+QString::number(ll)+"  "+QString::fromUtf8(pump[pumpNo].nozzle[ll].USC_T_Amount.toHex()));

        pump[pumpNo].nozzle[ll].filling_price_for_USC=QByteArray::fromHex(settings.value("filling_price_for_USC_"+QString::number(ll)).toByteArray());
        pump[pumpNo].nozzle[ll].filling_volume_for_USC=QByteArray::fromHex(settings.value("filling_volume_for_USC_"+QString::number(ll)).toByteArray());
        pump[pumpNo].nozzle[ll].filling_amount_for_USC=QByteArray::fromHex(settings.value("filling_amount_for_USC_"+QString::number(ll)).toByteArray());

        if(pump[pumpNo].nozzle[ll].filling_price_for_USC=="")
        {
            pump[pumpNo].nozzle[ll].filling_price[0]=0;
            pump[pumpNo].nozzle[ll].filling_price[1]=0;
            pump[pumpNo].nozzle[ll].filling_price[2]=0;
        }
        else
        {
            pump[pumpNo].nozzle[ll].filling_price=pump[pumpNo].nozzle[ll].filling_price_for_USC;
        }
        if(pump[pumpNo].nozzle[ll].filling_volume_for_USC=="")
        {
            pump[pumpNo].nozzle[ll].filling_volume[0]=0;
            pump[pumpNo].nozzle[ll].filling_volume[1]=0;
            pump[pumpNo].nozzle[ll].filling_volume[2]=0;
            pump[pumpNo].nozzle[ll].filling_volume[3]=0;
        }
        else
        {
            // pump[pumpNo].nozzle[ll].filling_volume=pump[pumpNo].nozzle[ll].filling_volume_for_USC;
            pump[pumpNo].nozzle[ll].filling_volume[0]=0;
            pump[pumpNo].nozzle[ll].filling_volume[1]=0;
            pump[pumpNo].nozzle[ll].filling_volume[2]=0;
            pump[pumpNo].nozzle[ll].filling_volume[3]=0;
        }
        if(pump[pumpNo].nozzle[ll].filling_amount_for_USC=="")
        {
            pump[pumpNo].nozzle[ll].filling_amount[0]=0;
            pump[pumpNo].nozzle[ll].filling_amount[1]=0;
            pump[pumpNo].nozzle[ll].filling_amount[2]=0;
            pump[pumpNo].nozzle[ll].filling_amount[3]=0;
        }
        else
        {
            // pump[pumpNo].nozzle[ll].filling_amount=pump[pumpNo].nozzle[ll].filling_amount_for_USC;
            pump[pumpNo].nozzle[ll].filling_amount[0]=0;
            pump[pumpNo].nozzle[ll].filling_amount[1]=0;
            pump[pumpNo].nozzle[ll].filling_amount[2]=0;
            pump[pumpNo].nozzle[ll].filling_amount[3]=0;
        }
    }

    Coding_key[0]=0x41;
    Coding_key[1]=0x4D;
    Coding_key[2]=0x49;
    Coding_key[3]=0x4E;
    Version_config_ini=settings.value("Version_config").toString();
    if(test)qDebug() <<"Float_setting"<<Float_setting;
    settings.setlog(" LOAD_SETTING Float_setting "+Float_setting);
    if(test)qDebug() <<"unit_price_dot_index"<<QString::number(unit_price_dot_index);
    settings.setlog(" LOAD_SETTING unit_price_dot_index "+QString::number(unit_price_dot_index));
    if(test)qDebug() <<"amount_dot_index"<<QString::number(amount_dot_index);
    settings.setlog(" LOAD_SETTING amount_dot_index "+QString::number(amount_dot_index));
    if(test)qDebug() <<"volume_dot_index"<<QString::number(volume_dot_index);
    settings.setlog(" LOAD_SETTING volume_dot_index "+QString::number(volume_dot_index));
    if(test)qDebug() <<"printing_header_data[0]"<<printing_header_data[0];
    if(test)qDebug() <<"printing_header_data[1]"<<printing_header_data[1];
    if(test)qDebug() <<"printing_header_data[2]"<<printing_header_data[2];
    if(test)qDebug() <<"printing_header_data[3]"<<printing_header_data[3];
    if(test)qDebug() <<"End_info[0]"<<End_info[0];
    if(test)qDebug() <<"End_info[1]"<<End_info[1];
    settings.setlog(" LOAD_SETTING printing_header_data[0] "+printing_header_data[0]);
    settings.setlog(" LOAD_SETTING printing_header_data[1] "+printing_header_data[1]);
    settings.setlog(" LOAD_SETTING printing_header_data[2] "+printing_header_data[2]);
    settings.setlog(" LOAD_SETTING printing_header_data[3] "+printing_header_data[3]);
    settings.setlog(" LOAD_SETTING End_info[0] "+End_info[0]);
    settings.setlog(" LOAD_SETTING End_info[1] "+End_info[1]);

    if(test)qDebug() <<"Card_reader_chose"<<Card_reader_chose;
    if(test)qDebug() <<"Preset_Control_flg"<<Preset_Control_flg;
    if(test)qDebug() <<"table_name"<<table_name;
    if(test)qDebug() <<"last_customer_plate"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
    if(test)qDebug() <<"number_of_salessave"<<number_of_salessave;
    if(test)qDebug() <<"pump_sta"<<pump_sta;
    if(test)qDebug() <<"saved_unit_price"<<saved_unit_price;
    if(test)qDebug() <<"ask_km"<<ask_km;
    if(test)qDebug() <<"plate_ini_flg"<<plate_ini_flg;
    if(test)qDebug() <<"Metre_Protocol"<<Metre;
    if(test)qDebug() <<"Nozzle_up_flg"<<Nozzle_up_flg;
    if(test)qDebug() <<"Pump_Filling_status"<<Pump_Filling_status;


    settings.setlog(" LOAD_SETTING Card_reader_chose "+QString::number(Card_reader_chose));
    settings.setlog(" LOAD_SETTING Preset_Control_flg "+QString::number(Preset_Control_flg));
    settings.setlog(" LOAD_SETTING table_name "+table_name);
    settings.setlog(" LOAD_SETTING last_customer_plate "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex()));
    settings.setlog(" LOAD_SETTING number_of_salessave "+QString::number(number_of_salessave));
    settings.setlog(" LOAD_SETTING pump_sta "+QString::number(pump_sta));
    settings.setlog(" LOAD_SETTING saved_unit_price "+QString::fromUtf8(saved_unit_price.toHex()));
    settings.setlog(" LOAD_SETTING plate_ini_flg "+QString::number(plate_ini_flg));
    settings.setlog(" LOAD_SETTING ask_km "+QString::number(ask_km));
    settings.setlog(" LOAD_SETTING Metre_Protocol "+Metre);

    /////////////////////////////////
    targetPath=settings.value("Gprs_Server").toString();
    savePath=settings.value("savePath").toString(); // root/Desktop/Yeni
    targetPath.append("/TankerUpdate");
    timeoutTime=settings.value("timeoutTime").toInt();
    downloadIniFile=settings.value("downloadIniFile").toString(); // version.txt
    downloadExFile=settings.value("downloadExFile").toString(); // PCR17_v4.zip
    saveIniFile=settings.value("saveIniFile").toString();    // version.ini
    currentPath=settings.value("currentPath").toString(); // /root/Desktop/Version_updater/Updater
    MyVersionDate=settings.value("MyVersionDate").toString();
    Myversion=settings.value("Myversion").toString();
    server=settings.value("Gprs_Server").toString();
    //imei=settings.value("Gprs_Imei").toString();
    debug_flg=settings.value("debug_flg").toBool();
    settingPath=settings.value("settingPath").toString();
    gateWay=settings.value("GateWay").toString();
    databasePath=settings.value("databasePath").toString();
    totalLT=settings.value("totalLT").toString();
    zipFile=savePath;
    zipFile.append(downloadExFile);
    check_dowloadIniFile=savePath;
    check_dowloadIniFile.append(saveIniFile);
    QDate updatedDate(QDate::fromString(QString::fromStdString(CryptoObj.decrypt(settings.value("UpdateBalanceLimit").toString().toStdString())),"ddMMyyyy"));

    qDebug()<<"-->>"<<QDate::currentDate().toJulianDay()-updatedDate.toJulianDay();
    if(QDate::currentDate().toJulianDay()-updatedDate.toJulianDay()>0){
        database->updateAllBalanceLimit();
        settings.setValue("UpdateBalanceLimit",QString::fromStdString(CryptoObj.encrypt(QDateTime::currentDateTime().toString("ddMMyyyy").toStdString())));
    }
    sync();
    system("sync");
}

void loadLanguage(){
    records_String=LanguageFile->readVariables("records").toString();
    enter_String=LanguageFile->readVariables("enter").toString();
    cancel_String=LanguageFile->readVariables("cancel").toString();
    clean_String=LanguageFile->readVariables("clean").toString();
    enterPlate_String=LanguageFile->readVariables("enterPlate").toString();
    pleaseCheckPumpConnection_String=LanguageFile->readVariables("pleaseCheckPumpConnection").toString();
    pumpCommunicationError_String=LanguageFile->readVariables("pumpCommunicationError").toString();
    menu_String=LanguageFile->readVariables("menu").toString();
    enterKilometer_String=LanguageFile->readVariables("enterKilometer").toString();
    whitelist_String=LanguageFile->readVariables("whitelist").toString();
    attendant_String=LanguageFile->readVariables("attendent").toString();
    sales_String=LanguageFile->readVariables("sales").toString();
    recordedSettings_String=LanguageFile->readVariables("recordedSettings").toString();
    back_String=LanguageFile->readVariables("back").toString();
    enterMenuPassword_String=LanguageFile->readVariables("enterMenuPassword").toString();
    readyToSale_String=LanguageFile->readVariables("readyToSale").toString();
    wrongPassword_String=LanguageFile->readVariables("wrongPassword").toString();
    readAccesspoint_String=LanguageFile->readVariables("readAccesspoint").toString();
    writeAccesspoint_String=LanguageFile->readVariables("writeAccesspoint").toString();
    visOpen_String=LanguageFile->readVariables("visOpen").toString();
    deviceAdress_String=LanguageFile->readVariables("deviceAdress").toString();
    pumpNozzleNumber_String=LanguageFile->readVariables("pumpNozzleNumber").toString();
    cardReaderAdress_String=LanguageFile->readVariables("cardReaderAdress").toString();
    askKm_String=LanguageFile->readVariables("askKm").toString();
    litreDecPoint_String=LanguageFile->readVariables("litreDec.Point").toString();
    amountDecPoint_String=LanguageFile->readVariables("amountDec.Point").toString();
    uPriceDecPoint_String=LanguageFile->readVariables("u.PriceDec.Point").toString();
    save_String=LanguageFile->readVariables("save").toString();
    deviceSet_String=LanguageFile->readVariables("deviceSet.").toString();
    receiptSet_String=LanguageFile->readVariables("receiptSet").toString();
    productSet_String=LanguageFile->readVariables("productSet").toString();
    testMode_String=LanguageFile->readVariables("testMode").toString();
    accesspoint_String=LanguageFile->readVariables("accesspoint").toString();
    printer_String=LanguageFile->readVariables("printer").toString();
    cardReader_String=LanguageFile->readVariables("cardReader").toString();
    startTest_String=LanguageFile->readVariables("startTest").toString();
    nozzle1Taken_String=LanguageFile->readVariables("nozzle1Taken").toString();
    pleaseReadCard_String=LanguageFile->readVariables("pleaseReadCard").toString();
    pleaseReadAttendantCard_String=LanguageFile->readVariables("pleaseReadAttendantCard").toString();
    attendantCard_String=LanguageFile->readVariables("attendantCard").toString();
    customerCard_String=LanguageFile->readVariables("customerCard").toString();
    visTag_String=LanguageFile->readVariables("visTag").toString();
    wrongCard_String=LanguageFile->readVariables("wrongCard").toString();
    blackList_String=LanguageFile->readVariables("blackList").toString();
    loyalty_String=LanguageFile->readVariables("loyalty").toString();
    error_String=LanguageFile->readVariables("error").toString();
    pleaseReadDriverCard_String=LanguageFile->readVariables("pleaseReadDriverCard").toString();
    nozzleSettingsAreMissing_String=LanguageFile->readVariables("nozzleSettingsAreMissing").toString();
    waitingSettingsFromLocwiz_String=LanguageFile->readVariables("waitingSettingsFromLocwiz").toString();

    automationErrorEnterPassword_String=LanguageFile->readVariables("automationErrorEnterPassword").toString();
    autoAdr01_String=LanguageFile->readVariables("autoAdr01").toString();
    accesspointSettingsReadingSuccessful_String=LanguageFile->readVariables("accesspointSettingsReadingSuccessful").toString();
    nozAdr01_String=LanguageFile->readVariables("nozAdr01").toString();
    channel01_String=LanguageFile->readVariables("channel01").toString();
    stopZ08_String=LanguageFile->readVariables("stopZ08").toString();
    otoAdr_String=LanguageFile->readVariables("otoAdr").toString();
    nozzAdr_String=LanguageFile->readVariables("nozzAdr").toString();
    channelNo_String=LanguageFile->readVariables("channelNo").toString();
    stopZ_String=LanguageFile->readVariables("stopZ").toString();
    nozzle_String=LanguageFile->readVariables("nozzle").toString();
    menuPass_String=LanguageFile->readVariables("menuPass").toString();
    manualAllowPassword_String=LanguageFile->readVariables("manualAllowPassword").toString();
    updateSuccessful_String=LanguageFile->readVariables("updateSuccessful").toString();
    systemInitializing_String=LanguageFile->readVariables("systemInitializing").toString();
    updateFailed_String=LanguageFile->readVariables("updateFailed").toString();
    noNewUpdatesFound_String=LanguageFile->readVariables("noNewUpdatesFound").toString();
    updateDeviceNotFound_String=LanguageFile->readVariables("updateDeviceNotFound").toString();
    saleToReady_String=LanguageFile->readVariables("saleToReady").toString();
    invalidLicence_String=LanguageFile->readVariables("invalidLicence").toString();
    enterDateTime_String=LanguageFile->readVariables("enterDateTime").toString();
    accesspointSettingsReadingFailed_String=LanguageFile->readVariables("accesspointSettingsReadingFailed").toString();
    otoAdr1_String=LanguageFile->readVariables("otoAdr1").toString();
    nozAdr1_String=LanguageFile->readVariables("nozAdr1").toString();
    channel1_String=LanguageFile->readVariables("channel1").toString();
    stopZ1_String=LanguageFile->readVariables("stopZ1").toString();
    accesspointSettingsUploadSuccessful_String=LanguageFile->readVariables("accesspointSettingsUploadSuccessful").toString();
    accesspointSettingsUploadFailed_String=LanguageFile->readVariables("accesspointSettingsUploadFailed").toString();
    accesspointSettingsReadingPleaseWait_String=LanguageFile->readVariables("accesspointSettingsReadingPleaseWait").toString();
    settingsSaved_String=LanguageFile->readVariables("settingsSaved").toString();
    systemWillBeRestarted_String=LanguageFile->readVariables("systemWillBeRestarted").toString();
    deviceIsLocked_String=LanguageFile->readVariables("deviceIsLocked").toString();
    uscActive_String=LanguageFile->readVariables("uscActive").toString();
    deviceSettingsAreMissing_String=LanguageFile->readVariables("deviceSettingsAreMissing").toString();
    systemLocked_String=LanguageFile->readVariables("systemLocked").toString();
    loadingAccesspointSettingsPleaseWait_String=LanguageFile->readVariables("loadingAccesspointSettingsPleaseWait").toString();

    fieldMode_String=LanguageFile->readVariables("fieldMode").toString();
    activateLicence_String=LanguageFile->readVariables("activateLicence").toString();
    backUpToSdCard_String=LanguageFile->readVariables("backUpToSdCard").toString();
    takeFromSdCard_String=LanguageFile->readVariables("takeFromSdCard").toString();
    updateNow_String=LanguageFile->readVariables("updateNow").toString();
    printReport_String=LanguageFile->readVariables("printReport").toString();
    serviceOpen_String=LanguageFile->readVariables("serviceOpen").toString();
    serviceClosed_String=LanguageFile->readVariables("serviceClosed").toString();
    keyboardClear_String=LanguageFile->readVariables("keyboardClear").toString();
    keyboardSpace_String=LanguageFile->readVariables("keyboardSpace").toString();
    keyboardBack_String=LanguageFile->readVariables("keyboardBack").toString();
    keyboardEnter_String=LanguageFile->readVariables("keyboardEnter").toString();
    taken_String=LanguageFile->readVariables("taken").toString();
    invalidNozzle_String=LanguageFile->readVariables("invalidNozzle").toString();

    enterValidDate_String=LanguageFile->readVariables("enterValidDate").toString();
    date_String=LanguageFile->readVariables("date").toString();
    time_String=LanguageFile->readVariables("time").toString();
    setDateTime_String=LanguageFile->readVariables("setDateTime").toString();
    incorrectTime_String=LanguageFile->readVariables("incorrectTime").toString();
    incorrectDate_String=LanguageFile->readVariables("incorrectDate").toString();
    systemStarting_String=LanguageFile->readVariables("systemStarting").toString();
    dateSetting_String=LanguageFile->readVariables("dateSetting").toString();
    networkSettings_String=LanguageFile->readVariables("networkSettings").toString();
    dateFormat_String=LanguageFile->readVariables("dateFormat").toString();

    idChecking_String=LanguageFile->readVariables("idChecking").toString();
    waitingPayment_String=LanguageFile->readVariables("waitingPayment").toString();
    inadequateLimit_String=LanguageFile->readVariables("inadequateLimit").toString();
    unregisteredPlate_String=LanguageFile->readVariables("unregisteredPlate").toString();
    waitingFilling_String=LanguageFile->readVariables("waitingFilling").toString();
    filling_String=LanguageFile->readVariables("filling").toString();
    wrongId_String=LanguageFile->readVariables("wrongId").toString();
    wrongProduct_String=LanguageFile->readVariables("wrongProduct").toString();
    passiveId_String=LanguageFile->readVariables("passiveId").toString();
    deletedId_String=LanguageFile->readVariables("deletedId").toString();
    fillingWasStopped_String=LanguageFile->readVariables("fillingWasStopp").toString();
    enterPassword_String=LanguageFile->readVariables("enterPassword").toString();
    password_String=LanguageFile->readVariables("password").toString();
    fieldModeActive_String=LanguageFile->readVariables("fieldModeActive").toString();
    enterUnitPrice_String=LanguageFile->readVariables("enterUnitPrice").toString();
    newUnitPrice_String=LanguageFile->readVariables("newUnitPrice").toString();
    unitPrice_String=LanguageFile->readVariables("unitPrice").toString();
    updated_String=LanguageFile->readVariables("updated").toString();
    startingSystem_String=LanguageFile->readVariables("startingSystem").toString();
    printerPaperOut_String=LanguageFile->readVariables("printerPaperOut").toString();
    automationError_String=LanguageFile->readVariables("automationError").toString();

    waiting_String=LanguageFile->readVariables("waiting").toString();
    kmControle_String=LanguageFile->readVariables("kmControle").toString();
    pleaseEnterKm_String=LanguageFile->readVariables("pleaseEnterKm").toString();
    freePlate_String=LanguageFile->readVariables("freePlate").toString();
    saleControl_String=LanguageFile->readVariables("saleControl").toString();
    plateSale_String=LanguageFile->readVariables("plateSale").toString();
    ipAdress_String=LanguageFile->readVariables("ipAdress").toString();
    macAdress_String=LanguageFile->readVariables("macAdress").toString();
    version_String=LanguageFile->readVariables("version").toString();

    idTaken_ByteArray=LanguageFile->readVariables("idTaken").toByteArray();
    pleaseReadAttendantCard_ByteArray=LanguageFile->readVariables("pleaseReadAttendantCard").toByteArray();
    checkingPlate_ByteArray=LanguageFile->readVariables("checkingPlate").toByteArray();
    pleaseTakeANozzleAndReadAttendantCard_ByteArray=LanguageFile->readVariables("pleaseTakeANozzleAndReadAttendantCard").toByteArray();
    idControl_ByteArray=LanguageFile->readVariables("idControl").toByteArray();
    checkingCard_ByteArray=LanguageFile->readVariables("checkingCard").toByteArray();
    pleaseTakeANozzle_ByteArray=LanguageFile->readVariables("pleaseTakeANozzle").toByteArray();
    incorrectOperation_ByteArray=LanguageFile->readVariables("incorrectOperation").toByteArray();
    idError_ByteArray=LanguageFile->readVariables("idError").toByteArray();
    fillingFailReadTag_ByteArray=LanguageFile->readVariables("fillingFailReadTag").toByteArray();
    acceptPopup_String=LanguageFile->readVariables("acceptPopup").toString();
    cancelPopup_String=LanguageFile->readVariables("cancelPopup").toString();
    textPopup_String=LanguageFile->readVariables("textPopup").toString();
    system("sync");
}

QString getMacAddress()
{
    
    QNetworkInterface eth0=QNetworkInterface::interfaceFromName("eth0");
    QList<QNetworkAddressEntry>entries=eth0.addressEntries();
    QNetworkAddressEntry entry=entries.first();
    QHostAddress ip=entry.ip();
    if(ip.toString().contains('.'))
        Static_ip=ip.toString();
    else Static_ip="OFFLINE";
    macAddress=eth0.hardwareAddress();
    if(!entries.isEmpty()){

        return eth0.hardwareAddress();
    }

}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOverrideCursor(Qt::BlankCursor);
    //    if(settings.value("change_file").toInt()!=1)
    //    {
    //        convert_bytearray_to_string();
    //        convert_setting();
    //    }

    loadLanguage();
    system("chmod 777 /var/www/html/*");


    constructor_db.setDatabaseName("/var/www/html/Upc_Database");
    get_price_db.setDatabaseName("/var/www/html/Upc_Database");
    nozzle_db.setDatabaseName("/var/www/html/Upc_Database");

    count_id_db.setDatabaseName("/var/www/html/Upc_Database");

    update_nozzle_price_db.setDatabaseName("/var/www/html/Upc_Database");
    update_nozzle_product_db.setDatabaseName("/var/www/html/Upc_Database");
    update_nozzle_total_db.setDatabaseName("/var/www/html/Upc_Database");

    nozzle_product_id_db.setDatabaseName("/var/www/html/Upc_Database");
    nozzle_values_db.setDatabaseName("/var/www/html/Upc_Database");
    nozzle_product_price_db.setDatabaseName("/var/www/html/Upc_Database");
    nozzle_total_amount_db.setDatabaseName("/var/www/html/Upc_Database");
    nozzle_total_volume_db.setDatabaseName("/var/www/html/Upc_Database");
    nozzle_total_sales_db.setDatabaseName("/var/www/html/Upc_Database");

    insert_sales_db.setDatabaseName("/var/www/html/Upc_Database");
    insert_probe_db.setDatabaseName("/var/www/html/Upc_Database");
    update_probe_db.setDatabaseName("/var/www/html/Upc_Database");
    delete_table_db.setDatabaseName("/var/www/html/Upc_Database");
    sales_date_db.setDatabaseName("/var/www/html/Upc_Database");
    sales_status_db.setDatabaseName("/var/www/html/Upc_Database");
    update_status_db.setDatabaseName("/var/www/html/Upc_Database");
    update_dateclock_db.setDatabaseName("/var/www/html/Upc_Database");
    update_sales_db.setDatabaseName("/var/www/html/Upc_Database");
    update_plate_db.setDatabaseName("/var/www/html/Upc_Database");
    total_sales_db.setDatabaseName("/var/www/html/Upc_Database");
    sales_plate_db.setDatabaseName("/var/www/html/Upc_Database");
    sales_id_db.setDatabaseName("/var/www/html/Upc_Database");
    controlsales_id_db.setDatabaseName("/var/www/html/Upc_Database");
    whitelist_db.setDatabaseName("/var/www/html/Upc_Database");
    plates_db.setDatabaseName("/var/www/html/Upc_Database");

    person_db.setDatabaseName("/var/www/html/Upc_Database");
    control_person_db.setDatabaseName("/var/www/html/Upc_Database");
    show_person_db.setDatabaseName("/var/www/html/Upc_Database");
    show_nozzle_db.setDatabaseName("/var/www/html/Upc_Database");
    show_whitelist_db.setDatabaseName("/var/www/html/Upc_Database");
    show_sales_db.setDatabaseName("/var/www/html/Upc_Database");
    tankLevel_db.setDatabaseName("/var/www/html/Upc_Database");



    probe_count=0;

    if(database->SettingsValues())
    {
        qDebug()<<"Config_values_Read_setting";
        settings.setlog(" LOAD_SETTING Config_values_Read_setting");
        setting_configuration=true;
    }
    else
    {
        qDebug()<<"Config_values_settings_error";
        settings.setlog(" LOAD_SETTING Config_values_settings_error");
        setting_configuration=false;
    }
    if(database->NozzleValues())
    {
        qDebug()<<"Config_values_Read_nozzle";
        settings.setlog(" LOAD_SETTING Config_values_Read_nozzle");
        nozzle_configuration=true;
    }
    else
    {
        qDebug()<<"Config_values_nozzle_error";
        settings.setlog(" LOAD_SETTING Config_values_nozzle_error");
        nozzle_configuration=false;
    }
    if(database->ProbeValues())
    {
        qDebug()<<"Config_values_Read_probe";
        settings.setlog(" LOAD_SETTING Config_values_Read_probe");
    }
    else
    {
        qDebug()<<"Config_values_probe_error";
        settings.setlog(" LOAD_SETTING Config_values_probe_error");
    }

    if(nozzle_configuration==true && setting_configuration==true)
    {
        config_error=false;
        setting_nozzle_number=settings.value("setting_nozzle_number").toInt();
        if(setting_nozzle_number==0) setting_nozzle_number=1;
        settings.setPriceStatus(pump[pumpNo].nozzle[setting_nozzle_number].unitprice.toHex(),0);
    }
    else
    {
        config_error=true;
        QDir dir("/media/root/");
        QFileInfoList list_usb=dir.entryInfoList();
        for(int i=0;i<list_usb.size();i++)
        {
            QString file_name=list_usb.at(i).fileName();
            if(file_name=="MEPSAN")
            {
                system("cp -f /media/root/MEPSAN/ Upc_Database /var/www/html/Upc_Database");
                system("cp -f /media/root/MEPSAN/setting_save.txt /var/www/html/setting_save.txt");
                break;
            }
        }
        sync();
        system("sync");

        if(database->SettingsValues())
        {
            qDebug()<<"Config_values_Read_setting";
            settings.setlog(" LOAD_SETTING Config_values_Read_setting");
            setting_configuration=true;
        }
        else
        {
            qDebug()<<"Config_values_settings_error";
            settings.setlog(" LOAD_SETTING Config_values_settings_error");
            setting_configuration=false;
        }
        if(database->NozzleValues())
        {
            qDebug()<<"Config_values_Read_nozzle";
            settings.setlog(" LOAD_SETTING Config_values_Read_nozzle");
            nozzle_configuration=true;
        }
        else
        {
            qDebug()<<"Config_values_nozzle_error";
            settings.setlog(" LOAD_SETTING Config_values_nozzle_error");
            nozzle_configuration=false;
        }
        if(nozzle_configuration==true && setting_configuration==true)
        {
            config_error=false;
            settings.setPriceStatus(pump[pumpNo].nozzle[1].unitprice.toHex(),0);
        }
    }

    QCryptographicHash calculateMD5(QCryptographicHash::Sha256);
    calculateMD5.addData(getMacAddress().toUtf8());
    licence=settings.value("Licence").toString();

    if(calculateMD5.result().toHex()!=licence)
    {
        QDir dir("/media/root/");
        QFileInfoList list_usb=dir.entryInfoList();
        for(int i=0;i<list_usb.size();i++)
        {
            QString file_name=list_usb.at(i).fileName();
            if(file_name=="MEPSAN")
            {
                system("cp -f /media/root/MEPSAN/Upc_Database /var/www/html/Upc_Database");
                system("cp -f /media/root/MEPSAN/setting_save.txt /var/www/html/setting_save.txt");
                break;
            }
        }
        sync();
        system("sync");
        licence=settings.value("Licence").toString();
    }

    loadSettings();

    qDebug()<<"selammmmmmmmmmmmmmmmmmmmmmmmmm";

    QFile file("/var/www/html/probe_info.txt");
    file.remove();
    for(int i=0;i<probe_count;i++)
    {
        settings.setProbeINfo(QString::fromUtf8(Probe[i].adres.toHex()).toUpper(),"0","-","-","-",Probe[i].minLevel,Probe[i].maxLevel,Probe[i].minTemperature,Probe[i].maxTemperature,Probe[i].tankSize,"0",Probe[i].probe_maxWaterLevel,Probe[i].probe_minWaterLevel);
    }


    LCDtimer1=new QTimer;
    cusscr= new CustomerScreen();
    LCDThread=new QThread();
    QObject::connect(LCDtimer1,SIGNAL(timeout()),cusscr,SLOT(LCD_refresh()));
    LCDtimer1->start(300);
    cusscr->moveToThread(LCDThread);
    LCDThread->start();
    MainWindow w;
    w.show();


    database->insertToStatus();
    qDebug()<<"selammm";
    int currentYear = QDateTime::currentDateTime().toString("yyyy").toInt();

    if(currentYear > 2017)
    {
        MACaddr=calculateMD5.result().toHex();
        LCD_status=1;
        //        if(calculateMD5.result().toHex()==licence)
        //        {
        //CLRC
        QThread clrcThread;
        CLRC clrc;
        clrc.I2CStartUp();
        clrc.clrctoThread(clrcThread);
        clrc.moveToThread(&clrcThread);
        clrcThread.start();

        //main_routine
        QThread mainThread;
        main_routine mt;
        mt.maintoThread(mainThread);
        mt.moveToThread(&mainThread);
        mainThread.start();

        initserial(1); //printer

        initserial(2); //Pompa

        initserial(3); //Acc.Point

        if(probe_count>0)
            initserial(4); //Probe

        if(upc_locvis==false)
        {
            QThread TcpThread;
            MyTcpSocket mts;
            mts.TcptoThread(TcpThread);
            mts.moveToThread(&TcpThread);
            TcpThread.start();
        }



        licenceCheck=0;

        return a.exec();

        //        }
        //        else
        //        {
        //            if(test)qDebug()<<"Lisans Hatası";
        //            settings.setlog(" LOAD_SETTING Lisans Hatası");
        //            LCD_status=13;
        //            LCD_refresh_flg=1;
        //            licenceCheck=1;


        //            return a.exec();
        //        }
    }
    else
    {
        if(test) qDebug()<<"Saat-Tarih Hatası";
        settings.setlog(" LOAD_SETTING Saat-Tarih Hatası");
        LCD_status=35;
        LCD_refresh_flg=1;
        return a.exec();
    }

}
