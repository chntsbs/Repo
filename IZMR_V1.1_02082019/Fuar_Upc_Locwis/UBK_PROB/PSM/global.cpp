#include<global.h>
#include<QString>
#include <QCoreApplication>
#include<QtCore>
#include <QByteArray>
#include"syslog.h"
#include"unistd.h"

Crypto CryptoObj;

QThread *LCDThread;
QTimer *LCDtimer1;
QTimer *buzzertimer;
QSqlDatabase constructor_db = QSqlDatabase::addDatabase("QSQLITE","constructor");
QSqlDatabase get_price_db = QSqlDatabase::addDatabase("QSQLITE","get_price");
QSqlDatabase nozzle_db = QSqlDatabase::addDatabase("QSQLITE","nozzle");
QSqlDatabase nozzle_product_id_db = QSqlDatabase::addDatabase("QSQLITE","nozzle_product_id");
QSqlDatabase nozzle_values_db = QSqlDatabase::addDatabase("QSQLITE","nozzle_values");
QSqlDatabase nozzle_product_price_db = QSqlDatabase::addDatabase("QSQLITE","nozzle_product_price");
QSqlDatabase nozzle_total_amount_db = QSqlDatabase::addDatabase("QSQLITE","nozzle_total_amount");
QSqlDatabase nozzle_total_volume_db = QSqlDatabase::addDatabase("QSQLITE","nozzle_total_volume");
QSqlDatabase nozzle_total_sales_db = QSqlDatabase::addDatabase("QSQLITE","nozzle_total_sales");
QSqlDatabase count_id_db = QSqlDatabase::addDatabase("QSQLITE","count_id");
QSqlDatabase update_nozzle_product_db = QSqlDatabase::addDatabase("QSQLITE","update_nozzle_product");
QSqlDatabase update_nozzle_price_db = QSqlDatabase::addDatabase("QSQLITE","update_nozzle_price");
QSqlDatabase update_nozzle_total_db = QSqlDatabase::addDatabase("QSQLITE","update_nozzle_total");
QSqlDatabase insert_sales_db = QSqlDatabase::addDatabase("QSQLITE","insert_sales");
QSqlDatabase insert_probe_db = QSqlDatabase::addDatabase("QSQLITE","insert_probe");
QSqlDatabase update_probe_db = QSqlDatabase::addDatabase("QSQLITE","update_probe");
QSqlDatabase delete_table_db = QSqlDatabase::addDatabase("QSQLITE","delete_table");
QSqlDatabase sales_date_db = QSqlDatabase::addDatabase("QSQLITE","sales_date");
QSqlDatabase sales_status_db = QSqlDatabase::addDatabase("QSQLITE","sales_status");
QSqlDatabase update_status_db = QSqlDatabase::addDatabase("QSQLITE","update_status");
QSqlDatabase update_dateclock_db = QSqlDatabase::addDatabase("QSQLITE","update_dateclock");
QSqlDatabase update_sales_db = QSqlDatabase::addDatabase("QSQLITE","update_sales");
QSqlDatabase update_plate_db = QSqlDatabase::addDatabase("QSQLITE","update_plate");
QSqlDatabase total_sales_db = QSqlDatabase::addDatabase("QSQLITE","total_sales");
QSqlDatabase whitelist_db = QSqlDatabase::addDatabase("QSQLITE","whitelist");
QSqlDatabase sales_plate_db = QSqlDatabase::addDatabase("QSQLITE","sales_plate");
QSqlDatabase sales_id_db = QSqlDatabase::addDatabase("QSQLITE","sales_id");
QSqlDatabase controlsales_id_db = QSqlDatabase::addDatabase("QSQLITE","controlsales_id");
QSqlDatabase person_db = QSqlDatabase::addDatabase("QSQLITE","person");
QSqlDatabase control_person_db = QSqlDatabase::addDatabase("QSQLITE","control_person");
QSqlDatabase show_person_db = QSqlDatabase::addDatabase("QSQLITE","show_person");
QSqlDatabase show_nozzle_db = QSqlDatabase::addDatabase("QSQLITE","show_nozzle");
QSqlDatabase show_whitelist_db = QSqlDatabase::addDatabase("QSQLITE","show_whitelist");
QSqlDatabase show_sales_db = QSqlDatabase::addDatabase("QSQLITE","show_sales");
QSqlDatabase tankLevel_db = QSqlDatabase::addDatabase("QSQLITE","tankLevel");
QSqlDatabase plates_db = QSqlDatabase::addDatabase("QSQLITE","plates");

