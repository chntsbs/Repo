#include "main_routine.h"

#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <QCoreApplication>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QDebug>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <QTimer>
#include <QThread>
#include"global.h"
#include"scproutine.h"
#include"syslog.h"

int plate_error_count;

main_routine::main_routine(QObject *parent) :
    QObject(parent)
{
    maintimer1=new QTimer(this);
    maintimer2=new QTimer(this);
    buzzertimer=new QTimer;
    database=new Database();
    Sales_start=0;
    Confirmation_flg=0;
    LCD_status=1;
    LCD_refresh_flg=1;
    ID_count=0;
}

void main_routine::scan_timer()
{
    maintimer1->start(1000);
    maintimer2->start(50000);
}

void main_routine::maintoThread(QThread &mainThread)
{
    connect(maintimer1,SIGNAL(timeout()),this,SLOT(main_func()));
    connect(maintimer2,SIGNAL(timeout()),this,SLOT(clock()));
    connect(&mainThread,SIGNAL(started()),this,SLOT(scan_timer()));
    connect(buzzertimer,SIGNAL(timeout()),this,SLOT(mainbuzzerOff()));
}

void main_routine::convert_check_RFID()
{
    //qDebug()<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP<<"bu ne";
    if(test)qDebug()<<"convert_check_RFID-ftyp"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP;
    settings.setlog("convert_check_RFID-ftyp "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP));
    if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP>=1) && (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP<=4))
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=1;
    }
    else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==20)  //TURPAK TTS
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=1;
    }
    else if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP>=7) && (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP<=10))
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=2;
    }
    else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==14)
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=9;
    }
    else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==15)
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=19;
    }
    else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==13)
    {
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP=0;
    }
}

void main_routine::mainbuzzerOn()
{
    if(test)qDebug()<<"BuzzerONN";
    system("echo 1 > /sys/class/gpio/gpio141/value");
    buzzertimer->start(100);
}
void main_routine::mainbuzzerOff()
{
    if(test)qDebug()<<"BuzzerOFF";
    system("echo 0 > /sys/class/gpio/gpio141/value");
    buzzertimer->stop();
}


