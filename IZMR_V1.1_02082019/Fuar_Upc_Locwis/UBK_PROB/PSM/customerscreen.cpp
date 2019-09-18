
#include "customerscreen.h"
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
#include <QVector>
#include <QThread>
#include"global.h"

QString getMacAddress();
unsigned char kk;
int lcd_count;
int check_status1_count(QString tablo);
int text_cursor_count;
QByteArray old_temp_text;
CustomerScreen *cusscr;


CustomerScreen::CustomerScreen()
{

    tik_count=0;
    CustomerID.clear();
    previous_CustomerID.clear();
    line1.clear();
    line2.clear();
    line3.clear();
    line4.clear();

    LCD_status=19;
    LCD_refresh_flg=1;


    kk=0;
    error_gprs="OFFLINE";
    StoredScreenRFID_fromMaster=9;
    
}



void CustomerScreen::LCD_scan_timer()
{
    LCDtimer1->start(50);
    //LCDtimer2->start(300);
}

void CustomerScreen::test_slot()
{

}

void CustomerScreen::LCD_refresh_2()
{
    LCD_refresh_flg=1;
}
void CustomerScreen::LCD_refresh()
{
    QString secretpaswrd;
    QString TempScreenString;
    //system("cp -f /onset/printk /proc/sys/kernel");
    LCD_refresh_flg=1;
    if(LCD_refresh_flg==1)
    {
        Clear_flg=0;
        LCD_refresh_flg=0;
        StatusLine.clear();
        ErrorLine.clear();
        switch (StoredScreenRFID_fromMaster) {
        case 1:StatusLine=pleaseReadCard_String; pumper_flg=false;  if(test) qDebug()<<"KART OKUTUNUZ --> StoredScreenRFID_fromMaster :"<<StoredScreenRFID_fromMaster; break;
        case 2:StatusLine=attendantCard_String; pumper_flg=true; break;
        case 3:StatusLine=customerCard_String; /*pumper_flg=false; */break;
        case 4:StatusLine=visTag_String; break;
        case 5:StatusLine=wrongCard_String; break;
        case 6:StatusLine=blackList_String; break;
        case 8:StatusLine=error_String; break;
        case 10:StatusLine=loyalty_String; break;
        case 11:StatusLine="LÜTFEN BEKLEYİNİZ"; break;
        default:
            break;
        }
        switch (LCD_status) {
        if(test) qDebug()<<"LCD STATUS -->"<<LCD_status;
        case 1: //AÇILIŞ EKRANI

            if((StoredScreenRFID_fromMaster!=1) && (StoredScreenRFID_fromMaster!=8))
            {
                StatusLine=readyToSale_String;
                ErrorLine.clear();
            }
            break;
        case 2: //PLAKA GiRiNiZ EKRANI

            //                if(Refresh_count==1)
            //                {
            //                    line3="                ";
            //                    LCD_refresh_flg=1;
            //                }
            //                else
            //                {
            //                    line3="   "+Keypad_data_for_LCD;//Licence_plate_code
            //                    LCD_refresh_flg=1;
            //                }
            break;

        case 3: //ID KONTROL EKRANI
            TempScreenString="CARD";
            if(TAG_read==true){TempScreenString="VEHICLE";}
            StatusLine=TempScreenString+idChecking_String;
            break;
        case 4: //SATIS ONAY
            break;
        case 5: //
            StatusLine=waitingPayment_String;
            break;
        case 6: //PLAKA KONTROL EKRANI
            if(limit_error==true)
            {
                StatusLine=inadequateLimit_String;
            }
            else if(wrong_product==true){
                StatusLine=error_String;
                ErrorLine.clear();
                ErrorLine=wrongProduct_String;
            }
            else
            {
                StatusLine=unregisteredPlate_String;
            }
            break;
        case 7: //
            StatusLine.append(waitingFilling_String);
            break;
        case 8: //
            StatusLine.append(filling_String);
            break;
        case 9: //
            if(USC_is_exist==false)
            {
                StatusLine=error_String;
                if(limit_error==true)
                {
                    ErrorLine=inadequateLimit_String;
                }
                else
                {
                    ErrorLine=wrongId_String;
                }
                if(wrong_product==true)
                {
                    ErrorLine=wrongProduct_String;
                }
                if(passive_id==true)
                {
                    ErrorLine=passiveId_String;
                }
                if(deleted_id==true)
                {
                    ErrorLine=deletedId_String;
                }
            }
            break;
        case 10: //ID plaka ekranı
            StatusLine.append(fillingWasStopped_String);
            break;
        case 11: //KM GiRiNiZ EKRANI
            //                line2="   KM GiRiNiZ   ";
            //                if(Refresh_count2==1)
            //                {
            //                    line3="                ";
            //                    LCD_refresh_flg=1;
            //                }
            //                else
            //                {
            //                    line3="     "+vehiclekmtr.toLatin1();//Licence_plate_code
            //                }
            //                line4="           ENTER";
            //                if(Refresh_count2>100){Refresh_count2=1;}
            break;

        case 12: //VERSION
            line1="VERSION:"+Myversion.toLatin1()+"   ";
            line2="                ";
            line3="MAC:"+getMacAddress().remove(":").toLatin1();
            line4="                ";
            break;
        case 13: //LICENCE
            last_screen_flg=13;
            if(system_lock==false)
            {
                StatusLine=invalidLicence_String;
            }
            else
            {
                StatusLine=systemLocked_String;

            }
            break;
        case 14: //IP
            line1="       IP       ";
            line2=" "+Static_ip.toLatin1()+"     ";
            line3="      IMEI      ";
            line4=" "+imei.toLatin1();
            break;
        case 15: //licence
            secretpaswrd=secretpaswrd.rightJustified(sifre.length(),'*');
            if(USC_com_error_flg==true)
            {
                line1=automationError_String.toUtf8();
                line2=enterPassword_String.toUtf8();
                line3=password_String.toUtf8()+":"+secretpaswrd.toLatin1()+"           ";
                line4="                ";
            }
            else
            {
                line1=enterPassword_String.toUtf8();
                line2="                ";
                line3=password_String.toUtf8()+":"+secretpaswrd.toLatin1()+"           ";
                line4="                ";
            }

            break;
        case 16: //licence
            StatusLine=fieldModeActive_String;
            break;
        case 17: //Uprice
            line1=enterUnitPrice_String.toUtf8();
            line2=nozzle_String.toUtf8()+"-"+QByteArray::number(screen_UP_nozzle)+":"+Lastprice[screen_UP_nozzle]+"        ";
            line4="                ";
            line3=newUnitPrice_String.toUtf8()+Unit_price_for_save[screen_UP_nozzle]+"         "; //???
            break;
        case 18: //Uprice
            line1="                ";
            line2=unitPrice_String.toUtf8();
            line3=updated_String.toUtf8();
            line4="                ";
            break;
        case 19: //REBOOT
            StatusLine=startingSystem_String;
            break;
        case 20: //PLAKA KONTROL EKRANI
            break;
        default:
            break;

        }

        if(meter_com_err==true)
        {
            line2="                ";
            line3=pumpCommunicationError_String.toUtf8();
            line4="                ";
            StatusLine=pumpCommunicationError_String;

            ErrorLine=pleaseCheckPumpConnection_String;
        }
        if((LCD_status!=2) &&(LCD_status!=15) && (LCD_status!=3) && (LCD_status!=11)&& (meter_com_err==false))
        {
            if(LCD_status==9)
            {
                qDebug()<<"mayd";
                TEXT_fromMaster=CustomerID.toLatin1();
                if(USC_is_exist==false)
                {
                    if(limit_error==true){ErrorLine=inadequateLimit_String;}
                }
            }
            if(wrong_product==false)
                //qDebug()<<"mayd"<<TEXT_fromMaster;
                ErrorLine.prepend(QString::fromUtf8(TEXT_fromMaster));
        }
        if(printer_paper==false)
        {
            ErrorLine=printerPaperOut_String;
        }

        LCD_refresh_flg=1;

        emit old_screen_lines(StatusLine,ErrorLine);
        emit changeFrame(LCD_status);
    }
}