SaveFile settings;

QString getMacAddress();
DCRpar DCR_par;
Delta Delta_m;
Delta Delta_meter;

// ---------------------------- RFID Variables ----------------------------- //

quint8 NozzleRFID_fromMaster;
quint8 DummyRFID_fromMaster;
quint8 StoredScreenRFID_fromMaster;
quint8 SCR_X_fromMaster;
quint8 SCR_Y_fromMaster;
quint8 Text_With_fromMaster;
quint8 Pixel_OnOff_fromMaster;
quint8 request_flg_RFID;
quint8 NewPlateExist=0;
quint8 ABU_address;
quint8 cash_register_pump_no;

QByteArray RFID_RandomKey;
QByteArray TextLen_fromMaster;
QByteArray TEXT_fromMaster;
QByteArray CardReader_Addr;
//--------------------------------------------------------------------------------

bool pumper_flg;
bool IETT=true;
bool new_configuration=false;
bool upc_locvis=true;
bool printer_paper=true;
bool serial_debug_flg=false;
bool clrc_debug_flg=false;
bool meter_debug_flg=false;
bool RFID_debug_flg=true;
bool printer_debug_flg=true;
bool gprs_debug=false;
bool update_debug_flg=false;
bool ask_nozzle_total_volume=false;
bool ask_nozzle_total_amount=false;
bool End_total_report=false;
bool debug_flg=0;
bool updatedatabase=false;
bool LCD_refresh_flg;
bool Pump_Filling_status;
bool suspend_first=false;
bool Sale_suspend_flg;
bool Sale_resume_flg;
bool online_flg;
bool check_server;
bool meter_last_error=false;
bool clrc_check_flg=false;
bool keypad_check_flg=false;
bool lcd_check_flg=false;
bool ack_receive_flg;
bool eot_receive_flg;
bool nak_receive_flg;
bool data_receive_flg=false;
bool dcr_ini_flg;
bool tag_Random_flg=1;
bool authorize_flg=1;
bool ok;
bool Send_authorize_flg;
bool Preset_Ack_flg;
bool Preset_Control_flg;
bool Update_flg;
bool update_message_sent;
bool ask_last_screen;
bool Allow_card_read ;
bool Card_reader_chose;
bool bater_low_flg;
bool pump_sta;
bool ask_km;
bool askSellingStyle;
bool askPlateSale;
bool next=false;
bool plateDB=false;
bool serbestMusteri=false;
bool customerCard;
bool customerCardLock;
bool customerCardRead;
bool kmConfirm_flg;
bool ask_km_after;
bool update_isImmediate_flg=0;
bool update_download_flg=0;
bool meter_com_err;
bool only_card_read_flg=false;
bool plate_ini_flg;
bool card_kontrol=false;
bool checkinternet=false;
bool givecopyflg=false;
bool SuspendReply=false;
bool ResumeReply=false;
bool USC_is_exist;
bool Paid_Confirm;
bool Change_status_to_IDLE;
bool Send_Card_reader;
bool TAG_read;
bool card_authorize;
bool Wrong_trans;
bool Wrong_trans_for_save=false;
bool Not_resume_TAG;
bool save_TS_Time_flg;
bool update_whitelist=false;
bool update_person=false;
bool limit_error=false;
bool USC_com_error_flg=false;
bool ttspause;
bool Tag_read_flg;
bool KM_Confirmation_flg;
bool TAG_KM_read;
bool Driver_wait_flg;
bool Person_wait_flg;
bool auttype_3_4;
bool Plate_screen_flg;
bool plate_entered_with_button;
bool allow_button_plate;
bool Card_read_flg;
bool Volumetotal_received;
bool Amounttotal_received;
bool Confirmation_flg_ok=false;  //usc yokken ekran için kondu
bool system_started;
bool Person_is_Ok;
bool virtual_pump;
bool kontrol_id;
bool Confirmation_flg_without_km;
bool test=false;
bool config_error;
bool configuration;
bool protect_flg;
bool system_lock;
bool activation_flg;
bool connection_is_exist=false;
bool cannotchange_frame=false;
bool test_modu;
bool printer_check_flg;
bool rfid_check_flg;
bool print_test_mode;
bool USC_com_lock;
bool Send_authorize_flg_2;
bool clrc_up_flg;
bool buzzer;
bool wait_km_flg;
bool saat_tarih_error=false;
bool password_is_deactive=true;
bool reboot_ok;
bool pump_sta_2;
bool saving_the_sales;
bool routinedebug=false;
bool read_configuration=false;
bool write_configuration=false;
bool write_configuration_active=false;
bool nozzle_configuration;
bool probe_configuration;
bool setting_configuration;
bool config_kontrol=false;
bool wrong_product;
bool passive_id;
bool deleted_id;
bool odometre_active;
bool odometre;
int immediatelyUpdateBalanceLimit=0;