void main_routine::main_func()
{
    if(QDate::currentDate().toJulianDay()-updatedDate.toJulianDay()>0){
        if((QDateTime::currentDateTime().toString("HHmmss")>="141355" && QDateTime::currentDateTime().toString("HHmmss")<="141359") || immediatelyUpdateBalanceLimit==2){
            if(Pump_Filling_status!=1){
                qDebug()<<"HOBAAAA";
            database->updateAllBalanceLimit();
            settings.setValue("UpdateBalanceLimit",QString::fromStdString(CryptoObj.encrypt(QDateTime::currentDateTime().toString("ddMMyyyy").toStdString())));
            immediatelyUpdateBalanceLimit=0;
            }
            else{
                qDebug()<<"FILLINGGGGGGGGGGGG BALANCE";
                immediatelyUpdateBalanceLimit=1;
            }

        }
    }

    if(upc_locvis) //LOCWIZ
    {
        if(meter_com_err==false)
        {
            if((USC_is_exist==false) && (config_error==false) && (End_total_report==false) && (reboot_ok!=true))  //USC yoksa
            {
                Confirmation_flg_without_km=false;
                if(((Card_read_flg==1) || (TAG_read==true)) && (Person_wait_flg==false) && (Driver_wait_flg==false)&&(Confirmation_flg_ok!=true)&&(LCD_status!=11)) //Card_or_TAG_read_flg
                {
                    if(TAG_read==true)
                    {
                        CustomerID=TAGID[DCR_par.DCR_Nozzle_Number].toHex(); //DCR_par.DCR_Nozzle_Number  => aktif nozzle
                        settings.setlog("SATIS_DEBUG TAG CustomerID  "+CustomerID);
                        if(test)qDebug()<<"TAG CustomerID" <<CustomerID;
                    }
                    if(database->kontrolSalesRfid(CustomerID))  //ilk kart okutuldu sorguya gitti
                    { customerCardLock=true;

                        qDebug()<<"Pumper Flag"<<pumper_flg;
                        if(plate_entered_with_button==true)
                        {
                            Card_read_flg=0;
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0;// ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-AFTER-KEYPAD-PLATE-ID-KAYIT BULUNAMADI "+CustomerID);
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;
                        }
                        else
                        {
                            convert_check_RFID();
                            if(test)qDebug()<<"CARD Filling-Auth-type"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP;
                            settings.setlog("SATIS_DEBUG USC-FALSE-SALE-TYPE-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                            if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1) || (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==2)) //müşteri(TTS-MTS) kartı okutuduysa
                            {
                                qDebug()<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP<<"CİHANGİRR";
                                switch (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP) {
                                case 1://tekli müşteri authorize
                                {
                                    customerCardRead=true;
                                    customerCard=true;
                                    settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    Sales_start=0;
                                    Enter_flg=0;
                                    if((TAG_read==false) &&(Card_read_flg==1))
                                    {
                                        card_authorize=true;
                                    }
                                    TAG_read=false;
                                    Card_read_flg=0;
                                    if(ask_km==true)
                                    {
                                        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1)
                                        {
                                            if(odometre_active==true)
                                            {
                                                pump[pumpNo].KM=odometre_KM;
                                                pump[pumpNo].runningTime=odometre_time;

                                                Confirmation_flg=1;
                                                ask_nozzle_total_volume=true;
                                                Confirmation_flg_ok=true;
                                                Sale_Error_code=2;
                                                Update_flg=true;
                                                LCD_refresh_flg=1;
                                                if(test)qDebug()<<"SATIS_DEBUG USC-FALSE-TEKLI-AUTHORIZE";
                                                settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-AUTHORIZE");
                                            }
                                            else
                                            {
                                                TAG_KM_read=true;
                                                LCD_status=11; // km BİLGİSİ
                                                if(test)qDebug()<<"SATIS_DEBUG USC-FALSE-TEKLI-KM GIR EKRANI";
                                                settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-KM GIR EKRANI");
                                            }
                                        }
                                        else
                                        {
                                            TAG_KM_read=true;
                                            LCD_status=11; // km BİLGİSİ
                                            if(test)qDebug()<<"SATIS_DEBUG USC-FALSE-TEKLI-KM GIR EKRANI";
                                            settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-KM GIR EKRANI");
                                        }
                                    }
                                    else
                                    {
                                        Confirmation_flg=1;
                                        ask_nozzle_total_volume=true;
                                        Confirmation_flg_ok=true;
                                        Sale_Error_code=2;
                                        Update_flg=true;
                                        LCD_refresh_flg=1;
                                        if(test)qDebug()<<"SATIS_DEBUG USC-FALSE-TEKLI-AUTHORIZE";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-AUTHORIZE");
                                    }
                                }
                                    break;
                                case 2:// müşteri + pompacı  authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-CIFTLI-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+" "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    TEXT_fromMaster=pleaseReadAttendantCard_ByteArray;
                                    settings.setlog("SATIS_DEBUG USC-FALSE-POMPACI Okutunuz");
                                    LCD_refresh_flg=1;
                                    Person_wait_flg=true;
                                    if((TAG_read==false) &&(Card_read_flg==1))
                                    {
                                        card_authorize=true;
                                    }
                                    TAG_read=false;
                                    Card_read_flg=0;
                                }
                                    break;
                                case 3:// müşteri + şöför + pompacı  authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-3LU-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    if(auttype_3_4==true)
                                    {
                                        TEXT_fromMaster=pleaseReadDriverCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-SOFOR Okutunuz");
                                        LCD_refresh_flg=1;
                                        Driver_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }
                                    else
                                    {
                                        TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-POMPACI Okutunuz");
                                        LCD_refresh_flg=1;
                                        Person_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }
                                }
                                    break;
                                case 4:// müşteri + şöför authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-4-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    if(auttype_3_4==true) /*???????????????????????????*/
                                    {
                                        TEXT_fromMaster=pleaseReadDriverCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-SOFOR Okutunuz");
                                        LCD_refresh_flg=1;
                                        Driver_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }
                                    else
                                    {
                                        TEXT_fromMaster=pleaseReadAttendantCard_ByteArray;
                                        settings.setlog("SATIS_DEBUG USC-FALSE-POMPACI Okutunuz");
                                        LCD_refresh_flg=1;
                                        Person_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }

                                }
                                    break;
                                default:
                                    break;
                                }
                            }
                            else //müşteri harici okutulan kart
                            {
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP!=13) //ilk şöför okutulmadı
                                {

                                    if(ask_km==true)
                                    {
                                        TAG_KM_read=true;
                                        LCD_status=11; // km BİLGİSİ
                                        if(test)qDebug()<<"KM GIR EKRANI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-OTHERS-KM GIR EKRANI");
                                    }
                                    else
                                    {
                                        Confirmation_flg=1;
                                        ask_nozzle_total_volume=true;
                                        Confirmation_flg_ok=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                        Sale_Error_code=2;
                                        Update_flg=true;
                                        if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-OTHERS-SATIS AUTHORIZE GONDER");
                                    }
                                }
                                else   //ilk şöför kartı okutulduysa izin yok
                                {
                                    Card_read_flg=0;
                                    TAG_read=false;
                                    LCD_status=9; // kayıt bulumadı lcd
                                    Allow_card_read=0; //ANIL
                                    LCD_refresh_flg=1;
                                    if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                    settings.setlog("SATIS_DEBUG USC-FALSE-ILK-SOFOR-OKUTULDU-HATA");
                                    Temp_ID[0].clear();
                                    Temp_ID[1].clear();
                                    Temp_ID[2].clear();
                                    tag_read_count=0;
                                    Tag_read_count_flg=0;
                                    Person_is_Ok=false;

                                }
                            }
                        }
                    }
                    else ///pompacı kontrol
                    {

                        settings.setlog("SATIS_DEBUG USC-FALSE-ILK-SOFOR-OKUTULDU-HATA");
                        if(TAG_read!=true)
                        {
                            if(database->kontrolPersonRfid(CustomerID))
                            {
                                pumper_flg=true;
                                qDebug()<<"pompacı kartı okutuldu"<<plate_entered_with_button;
                                if(plate_entered_with_button==true)
                                {
                                    qDebug()<<pump[pumpNo].PLATE<<"plate?"<<database->kontrolSalesPlate(pump[pumpNo].PLATE);

                                    if(database->kontrolSalesPlate(pump[pumpNo].PLATE))  //önceden plaka girildiğinde nozzle a değerler yüklensin diye
                                    {

                                        qDebug()<<"pompacı okutulduuu";
                                        convert_check_RFID();
                                        if(test)qDebug()<<"plate_entered_with_button AUTHORIZE";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP));
                                        if(ask_km==true)
                                        {
                                            LCD_status=11; // KM GİRİS
                                            LCD_refresh_flg=1;
                                            if(test)qDebug()<<"KM GIRIS EKRANI";
                                            settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button KM GIRIS EKRANI ");
                                        }
                                        else
                                        {
                                            qDebug()<<"selam";
                                            Sales_start=0;
                                            plate_entered_with_button=false;
                                            Enter_flg=0;
                                            if((TAG_read==false) &&(Card_read_flg==1))
                                            {card_authorize=true;}
                                            TAG_read=false;
                                            Card_read_flg=0;
                                            qDebug()<<"pumpergf";

                                            if(askSellingStyle==true && ask_km==true){
                                            StoredScreenRFID_fromMaster=2; //person
                                            }
//                                            if(askSellingStyle==false && ask_km==true){
//                                            StoredScreenRFID_fromMaster=2; //person
//                                            }
                                            Confirmation_flg=1;
                                            ask_nozzle_total_volume=true;
                                            Confirmation_flg_ok=true;
                                            KM_Confirmation_flg=true;
                                            Sale_Error_code=2;
                                            Update_flg=true;
                                            LCD_refresh_flg=1;
                                            if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                            settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button SATIS AUTHORIZE GONDER ");
                                        }
                                    }
                                    else
                                    {

                                        if(askSellingStyle==true){
                                            if(ask_km==true){
                                            LCD_refresh_flg=1;
                                            LCD_status=11;
                                            qDebug()<<"kotrol plaka yanıs";
                                            } else {
                                                if(USC_is_exist==false)
                                                {
                                                    qDebug()<<"buradayken575";
                                                    //StoredScreenRFID_fromMaster=4; //vehicle
                                                    TAG_KM_read=false;//km ekranına gitmezse satış başlar
                                                    Confirmation_flg=1;
                                                    ask_nozzle_total_volume=true;
                                                    KM_Confirmation_flg=true;
                                                    Sale_Error_code=2;
                                                    Update_flg=true;
                                                    LCD_refresh_flg=1;
                                                    plate_entered_with_button=false;
                                                    TAG_read=false;
                                                    Card_read_flg=0;
                                                    if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                                    settings.setlog("SATIS_DEBUG AFTER_KM SATIS AUTHORIZE GONDER");
                                                    Confirmation_flg_ok=true;
                                                }
                                                else
                                                {
                                                    Confirmation_flg_without_km=false;  ////sonradan
                                                    KM_Confirmation_flg=true;
                                                    Confirmation_flg_ok=true;
                                                }
                                            }



                                        } else {

                                        Card_read_flg=0;
                                        plate_entered_with_button=false;
                                        qDebug()<<"maybe";
                                        LCD_status=6; // kayıt bulumadı lcd
                                        LCD_refresh_flg=1;
                                        if(test)qDebug()<<"PLAKA ÖNCE DATABASE HATASI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button PLAKA BULUNAMADI ");
                                        Temp_ID[0].clear();
                                        Temp_ID[1].clear();
                                        Temp_ID[2].clear();
                                        tag_read_count=0;
                                        Tag_read_count_flg=0;
                                        Person_is_Ok=false;
                                        }
                                    }


                                }
                                else
                                {
                                    if((TAG_read==false) &&(Card_read_flg==1))
                                    {card_authorize=true;}
                                    if(test)qDebug()<<"Plaka Giriniz ekranı_1"<<card_authorize;
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Plaka Giriniz ekranı_1 ");
                                    LCD_status=2; // Plaka Giriniz ekranı

                                    LCD_refresh_flg=1;

                                }
                            }
                            else
                            {
                                pumper_flg=false;
                                if(wrong_product==false)
                                {
                                    qDebug()<<"kotrol plaka yaanluş";
                                    Card_read_flg=0;
                                    LCD_status=9; // kayıt bulumadı lcd
                                    Allow_card_read=0; //ANIL
                                    LCD_refresh_flg=1;
                                    if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-1 ");
                                    Temp_ID[0].clear();
                                    Temp_ID[1].clear();
                                    Temp_ID[2].clear();
                                    tag_read_count=0;
                                    Tag_read_count_flg=0;
                                    Person_is_Ok=false;
                                }
                                else
                                {
                                    Card_read_flg=0;
                                    LCD_status=9; // kayıt bulumadı lcd
                                    Allow_card_read=0; //ANIL
                                    LCD_refresh_flg=1;
                                    if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-1 ");
                                    Temp_ID[0].clear();
                                    Temp_ID[1].clear();
                                    Temp_ID[2].clear();
                                    tag_read_count=0;
                                    Tag_read_count_flg=0;
                                    Person_is_Ok=false;
                                }
                            }
                        }
                        else
                        {
                            if(wrong_product==false)
                            {
                                qDebug()<<"kotrol plaka yaanllış";
                                Card_read_flg=0;
                                LCD_status=9; // kayıt bulumadı lcd
                                Allow_card_read=0; //ANIL
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-1 ");
                                Temp_ID[0].clear();
                                Temp_ID[1].clear();
                                Temp_ID[2].clear();
                                tag_read_count=0;
                                Tag_read_count_flg=0;
                                Person_is_Ok=false;
                            }
                            else
                            {qDebug()<<"kotrol plaka dou";
                                Card_read_flg=0;
                                LCD_status=9; // kayıt bulumadı lcd
                                Allow_card_read=0; //ANIL
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-1 ");
                                Temp_ID[0].clear();
                                Temp_ID[1].clear();
                                Temp_ID[2].clear();
                                tag_read_count=0;
                                Tag_read_count_flg=0;
                                Person_is_Ok=false;
                            }
                        }

                    }

                }

                //////POMPACI SORGU
                if(Card_read_flg==1)
                {qDebug()<<"buraya geliyor1";
                    if(Person_wait_flg==true) // customer + pompacı için pompacı bekleniyor
                    {
                        Person_wait_flg=false;
                        Card_read_flg=0;
                        settings.setlog("SATIS_DEBUG USC-FALSE-customer + pompacı Icin Pompacı Kontrol "+CustomerID);
                        if(database->kontrolPersonRfid(CustomerID)) //pompacı database control
                        {
                            //pumper_flg=true;
                            qDebug()<<"pumpertr";
                            StoredScreenRFID_fromMaster=2;
                            Card_read_flg=0;

                            if(ask_km==true)
                            {
                                TAG_KM_read=true;
                                LCD_status=11; // km BİLGİSİ
                                if(test)qDebug()<<"KM GIR EKRANI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Person-wait KM GIR EKRANI");
                            }
                            else
                            {
                                Confirmation_flg=1;
                                ask_nozzle_total_volume=true;
                                Confirmation_flg_ok=true;
                                Sale_Error_code=2;
                                Update_flg=true;
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Person-wait SATIS AUTHORIZE GONDER");
                            }

                        }
                        else
                        {
                            //pumper_flg=false;
                            qDebug()<<"kotrol plaka dogru5";
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0; //ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Person-wait KAYIT BULUNAMADI");
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;
                        }

                    }
                }

                //////ŞÖFÖR SORGU
                if(Card_read_flg==1)
                {//qDebug()<<"kotrol";
                    if(Driver_wait_flg==true) // customer + Şöför +pompacı+ için şöför bekleniyor
                    {
                        Driver_wait_flg=false;
                        Card_read_flg=0;
                        settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait Sofor Kontrol "+CustomerID);
                        if(database->kontrolSalesRfid(CustomerID)) //ŞÖFÖR database control
                        {
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP==13)  //ŞÖFÖR varsa
                            {
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==3)
                                {
                                    TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait Pompacı Okutunuz");
                                    LCD_refresh_flg=1;
                                    Person_wait_flg=true;
                                    TAG_read=false;
                                }
                                else //Customer + Şöför  FTYP ==4
                                {
                                    Card_read_flg=0;
                                    if(ask_km==true)
                                    {
                                        TAG_KM_read=true;
                                        LCD_status=11; // km BİLGİSİ
                                        if(test)qDebug()<<"KM GIR EKRANI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait KM GIR EKRANI");
                                    }
                                    else
                                    {qDebug()<<"kotrol plaka dogr5u";
                                        Confirmation_flg=1;
                                        ask_nozzle_total_volume=true;
                                        Confirmation_flg_ok=true;
                                        Sale_Error_code=2;
                                        Update_flg=true;
                                        LCD_refresh_flg=1;
                                        if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait SATIS AUTHORIZE GONDER");
                                    }
                                }
                            }
                            else
                            {qDebug()<<"kotrol plaka dogru68";
                                LCD_status=9; // kayıt bulumadı lcd
                                Allow_card_read=0; //ANIL
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait KAYIT BULUNAMADI-1");
                                Temp_ID[0].clear();
                                Temp_ID[1].clear();
                                Temp_ID[2].clear();
                                tag_read_count=0;
                                Tag_read_count_flg=0;
                                Person_is_Ok=false;
                            }

                        }
                        else
                        {qDebug()<<"kotrol 547plaka dogru";
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0; //ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait KAYIT BULUNAMADI-2");
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;
                        }

                    }
                }


                if(Plate_control_flg==1)
                {
                    Plate_control_flg=0;
                    Enter_flg=0;
                    qDebug()<<"here3";
                    if(askSellingStyle==true){
                        TEXT_fromMaster="        ";
                    } else {
                    TEXT_fromMaster=checkingPlate_ByteArray;
                    }
                    LCD_status=1; //plaka kontrolü yapılıyor
                    LCD_refresh_flg=1;
                    if(test)qDebug()<<"plaka kontrolü yapılıyor"<<QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE);
                    settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg PLAKA KONTROLU "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex()));
                    Person_is_Ok=true;
                    QString temp_plate;
                    temp_plate=QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE);
                    if(plate_entered_with_button==true)
                    {   qDebug()<<"fder";
                        settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg plate_entered_with_button "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex()));
                        temp_plate=pump[pumpNo].PLATE;
                        if(Nozzle_up_flg==1)
                        {
                            TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg plate_entered_with_button POMPACI OKUTUNUZ");
                        }
                        else
                        {
                            qDebug()<<"fdere";
                            TEXT_fromMaster=pleaseTakeANozzleAndReadAttendantCard_ByteArray;
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg plate_entered_with_button TABANCA ALINIZ POMPACI OKUTUNUZ");
                        }
                        LCD_status=1; //ana ekrana dön
                        LCD_refresh_flg=1;
                    }
                    else
                    {
                        if(test)qDebug()<<"temp_plate"<<temp_plate;
                        settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg temp_plate "+temp_plate);
                        if(database->kontrolSalesPlate(temp_plate))
                        {
                            convert_check_RFID();
                            if(test)qDebug()<<"PLAKA KAYIT VAR";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg PLAKA KAYIT VAR");
                            if(ask_km==true)
                            {
                                LCD_status=11; // KM GİRİS
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KM GIRIS EKRANI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg KM GIRIS EKRANI");
                            }
                            else
                            {
                                Sales_start=0;
                                Enter_flg=0;
                                qDebug()<<"pumperol";
                                if(askSellingStyle==true && ask_km==true){
                                StoredScreenRFID_fromMaster=2; //person
                                }

                                Confirmation_flg=1;
                                ask_nozzle_total_volume=true;
                                Confirmation_flg_ok=true;
                                KM_Confirmation_flg=true;
                                Sale_Error_code=2;
                                Update_flg=true;
                                LCD_status=1; //ana ekrana dön
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg SATIS AUTHORIZE GONDER");
                            }


                        }
                        else
                        {
                            if(wrong_product==false)
                            {
                                if(askSellingStyle==true){
                                    if(ask_km==true){
                                    LCD_refresh_flg=1;
                                    TAG_KM_read=true;
                                    LCD_status=11;
                                    qDebug()<<"kotrol plaka8 dogru";
                                    } else {
                                        if(USC_is_exist==false)
                                        {
                                            qDebug()<<"pumperkhj";
                                            StoredScreenRFID_fromMaster=2; //vehicle
                                            TAG_KM_read=false;//km ekranına gitmezse satış başlar
                                            Confirmation_flg=1;
                                            ask_nozzle_total_volume=true;
                                            KM_Confirmation_flg=true;
                                            Sale_Error_code=2;
                                            Update_flg=true;
                                            LCD_refresh_flg=1;
                                            plate_entered_with_button=false;
                                            TAG_read=false;
                                            Card_read_flg=0;
                                            if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                            settings.setlog("SATIS_DEBUG AFTER_KM SATIS AUTHORIZE GONDER");
                                            Confirmation_flg_ok=true;
                                        }
                                        else
                                        {
                                            Confirmation_flg_without_km=false;  ////sonradan
                                            KM_Confirmation_flg=true;
                                            Confirmation_flg_ok=true;
                                        }
                                    }


                                }
                                else {
                                    CustomerID=QString::fromUtf8(Customer_plate);
                                    LCD_refresh_flg=1;
                                    LCD_status=6; // PLAKA KAYIT BULUNAMADI
                                    TAG_read=false;
                                    Card_read_flg=0;
                                    if(test)qDebug()<<"PLAKA KAYIT YOK";
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg PLAKA KAYIT YOK");
                                    Temp_ID[0].clear();
                                    Temp_ID[1].clear();
                                    Temp_ID[2].clear();
                                    Customer_plate.clear();
                                    Person_is_Ok=false;
                                }
                            }
                            else
                            {   qDebug()<<"kotrol plaka dogrupş";
                                Card_read_flg=0;
                                LCD_status=9; // kayıt bulumadı lcd
                                Allow_card_read=0; //ANIL
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-1 ");
                                Temp_ID[0].clear();
                                Temp_ID[1].clear();
                                Temp_ID[2].clear();
                                tag_read_count=0;
                                Tag_read_count_flg=0;
                                Person_is_Ok=false;
                                Customer_plate.clear();
                            }

                        }
                    }
                }

                if((Plate_screen_flg==true) && (Confirmation_flg!=1) && (Plate_control_flg!=1))
                {
                    Plate_screen_flg=false;
                    //POMPACI KARTI PLAKA SORGULA
                    if(test)qDebug()<<"Plaka Giriniz ekranı_2";
                    settings.setlog("SATIS_DEBUG USC-FALSE-Plate_screen_flg Plaka Giriniz");
                    LCD_status=2; // Plaka Giriniz ekranı
                    LCD_refresh_flg=1;
                    plate_entered_with_button=true;

                }
                if((Person_is_Ok==true)&&(Nozzle_up_flg==false)&&(LCD_status!=11)&&(reboot_ok!=true))
                {
                    plate_enter_count++;
                    if(test)qDebug()<<"Cancel-entered1-"<<plate_enter_count;
                    if(plate_enter_count>=300)
                    {
                        plate_enter_count=0;
                        Person_is_Ok=false;
                        Cancel_flg=1;
                        Customer_plate.clear();
                        Keypad_data_for_LCD.clear();
                        CustomerID.clear();
                        pump[pumpNo].PLATE.clear();
                        qDebug()<<"Cancel-entered-plate1";
                    }

                }
            }
        }
    }
    else
    {
        if(meter_com_err==false)
        {
            if((USC_is_exist==false) && (config_error==false) && (End_total_report==false) && (reboot_ok!=true))  //USC yoksa
            {   Confirmation_flg_without_km=false;
                if(((Card_read_flg==1) || (TAG_read==true)) && (Person_wait_flg==false) && (Driver_wait_flg==false)&&(Confirmation_flg_ok!=true)&&(LCD_status!=11)) //Card_or_TAG_read_flg
                {
                    if(TAG_read==true)
                    {
                        CustomerID=TAGID[DCR_par.DCR_Nozzle_Number].toHex();
                        settings.setlog("SATIS_DEBUG TAG CustomerID  "+CustomerID);
                        if(test)qDebug()<<"TAG CustomerID" <<CustomerID;
                    }
                    if(database->kontrolSalesRfid(CustomerID))  //ilk kart okutuldu sorguya gitti
                    {
                        if(plate_entered_with_button==true)
                        {
                            Card_read_flg=0;
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0;// ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-AFTER-KEYPAD-PLATE-ID-KAYIT BULUNAMADI "+CustomerID);
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;

                        }
                        else
                        {
                            convert_check_RFID();
                            if(test)qDebug()<<"CARD Filling-Auth-type"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP;
                            settings.setlog("SATIS_DEBUG USC-FALSE-SALE-TYPE-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                            if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1) || (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==2)) //müşteri(TTS-MTS) kartı okutuduysa
                            {
                                switch (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP) {
                                case 1://tekli müşteri authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    Sales_start=0;
                                    Enter_flg=0;
                                    if((TAG_read==false) &&(Card_read_flg==1))
                                    {card_authorize=true;}
                                    TAG_read=false;
                                    Card_read_flg=0;
                                    if(ask_km==true)
                                    {
                                        TAG_KM_read=true;
                                        LCD_status=11; // km BİLGİSİ
                                        if(test)qDebug()<<"SATIS_DEBUG USC-FALSE-TEKLI-KM GIR EKRANI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-KM GIR EKRANI");
                                    }
                                    else
                                    {
                                        Confirmation_flg=1;
                                        ask_nozzle_total_volume=true;
                                        Confirmation_flg_ok=true;
                                        Sale_Error_code=2;
                                        Update_flg=true;
                                        LCD_refresh_flg=1;
                                        if(test)qDebug()<<"SATIS_DEBUG USC-FALSE-TEKLI-AUTHORIZE";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-TEKLI-AUTHORIZE");
                                    }
                                }
                                    break;
                                case 2:// müşteri + pompacı  authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-CIFTLI-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                                    settings.setlog("SATIS_DEBUG USC-FALSE-POMPACI Okutunuz");
                                    LCD_refresh_flg=1;
                                    Person_wait_flg=true;
                                    if((TAG_read==false) &&(Card_read_flg==1))
                                    {card_authorize=true;}
                                    TAG_read=false;
                                    Card_read_flg=0;
                                }
                                    break;
                                case 3:// müşteri + şöför + pompacı  authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-3LU-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    if(auttype_3_4==true)
                                    {
                                        TEXT_fromMaster=pleaseReadDriverCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-SOFOR Okutunuz");
                                        LCD_refresh_flg=1;
                                        Driver_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }
                                    else
                                    {
                                        TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-POMPACI Okutunuz");
                                        LCD_refresh_flg=1;
                                        Person_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }
                                }
                                    break;
                                case 4:// müşteri + şöför authorize
                                {
                                    settings.setlog("SATIS_DEBUG USC-FALSE-4-FTYP-ATYP "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP));
                                    if(auttype_3_4==true)
                                    {
                                        TEXT_fromMaster=pleaseReadDriverCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-SOFOR Okutunuz");
                                        LCD_refresh_flg=1;
                                        Driver_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }
                                    else
                                    {
                                        TEXT_fromMaster=pleaseReadDriverCard_String.toUtf8();
                                        settings.setlog("SATIS_DEBUG USC-FALSE-POMPACI Okutunuz");
                                        LCD_refresh_flg=1;
                                        Person_wait_flg=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                    }

                                }
                                    break;
                                default:
                                    break;
                                }
                            }
                            else //müşteri harici okutulan kart
                            {
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP!=13) //ilk şöför okutulmadı
                                {

                                    if(ask_km==true)
                                    {
                                        TAG_KM_read=true;
                                        LCD_status=11; // km BİLGİSİ
                                        if(test)qDebug()<<"KM GIR EKRANI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-OTHERS-KM GIR EKRANI");
                                    }
                                    else
                                    {
                                        Confirmation_flg=1;
                                        ask_nozzle_total_volume=true;
                                        Confirmation_flg_ok=true;
                                        if((TAG_read==false) &&(Card_read_flg==1))
                                        {card_authorize=true;}
                                        TAG_read=false;
                                        Card_read_flg=0;
                                        Sale_Error_code=2;
                                        Update_flg=true;
                                        if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-OTHERS-SATIS AUTHORIZE GONDER");
                                    }
                                }
                                else   //ilk şöför kartı okutulduysa izin yok
                                {
                                    Card_read_flg=0;
                                    TAG_read=false;
                                    LCD_status=9; // kayıt bulumadı lcd
                                    Allow_card_read=0; //ANIL
                                    LCD_refresh_flg=1;
                                    if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                    settings.setlog("SATIS_DEBUG USC-FALSE-ILK-SOFOR-OKUTULDU-HATA");
                                    Temp_ID[0].clear();
                                    Temp_ID[1].clear();
                                    Temp_ID[2].clear();
                                    tag_read_count=0;
                                    Tag_read_count_flg=0;
                                    Person_is_Ok=false;

                                }
                            }
                        }
                    }
                    else ///pompacı kontrol
                    {
                        if(test) qDebug()<<"pompacı kontrol";
                        settings.setlog("SATIS_DEBUG USC-FALSE-ILK-SOFOR-OKUTULDU-HATA");
                        if(TAG_read!=true)
                        {
                            if(database->kontrolPersonRfid(CustomerID))
                            {
                                //pumper_flg=true;

                                if(plate_entered_with_button==true)
                                {

                                    if(database->kontrolSalesPlate(pump[pumpNo].PLATE))  //önceden plaka girildiğinde nozzle a değerler yüklensin diye
                                    {

                                        qDebug()<<"kotrol plaka dogru";
                                        convert_check_RFID();
                                        if(test)qDebug()<<"plate_entered_with_button AUTHORIZE";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP));
                                        if(ask_km==true)
                                        {
                                            LCD_status=11; // KM GİRİS
                                            LCD_refresh_flg=1;
                                            if(test)qDebug()<<"KM GIRIS EKRANI";
                                            settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button KM GIRIS EKRANI ");
                                        }
                                        else
                                        {
                                            Sales_start=0;
                                            plate_entered_with_button=false;
                                            Enter_flg=0;
                                            if((TAG_read==false) &&(Card_read_flg==1))
                                            {card_authorize=true;}
                                            TAG_read=false;
                                            Card_read_flg=0;
                                            qDebug()<<"pumpervd";
                                            StoredScreenRFID_fromMaster=2; //person
                                            Confirmation_flg=1;
                                            ask_nozzle_total_volume=true;
                                            Confirmation_flg_ok=true;
                                            KM_Confirmation_flg=true;
                                            Sale_Error_code=2;
                                            Update_flg=true;
                                            LCD_refresh_flg=1;
                                            if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                            settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button SATIS AUTHORIZE GONDER ");
                                        }
                                    }
                                    else
                                    {
                                        Card_read_flg=0;
                                        plate_entered_with_button=false;
                                        LCD_status=6; // kayıt bulumadı lcd
                                        LCD_refresh_flg=1;
                                        if(test)qDebug()<<"PLAKA ÖNCE DATABASE HATASI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-plate_entered_with_button PLAKA BULUNAMADI ");
                                        Temp_ID[0].clear();
                                        Temp_ID[1].clear();
                                        Temp_ID[2].clear();
                                        tag_read_count=0;
                                        Tag_read_count_flg=0;
                                        Person_is_Ok=false;
                                    }


                                }
                                else
                                {
                                    if((TAG_read==false) &&(Card_read_flg==1))
                                    {card_authorize=true;}
                                    if(test)qDebug()<<"Plaka Giriniz ekranı_1"<<card_authorize;
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Plaka Giriniz ekranı_1 ");
                                    LCD_status=2; // Plaka Giriniz ekranı
                                    LCD_refresh_flg=1;

                                }
                            }
                            else
                            {
                                //pumper_flg=false;
                                qDebug()<<"kotrol plaka yanlış1";
                                Card_read_flg=0;
                                LCD_status=9; // kayıt bulumadı lcd
                                Allow_card_read=0; //ANIL
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-1 ");
                                Temp_ID[0].clear();
                                Temp_ID[1].clear();
                                Temp_ID[2].clear();
                                tag_read_count=0;
                                Tag_read_count_flg=0;
                                Person_is_Ok=false;
                            }
                        }
                        else
                        {
                            qDebug()<<"kotrol plaka yanlış2";
                            Card_read_flg=0;
                            TAG_read=false;
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0; //ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Pompacı-Kayıt-Bulunamadı-2 ");
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;
                        }

                    }

                }

                //////POMPACI SORGU
                if(Card_read_flg==1)
                {
                    if(Person_wait_flg==true) // customer + pompacı için pompacı bekleniyor
                    {
                        Person_wait_flg=false;
                        Card_read_flg=0;
                        settings.setlog("SATIS_DEBUG USC-FALSE-customer + pompacı Icin Pompacı Kontrol "+CustomerID);
                        if(database->kontrolPersonRfid(CustomerID)) //pompacı database control
                        {
                            //pumper_flg=true;
                            Card_read_flg=0;

                            if(ask_km==true)
                            {
                                TAG_KM_read=true;
                                LCD_status=11; // km BİLGİSİ
                                if(test)qDebug()<<"KM GIR EKRANI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Person-wait KM GIR EKRANI");
                            }
                            else
                            {
                                Confirmation_flg=1;
                                ask_nozzle_total_volume=true;
                                Confirmation_flg_ok=true;
                                Sale_Error_code=2;
                                Update_flg=true;
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Person-wait SATIS AUTHORIZE GONDER");
                            }

                        }
                        else
                        {
                            //pumper_flg=false;
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0; //ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Person-wait KAYIT BULUNAMADI");
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;
                        }

                    }
                }

                //////ŞÖFÖR SORGU
                if(Card_read_flg==1)
                {
                    if(Driver_wait_flg==true) // customer + Şöför +pompacı+ için şöför bekleniyor
                    {
                        Driver_wait_flg=false;
                        Card_read_flg=0;
                        settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait Sofor Kontrol "+CustomerID);
                        if(database->kontrolSalesRfid(CustomerID)) //ŞÖFÖR database control
                        {
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].DFTYP==13)  //ŞÖFÖR varsa
                            {
                                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==3)
                                {
                                    TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                                    settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait Pompacı Okutunuz");
                                    LCD_refresh_flg=1;
                                    Person_wait_flg=true;
                                    TAG_read=false;
                                }
                                else //Customer + Şöför  FTYP ==4
                                {
                                    Card_read_flg=0;
                                    if(ask_km==true)
                                    {
                                        TAG_KM_read=true;
                                        LCD_status=11; // km BİLGİSİ
                                        if(test)qDebug()<<"KM GIR EKRANI";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait KM GIR EKRANI");
                                    }
                                    else
                                    {
                                        Confirmation_flg=1;
                                        ask_nozzle_total_volume=true;
                                        Confirmation_flg_ok=true;
                                        Sale_Error_code=2;
                                        Update_flg=true;
                                        LCD_refresh_flg=1;
                                        if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                        settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait SATIS AUTHORIZE GONDER");
                                    }
                                }
                            }
                            else
                            {
                                LCD_status=9; // kayıt bulumadı lcd
                                Allow_card_read=0; //ANIL
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait KAYIT BULUNAMADI-1");
                                Temp_ID[0].clear();
                                Temp_ID[1].clear();
                                Temp_ID[2].clear();
                                tag_read_count=0;
                                Tag_read_count_flg=0;
                                Person_is_Ok=false;
                            }

                        }
                        else
                        {
                            LCD_status=9; // kayıt bulumadı lcd
                            Allow_card_read=0; //ANIL
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KART_OKUTULDU-KAYIT BULUNAMADI";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Driver_wait KAYIT BULUNAMADI-2");
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            tag_read_count=0;
                            Tag_read_count_flg=0;
                            Person_is_Ok=false;
                        }

                    }
                }


                if(Plate_control_flg==1)
                {
                    Plate_control_flg=0;
                    Enter_flg=0;
                    qDebug()<<"here2";
                    TEXT_fromMaster=checkingPlate_ByteArray;
                    LCD_status=1; //plaka kontrolü yapılıyor
                    LCD_refresh_flg=1;
                    if(test)qDebug()<<"plaka kontrolü yapılıyor"<<QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE);
                    settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg PLAKA KONTROLU "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex()));
                    Person_is_Ok=true;
                    QString temp_plate;
                    temp_plate=QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE);
                    if(plate_entered_with_button==true)
                    {
                        settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg plate_entered_with_button "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex()));
                        temp_plate=pump[pumpNo].PLATE;
                        if(Nozzle_up_flg==1)
                        {
                            TEXT_fromMaster=pleaseReadAttendantCard_String.toUtf8();
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg plate_entered_with_button POMPACI OKUTUNUZ");
                        }
                        else
                        {
                            TEXT_fromMaster=pleaseTakeANozzleAndReadAttendantCard_ByteArray;
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg plate_entered_with_button TABANCA ALINIZ POMPACI OKUTUNUZ");
                        }
                        LCD_status=1; //ana ekrana dön
                        LCD_refresh_flg=1;
                    }
                    else
                    {
                        if(test)qDebug()<<"temp_plate"<<temp_plate;
                        settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg temp_plate "+temp_plate);
                        if(database->kontrolSalesPlate(temp_plate))
                        {
                            convert_check_RFID();
                            if(test)qDebug()<<"PLAKA KAYIT VAR";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg PLAKA KAYIT VAR");
                            if(ask_km==true)
                            {
                                LCD_status=11; // KM GİRİS
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"KM GIRIS EKRANI";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg KM GIRIS EKRANI");
                            }
                            else
                            {
                                Sales_start=0;
                                Enter_flg=0;
                                qDebug()<<"pumper98";
                                StoredScreenRFID_fromMaster=2; //person
                                Confirmation_flg=1;
                                ask_nozzle_total_volume=true;
                                Confirmation_flg_ok=true;
                                KM_Confirmation_flg=true;
                                Sale_Error_code=2;
                                Update_flg=true;
                                LCD_status=1; //ana ekrana dön
                                LCD_refresh_flg=1;
                                if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                                settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg SATIS AUTHORIZE GONDER");
                            }


                        }
                        else
                        {
                            CustomerID=QString::fromUtf8(Customer_plate);
                            LCD_refresh_flg=1;
                            LCD_status=6; // PLAKA KAYIT BULUNAMADI
                            TAG_read=false;
                            Card_read_flg=0;
                            if(test)qDebug()<<"PLAKA KAYIT YOK";
                            settings.setlog("SATIS_DEBUG USC-FALSE-Plate_control_flg PLAKA KAYIT YOK");
                            Temp_ID[0].clear();
                            Temp_ID[1].clear();
                            Temp_ID[2].clear();
                            Customer_plate.clear();
                            Person_is_Ok=false;
                        }
                    }
                }

                if((Plate_screen_flg==true) && (Confirmation_flg!=1) && (Plate_control_flg!=1))
                {
                    Plate_screen_flg=false;
                    //POMPACI KARTI PLAKA SORGULA
                    if(test)qDebug()<<"Plaka Giriniz ekranı_2";
                    settings.setlog("SATIS_DEBUG USC-FALSE-Plate_screen_flg Plaka Giriniz");
                    LCD_status=2; // Plaka Giriniz ekranı
                    LCD_refresh_flg=1;
                    plate_entered_with_button=true;

                }
                qDebug()<<plate_enter_count<<"two"<<Person_is_Ok<<"nozleupflg"<<Nozzle_up_flg<<LCD_status<<reboot_ok;
                if((Person_is_Ok==true)&&(Nozzle_up_flg==false)&&(LCD_status!=11)&&(reboot_ok!=true))
                {
                    plate_enter_count++;
                    if(test)qDebug()<<"Cancel-entered1-"<<plate_enter_count;
                    if(plate_enter_count>=300)
                    {
                        plate_enter_count=0;
                        Person_is_Ok=false;
                        Cancel_flg=1;
                        Customer_plate.clear();
                        Keypad_data_for_LCD.clear();
                        CustomerID.clear();
                        pump[pumpNo].PLATE.clear();
                        qDebug()<<"Cancel-entered-plate2";
                    }

                }
            }
            else if((USC_is_exist==true) && ((reboot_ok!=true)))  //USC bağlantısı varsa
            {

                //        switch (StoredScreenRFID_fromMaster) {
                //        case 1:line2="   READ CARD      "; break;
                //        case 2:line2="   PERSON CARD    "; break;
                //        case 3:line2="  CUSTOMER CARD   "; break;
                //        case 4:line2="  VEHICLE CARD    "; break;
                //        case 5:line2="   WRONG CARD     "; break;
                //        case 6:line2="   BLACKLIST      "; break;
                //        case 7:line2="     STAWIZ       "; break;
                //        case 8:line2="     ERROR        "; break;
                //        case 9:line2="     STAWIZ       "; break;
                //        case 10:line2="    LOYALTY      "; break;
                //        default:
                //            break;
                //        }


                if(((Card_read_flg==1) || (TAG_read==true)) &&(Confirmation_flg_ok!=true))
                {
                    //TEXT_fromMaster=idControl_String;
                    if(test) qDebug()<<"STORED and ID COUNT :"<<StoredScreenRFID_fromMaster<<"--->"<<ID_count;
                    if((StoredScreenRFID_fromMaster==2) && (ID_count==1)) // pompacı ise plaka sor
                    {
                        LCD_refresh_flg=1;
                        if(plate_entered_with_button!=true) //önce pompacı kartı okutulduysa
                        {
                            //POMPACI KARTI PLAKA SORGULA
                            if(test)qDebug()<<"PLAKA GIRINIZ EKRANI";
                            settings.setlog("SATIS_DEBUG USC-OK-PERSON CARD PLAKA GIRINIZ EKRANI");
                            LCD_status=2; // Plaka Giriniz ekranı
                            LCD_refresh_flg=1;


                        }
                        else  //plaka önceden girildiyse
                        {
                            Enter_flg=0;
                            TEXT_fromMaster=checkingCard_ByteArray;
                            LCD_refresh_flg=1;
                            settings.setlog("SATIS_DEBUG USC-OK-PERSON CARD plate_entered_with_button "+QString::number(Confirmation_flg_without_km));
                            if(Confirmation_flg_without_km==true)
                            {
                                if(ask_km==true)
                                {
                                    LCD_status=11; // km BİLGİSİ
                                    LCD_refresh_flg=1;
                                    if(test)qDebug()<<"KM GIR EKRANI";
                                    settings.setlog("SATIS_DEBUG USC-OK-PERSON CARD plate_entered_with_button KM GIR EKRANI");
                                    Confirmation_flg_without_km=false;

                                }
                                else
                                {
                                    LCD_status=1; // dolum bekleniyor
                                    LCD_refresh_flg=1;
                                    Confirmation_flg_ok=true;
                                    Confirmation_flg_without_km=false;
                                    settings.setlog("SATIS_DEBUG USC-OK-PERSON CARD plate_entered_with_button DOLUM BEKLENIYOR");
                                }
                                Plate_control_flg=0;
                            }
                        }
                    }
                    else if(((StoredScreenRFID_fromMaster==3) ||(StoredScreenRFID_fromMaster==4)||(StoredScreenRFID_fromMaster==10)) && (Confirmation_flg==1))
                    {
                        settings.setlog("SATIS_DEBUG USC-OK-TTS-MTS-LOYALTY "+QString::number(StoredScreenRFID_fromMaster));
                        if(ask_km==true)
                        {
                            LCD_status=11; // km BİLGİSİ
                            LCD_refresh_flg=1;
                            TAG_read=false;
                            if(test)qDebug()<<"KM GIR EKRANI";
                            settings.setlog("SATIS_DEBUG USC-OK-TTS-MTS-LOYALTY KM GIR EKRANI ");
                        }
                        else
                        {
                            TAG_read=false;
                            Confirmation_flg_without_km=false;  ////sonradan
                            Confirmation_flg_ok=true; ////sonradan
                        }

                    }
                }

                if((Nozzle_up_flg==1)&& ((StoredScreenRFID_fromMaster==5) || (StoredScreenRFID_fromMaster==6)||(StoredScreenRFID_fromMaster==8))) //kart hatası
                {
                    Card_read_flg=0;
                    Allow_card_read=0;
                    TAG_read=false;
                    //LCD_status=9; // KART HATASI
                    LCD_refresh_flg=1;
                    Plate_control_flg=0;
                    ID_count=0;
                    Person_is_Ok=false;
                    settings.setlog("SATIS_DEBUG USC-OK-ERROR ");
                }

                if(Plate_control_flg==1) ///PLAKA GİRİLDİYSE
                {
                    Card_read_flg=0;
                    QString temp_plate_2;
                    Person_is_Ok=true;
                    temp_plate_2=QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE);
                    if(plate_entered_with_button!=true)
                    {
                        Enter_flg=0;
                        qDebug()<<"here";
                        TEXT_fromMaster=checkingPlate_ByteArray;
                        LCD_status=1;
                        LCD_refresh_flg=1;
                        settings.setlog("SATIS_DEBUG USC-OK-PLAKA KONTROLU ");
                    }
                    else
                    {
                        //Person_is_Ok=true;
                        temp_plate_2=QString::fromUtf8(pump[pumpNo].PLATE);
                        if(Nozzle_up_flg!=1)
                        {
                            TEXT_fromMaster=pleaseTakeANozzle_ByteArray;
                        }
                        LCD_status=1;
                        LCD_refresh_flg=1;
                    }
                    if(Confirmation_flg_without_km==true)
                    {
                        // Person_is_Ok=true;
                        temp_plate_2=temp_plate_2.replace(" ","");
                        if(test)qDebug()<<"Confirmation_flg=1"<<Customer_plate<<temp_plate_2;
                        settings.setlog("SATIS_DEBUG USC-OK-AUTHORIZE-GELDI "+temp_plate_2);
                        if(database->kontrolSalesPlate(temp_plate_2))
                        {
                            if(test)qDebug()<<"plaka için customer ID sorgula";
                            settings.setlog("SATIS_DEBUG USC-OK-plaka için customer ID sorgula");
                        }
                        else
                        {
                            //Person_is_Ok=false;
                            if(test)qDebug()<<"plaka için customer ID sorgula-false";
                            settings.setlog("SATIS_DEBUG USC-OK-plaka için customer ID sorgula-FALSE");
                        }
                        if(ask_km==true)
                        {
                            LCD_status=11; // km BİLGİSİ
                            LCD_refresh_flg=1;
                            if(test)qDebug()<<"KM GIR EKRANI";
                            settings.setlog("SATIS_DEBUG USC-OK-KM GIR EKRANI");
                            Confirmation_flg_without_km=false;

                        }
                        else
                        {
                            LCD_status=1; // dolum bekleniyor
                            LCD_refresh_flg=1;
                            Confirmation_flg_ok=true;
                            Confirmation_flg_without_km=false;
                            settings.setlog("SATIS_DEBUG USC-OK-   DOLUM BEKLENIYOR");
                        }
                        Plate_control_flg=0;
                        plate_entered_with_button=false;
                    }
                    if(test)qDebug()<<"PLAKA KONTROL EKRANI"<<Customer_plate;
                }

                if(Plate_screen_flg==true)
                {
                    Plate_screen_flg=false;
                    //POMPACI KARTI PLAKA SORGULA
                    if(test)qDebug()<<"Plaka Giriniz ekranı";
                    settings.setlog("SATIS_DEBUG USC-OK-Plate_screen_flg Plaka Giriniz ekranı");
                    LCD_status=2; // Plaka Giriniz ekranı
                    LCD_refresh_flg=1;
                    plate_entered_with_button=true;
                    Keypad_data_for_LCD.clear();
                }
                if((Person_is_Ok==true)&&(Nozzle_up_flg==false)&&(LCD_status!=11)&&(reboot_ok!=true))
                {
                    plate_enter_count++;
                    if(test) qDebug()<<"Cancel-entered2-"<<plate_enter_count;
                    if(plate_enter_count>=30)
                    {
                        plate_enter_count=0;
                        Person_is_Ok=false;
                        Cancel_flg=1;
                        Customer_plate.clear();
                        Keypad_data_for_LCD.clear();
                        CustomerID.clear();
                        pump[pumpNo].PLATE.clear();
                    }

                }
            }
        }
    }
}

void main_routine::clock()
{
    if(LCD_status==1)
    {
        LCD_refresh_flg=1;
    }

}