QString table_name;
QString sale_type;
QString CustomerID;
QString previous_CustomerID;
QString Temp_CustomerID;
QString Keypad_data;
QString platem;
QString printing_header_data[4];
QString fisdate;
QString fisclock;
QString Product_name;
QString kdvoran; // KDV oranı
QString type;
QString End_info[3];
QString SettingsFile="/var/www/html/setting.txt";
QString Amount_for_Database;
QString Volume_for_Database;
QString Unitprice_for_Database;
QString Liter_preset;
QString vehiclekmtr;
QString savePath;
QString savePath2;
QString targetPath;
QString targetPath2;
QString Myversion;
QString MyVersionDate;
QString downloadIniFile;
QString downloadExFile;
QString saveIniFile;
QString currentPath;
QString myIniFile;
QString zipFile;
QString checkFile;
QString checkFile2;
QString read_iniUpdate;
QString licence;
QString macAddress;
QString server;
QString imei;
QString Static_ip;
QString version;
QString sifre;
QString settingPath;
QString Psifre;
QString Float_setting;
QString Metre;
QString databasePath;
QString totalLT;
QString check_dowloadIniFile;
QString logDatem;
QString saved_day;
QString TS_time;
QString TS_date;
QString Nozzle_Up_Time;
QString Nozzle_Down_Time;
QString RFID_Read_Time;
QString RFID_Result_Time;
QString Authorize_Time;
QString Filling_Start_Time;
QString Paid_Confrim_Time;
QString Paid_Fiscal_Time;
QString commokpswrd;
QString otopswrd;
QString priceuppswrd="1234";
QString SalesID;
QString Saved_Customer_ID;
QString Saved_Driver_ID;
QString Saved_Person_ID;
QString Temp_ID[5];
QString Version_config_ini;
QString StatusLine;
QString ErrorLine;
QString usc_ip;
QString usc_port;
QString read_acc_auto_adr;
QString read_acc_nozzle_adr;
QString read_acc_channel;
QString read_acc_wait_time;
QString new_unit_price[6];
QString odometre_KM;
QString odometre_time;
QString balanceLimit;
QString gateWay;
QString customerPlate;
QString dataTime;

QByteArray ReadData_probe;
QByteArray printer_temp_buff;
QByteArray printer_temp_buff_for_database;
QByteArray space_buffer;
QByteArray ReadUSCData;
QByteArray ReadPrintData;
QByteArray ReadTempData;
QByteArray ReadPrintTempData;
QByteArray MACaddr;
QByteArray Lastprice[5];
QByteArray line1;
QByteArray line2;
QByteArray line3;
QByteArray line4;
QByteArray saved_unit_price;
QByteArray Licence_plate_code;
QByteArray Customer_plate;
QByteArray Keypad_data_for_LCD;
QByteArray Receipt_customer_plate;
QByteArray ReadTempData2;
QByteArray ReadTempData_RFID;
QByteArray SendPrintDataCOPY;
QByteArray tempBuffer;
QByteArray RandomBuffer;
QByteArray Decoding_Key;
QByteArray dcr_ini2=QByteArrayLiteral("\x51\xC0\xFA");
QByteArray dcr_ini=QByteArrayLiteral("\x50\xC0\xFA");
QByteArray dcr_pool=QByteArrayLiteral("\x50\x20\xFA");
QByteArray priceArray=QByteArrayLiteral("\x00\x15\x00");
QByteArray RandomArray=QByteArrayLiteral("\x00\x01\x05\x06\x055\x89\x56\x78\x30\x44\x88\x11\x45");
QByteArray ReadTAG;
QByteArray temp_Amount=QByteArrayLiteral("\x00\x00\x00\x00");
QByteArray temp_Volume=QByteArrayLiteral("\x00\x00\x00\x00");
QByteArray SendPrintData;
QByteArray Unit_price;
QByteArray kdvttr;
QByteArray kdv_temp_buff;
QByteArray stringBCD;
QByteArray Stringbcd[5];
QByteArray data_with_dot;
QByteArray Unit_price_for_save[5];
QByteArray error_gprs;
QByteArray TAGID[5];
QByteArray serial_No;
QByteArray Temp_TAGID;
QByteArray Coding_key;
QByteArray acc_otomasyon_adresi;
QByteArray acc_nozzle_adresi;
QByteArray acc_channel_no_adresi;
QByteArray acc_pause_time;
QByteArray probe_address[10];


int tempcard_reader_address;
int probe_address_select=-1;
int select_version=1;
int probe_count;
int keyboardclear;
int LCD_status;
int lock_status;
int LCD_status_last2;
int LCD_status_last;
int number_of_salessave;
int last_screen_flg;
int reboot_control_flg=0;
int count=0;
int  ZetNO;
int vardiyano;
int fisno;
int Product_KDV;
int bcd[5];
int send_meter_data_count;
int timeoutTime;
int tik_count;
int counttimer;
int update_check_flg;
int screen_UP_nozzle;
int ID_count;
int CLRC_ID_Count;
int total_nozzle_number;
int virtual_pumpno;
int sales_send_count;
int plate_enter_count;
int pump_lock_count;
int lock_day;
int getverison_count=0;
int USC_com_count;
int Sale_suspend_count;
int updateisstarting;
int licenceCheck;
int wait_km_second;
int system_shutdown_count;
int read_configuration_success=3;
int write_configuration_success;
int try_read_count=0;
int try_write_count=0;
int write_config_mesaj=5;
int tts_pause_count;
int probe_mesaj_tick=0;
int setting_nozzle_number;
unsigned authorize_count;
unsigned int tag_read_count;
unsigned int tag_suspend_count;
unsigned int tag_resume_count;
unsigned int int_amount;
unsigned int int_kdv;
unsigned int kdv_result;
unsigned int TS_no=0;
unsigned int USC_com_error_count=0;
unsigned int USC_ok_read_count;
unsigned int Tag_read_zero_count;
unsigned int authorize_count2;
unsigned int locktimer;
unsigned int keylocktimer;
unsigned int wait_km_count;
unsigned int watchdog_count;
long int daily_sold_volume;


quint8 Confirmation_flg;
quint8 Nozzle_up_flg;
quint8 Pump_Addr=0x50;
quint8 siteControler_Addr=0x50;
quint8 token;
quint8 Preset_Token;
quint8 request_flg=1;
quint8 siteControler_status;
quint8 tokenACK;
quint8 tokenX=0x00;
quint8 time_Random;
quint8 RF_Reader_ErrorCode;
quint8 clrc_ErrCode=0;
quint8 tca_ErrCode=0;
quint8 Number_of_Nozzle;
quint8 Sale_Error_code=0;
quint8 Payment_Error_code=0;
quint8 ZeroID_count;
quint8 Activation_Nozzle;
quint8 product_id[6];
quint8 tank_no[6];

char Filling_comp_flg;
char Filling_comp_flg_for_gprs;
char Enter_plate_flg;
char Tag_read_count_flg;
char buzzer_on_flg;
char Enter_flg;
char Clear_flg;
char Cancel_flg;
char Plate_control_flg;
char Sales_start;
char pool_sta_flg;
char USC_last_index;
char last_index_RFID;
char unit_price_dot_index;
char amount_dot_index;
char volume_dot_index;
char delete_last_sale_flg;
char break_message_count;
unsigned char Payload_type;
defination_pump pump[4];
defination_ECR ECR_par;
defination_Probe Probe[10];


quint8 USCtoken;
quint8 crc_msb;
quint8 crc_lsb;
quint8 USC_crc_lsb;
quint8 USC_crc_msb;
quint8 RFIDcrc_lsb;
quint8 RFIDcrc_msb;
quint8 crc_lsb_ind;
quint8 crc_msb_ind;
quint8 USC_crc_lsb_ind;
quint8 USC_crc_msb_ind;
quint8 pumpNo;
quint8 nozzleNo;
quint8 TotalnozzleNo;
quint8 temp_PNZdata;
quint8 ABU_addr;
quint8 PPN;
quint8 DCRP;
quint8 RNO;
quint8 BUZ;
quint8 DNO;
quint8 LNO;
quint8 KNO;
quint8 DEV;
quint8 DCR;
quint8 TMO;
quint8 Size;
quint8 Mode;
quint8 RST_Type;
quint8 SHF;
quint8 answer_flg;

int CD_message_LNG;

char last_index;

bool poll_receive_flg;

long Crypto_BUF[32];
long Crypto_BUF_meter[32];

QByteArray tempPNZ;
QByteArray SDAT_forECR; //
QByteArray REC_Number;
QByteArray DDAT;
QByteArray PASS;
QByteArray CMD_forBypassMode;
QString DCR_Date;
QString DCR_Time;

// ------------------------------- language --------------------------------------//
QString records_String;
QString clean_String;
QString cancel_String;
QString enter_String;
QString readyToSale_String;
QString enterPlate_String;
QString pleaseCheckPumpConnection_String;
QString pumpCommunicationError_String;
QString enterMenuPassword_String;
QString menu_String;
QString enterKilometer_String;
QString whitelist_String;
QString attendant_String;
QString sales_String;
QString recordedSettings_String;
QString back_String;
QString wrongPassword_String;
QString readAccesspoint_String;
QString writeAccesspoint_String;
QString visOpen_String;
QString deviceAdress_String;
QString pumpNozzleNumber_String;
QString cardReaderAdress_String;
QString askKm_String;
QString litreDecPoint_String;
QString amountDecPoint_String;
QString uPriceDecPoint_String;
QString save_String;
QString deviceSet_String;
QString receiptSet_String;
QString productSet_String;
QString testMode_String;
QString accesspoint_String;
QString printer_String;
QString cardReader_String;
QString startTest_String;
QString nozzle1Taken_String;
QString pleaseReadCard_String;
QString pleaseReadAttendantCard_String;
QString attendantCard_String;
QString customerCard_String;
QString visTag_String;
QString wrongCard_String;
QString blackList_String;
QString loyalty_String;
QString error_String;
QString pleaseReadDriverCard_String;
QString nozzleSettingsAreMissing_String;
QString waitingSettingsFromLocwiz_String;
QString automationErrorEnterPassword_String;
QString autoAdr01_String;
QString accesspointSettingsReadingSuccessful_String;
QString nozAdr01_String;
QString channel01_String;
QString stopZ08_String;
QString otoAdr_String;
QString nozzAdr_String;
QString channelNo_String;
QString stopZ_String;
QString nozzle_String;
QString menuPass_String;
QString manualAllowPassword_String;
QString updateSuccessful_String;
QString systemInitializing_String;
QString updateFailed_String;
QString noNewUpdatesFound_String;
QString updateDeviceNotFound_String;
QString saleToReady_String;
QString invalidLicence_String;
QString enterDateTime_String;
QString accesspointSettingsReadingFailed_String;
QString otoAdr1_String;
QString nozAdr1_String;
QString channel1_String;
QString stopZ1_String;
QString accesspointSettingsUploadSuccessful_String;
QString accesspointSettingsUploadFailed_String;
QString accesspointSettingsReadingPleaseWait_String;
QString settingsSaved_String;
QString systemWillBeRestarted_String;
QString deviceIsLocked_String;
QString uscActive_String;
QString deviceSettingsAreMissing_String;
QString systemLocked_String;
QString loadingAccesspointSettingsPleaseWait_String;
QString fieldMode_String;
QString activateLicence_String;
QString backUpToSdCard_String;
QString takeFromSdCard_String;
QString updateNow_String;
QString printReport_String;
QString serviceOpen_String;
QString serviceClosed_String;
QString keyboardClear_String;
QString keyboardSpace_String;
QString keyboardBack_String;
QString keyboardEnter_String;
QString taken_String;
QString invalidNozzle_String;
QString textPopup_String;
QString cancelPopup_String;
QString acceptPopup_String;
QString idChecking_String;
QString waitingPayment_String;
QString inadequateLimit_String;
QString unregisteredPlate_String;
QString waitingFilling_String;
QString filling_String;
QString wrongId_String;
QString wrongProduct_String;
QString passiveId_String;
QString deletedId_String;
QString fillingWasStopped_String;
QString enterPassword_String;
QString password_String;
QString fieldModeActive_String;
QString enterUnitPrice_String;
QString newUnitPrice_String;
QString unitPrice_String;
QString updated_String;
QString startingSystem_String;
QString printerPaperOut_String;
QString automationError_String;

QString incorrectDate_String;
QString incorrectTime_String;
QString date_String;
QString time_String;
QString setDateTime_String;
QString dateFormat_String;
QString systemStarting_String;
QString enterValidDate_String;
QString dateSetting_String;
QString networkSettings_String;
QString waiting_String;
QString kmControle_String;
QString pleaseEnterKm_String;
QString freePlate_String;
QString saleControl_String;
QString plateSale_String;
QString ipAdress_String;
QString macAdress_String;
QString version_String;

QByteArray idTaken_ByteArray;
QByteArray pleaseReadAttendantCard_ByteArray;
QByteArray checkingPlate_ByteArray;
QByteArray pleaseTakeANozzleAndReadAttendantCard_ByteArray;
QByteArray idControl_ByteArray;
QByteArray checkingCard_ByteArray;
QByteArray pleaseTakeANozzle_ByteArray;
QByteArray incorrectOperation_ByteArray;
QByteArray idError_ByteArray;
QByteArray fillingFailReadTag_ByteArray;


// ******************************lANGUAGE ****************************************** //


// **************************CHROMIUM SETTINS ************************************** //
QString browserStart;
QString keyboard;
QString kioskMode;
QString noNotifications;
QString touch;
QString fullScreen;
QString noErrorDialogs;
QString disableTranslate;
QString fastStart;
QString noInfoBar;
QString app;
QString appTo;
QString appToEmpty;
QString appToFull;
QString keyBlock;
QString incognito;
QString disableGpu;
QString disableD3d11;
QString disableLocalStorage;
QString disableOffnePages;
QString disablePluginPowerSaver;
QString disablePluginsDiscovery;
QString disableSync;
QString disableWebgl;
QString noExperiments;
QString disableSuggestionsService;
QString disableSavePasswordBuble;
QString disableAppcontainer;
QProcess process;

QDate updatedDate;
// **************************CHROMIUM SETTINS ************************************** //
QString logTime()
{
    QDateTime dateTime=QDateTime::currentDateTime();
    QString time=dateTime.toString("hh:mm:ss:zzz");
    return time;
}
QString logDate()
{
    QDateTime dateTime=QDateTime::currentDateTime();
    QString date=dateTime.toString("yyyy-MM-dd");
    logDatem=date;
    return date;
}



///////convert////////////////////////////////
QByteArray convert_format(QByteArray data,unsigned char dot_ind)
{
    if(test)qDebug()<<"DATAA"<<data<<dot_ind;
    u_int8_t temp,i;
    QByteArray temp_array;
    temp_array=data;
    
    if(data.contains(",")==true)
    {
        if(dot_ind!=0)
        {
            if(dot_ind>=(data.length()-data.lastIndexOf(",")-1))
            {
                temp=dot_ind-(data.length()-data.lastIndexOf(",")-1);
                for(i=0;i<temp;i++)
                {
                    data.append("0");
                    
                }
            }
            else
            {
                data.remove(data.lastIndexOf(",")+dot_ind+1,(data.length()-data.lastIndexOf(",")-1)-dot_ind);
            }
            data_with_dot=data;
            data.remove(data.lastIndexOf(","),1);
        }
        
    }
    else if(data.contains(".")==true)
    {
        if(dot_ind!=0)
        {
            if(test)qDebug()<<"selam : "<<QString::number(dot_ind);
            if(dot_ind>=(data.length()-data.lastIndexOf(".")-1))
            {

                temp=dot_ind-(data.length()-data.lastIndexOf(".")-1);
                for(i=0;i<temp;i++)
                {
                    data.append("0");
                }
            }
            else
            {
                data.remove(data.lastIndexOf(".")+dot_ind+1,(data.length()-data.lastIndexOf(".")-1)-dot_ind);
            }
            data_with_dot=data;
            data.remove(data.lastIndexOf("."),1);
        }
        
    }
    else
    {
        for(i=0;i<dot_ind;i++)
        {
            data.append("0");
        }
        //data_with_dot=data.insert((data.length()-dot_ind),".");
    }
    
    return data;
}


//_______________________________________dec_to_bcd_____________________________________
int dec_to_bcd(int dec)
{ unsigned dec_100000,dec_10000,dec_1000,dec_100;

    stringBCD.clear();
    Stringbcd[0].clear();
    Stringbcd[1].clear();
    Stringbcd[2].clear();
    Stringbcd[3].clear();

    bcd[0]=0; bcd[1]=0; bcd[2]=0; bcd[3]=0;

    if(dec<100){bcd[0]=(((dec/10)<<4)+(dec%10)); }

    else if ((dec>=100)& (dec<1000)){bcd[1]=dec/100;  dec_100=dec%100;    bcd[0]=(((dec_100/10)<<4)+(dec_100%10)); }

    else if ((dec>=1000)& (dec<10000)){dec_100=dec/100;  bcd[1]=(((dec_100/10)<<4)+(dec_100%10));  dec_100=dec%100;    bcd[0]=(((dec_100/10)<<4)+(dec_100%10)); }

    else if ((dec>=10000)& (dec<1000000))
    {
        dec_1000=dec/10000;   bcd[2]=(((dec_1000/10)<<4)+(dec_1000%10));

        dec_1000=((dec%10000)/100);      bcd[1]=(((dec_1000/10)<<4)+(dec_1000%10));

        dec_100=dec%100;                 bcd[0]=(((dec_100/10)<<4)+(dec_100%10));


    }

    else if ((dec>=1000000)& (dec<100000000))
    {
        dec_100000=dec/1000000;   bcd[3]=(((dec_100000/10)<<4)+(dec_100000%10));

        dec_10000=((dec%1000000)/10000);      bcd[2]=(((dec_10000/10)<<4)+(dec_10000%10));

        dec_1000=((dec%10000)/100);      bcd[1]=(((dec_1000/10)<<4)+(dec_1000%10));

        dec_100=dec%100;                 bcd[0]=(((dec_100/10)<<4)+(dec_100%10));

    }

}


void saveUnitPrice(quint8 NZL)
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    if(Unit_price_for_save[NZL].contains(",")==true)
    {
        Unit_price_for_save[NZL].replace(Unit_price_for_save[NZL].lastIndexOf(","),1,".");
    }
    settings.setValue("Unit_price_"+QString::number(NZL),QString::fromUtf8(Unit_price_for_save[NZL]));
    sync();
    system("sync");

    if(test)qDebug()<<"SAVED UNiT PRiCE"<<Unit_price_for_save[NZL].toHex();
    settings.setlog("SATIS_DEBUG SAVED UNiT PRiCE "+QString::fromUtf8(Unit_price_for_save[NZL].toHex()));
}


void saveVolumeTotal(quint8 PMPN,quint8 NZL)
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("Begin_total_volume_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].BT_Volume.toHex());
    settings.setValue("End_total_volume_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].T_Volume.toHex());
    settings.setValue("End_total_volume_USC_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].USC_T_Volume.toHex());
    sync();
    system("sync");
}
void saveAmountTotal(quint8 PMPN , quint8 NZL)
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("Begin_total_amount_"+QString::number(NZL),pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].BT_Amount.toHex());
    settings.setValue("End_total_amount_"+QString::number(NZL),pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Amount.toHex());
    settings.setValue("End_total_amount_USC_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].USC_T_Amount.toHex());
    sync();
    system("sync");
}
void saveFilling(quint8 PMPN , quint8 NZL)
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("filling_price_for_USC_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].filling_price.toHex());
    settings.setValue("filling_volume_for_USC_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].filling_volume.toHex());
    settings.setValue("filling_amount_for_USC_"+QString::number(NZL),pump[pumpNo].nozzle[NZL].filling_amount.toHex());
    sync();
    system("sync");
}

void saveTsNo()
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("TS_no",TS_no);
    sync();
    system("sync");
}


