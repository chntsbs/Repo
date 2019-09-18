#ifndef GLOBAL_H
#define GLOBAL_H

#include<QString>
#include <QCoreApplication>
#include<QtCore>
#include <QByteArray>
#include <QSql>
#include "QtSql"
#include "crypto.h"

extern Crypto CryptoObj;

extern QThread *LCDThread;
extern QTimer *LCDtimer1;
extern QTimer *buzzertimer;
extern QDate updatedDate;
extern QSqlDatabase constructor_db;
extern QSqlDatabase get_price_db;
extern QSqlDatabase nozzle_db;
extern QSqlDatabase nozzle_product_id_db;
extern QSqlDatabase nozzle_values_db;
extern QSqlDatabase nozzle_product_price_db;
extern QSqlDatabase nozzle_total_amount_db;
extern QSqlDatabase nozzle_total_volume_db;
extern QSqlDatabase nozzle_total_sales_db;
extern QSqlDatabase count_id_db;
extern QSqlDatabase update_nozzle_product_db;
extern QSqlDatabase update_nozzle_price_db;
extern QSqlDatabase update_nozzle_total_db;
extern QSqlDatabase insert_sales_db;
extern QSqlDatabase insert_probe_db;
extern QSqlDatabase update_probe_db;
extern QSqlDatabase delete_table_db;
extern QSqlDatabase sales_date_db;
extern QSqlDatabase sales_status_db;
extern QSqlDatabase update_status_db;
extern QSqlDatabase update_dateclock_db;
extern QSqlDatabase update_sales_db;
extern QSqlDatabase update_plate_db;
extern QSqlDatabase total_sales_db;
extern QSqlDatabase whitelist_db;
extern QSqlDatabase plates_db;
extern QSqlDatabase sales_plate_db;
extern QSqlDatabase sales_id_db;
extern QSqlDatabase controlsales_id_db;
extern QSqlDatabase person_db;
extern QSqlDatabase control_person_db;
extern QSqlDatabase show_person_db;
extern QSqlDatabase show_nozzle_db;
extern QSqlDatabase show_whitelist_db;
extern QSqlDatabase show_sales_db;
extern QSqlDatabase tankLevel_db;


// ---------------------------- Card Reader RFID Variables ----------------------------- //
#define ReadCard 0x01
#define PersonCard 0x02
#define CustomerCard 0x03
#define VehicleCard 0x04
#define WrongCard 0x05
#define Blacklist 0x06
#define Stawiz 0x07
#define Error 0x08
#define MainScreen 0x09
#define LoyalytCard 0x10



extern quint8 request_flg_RFID;
extern quint8 NozzleRFID_fromMaster;
extern quint8 DummyRFID_fromMaster;
extern quint8 StoredScreenRFID_fromMaster;
extern quint8 SCR_X_fromMaster;
extern quint8 SCR_Y_fromMaster;
extern quint8 Text_With_fromMaster;
extern quint8 Pixel_OnOff_fromMaster;
extern quint8 NewPlateExist;
extern quint8 ABU_address;

extern QByteArray CardReader_Addr;
extern QByteArray RFID_RandomKey;
extern QByteArray TextLen_fromMaster;
extern QByteArray TEXT_fromMaster; // error line
extern QByteArray TAGID[5];
//----------------------------------------------------------------------------------------


extern bool pumper_flg;
extern bool new_configuration;
extern bool printer_paper;
extern bool IETT;
extern bool upc_locvis;
extern bool update_debug_flg;
extern bool check_server;
extern bool serial_debug_flg;
extern bool meter_debug_flg;
extern bool RFID_debug_flg;
extern bool printer_debug_flg;
extern bool clrc_debug_flg;
extern bool suspend_first;
extern bool gprs_debug;
extern bool debug_flg;
extern bool updatedatabase;
extern bool LCD_refresh_flg;
extern bool Pump_Filling_status;
extern bool Sale_suspend_flg;
extern bool Sale_resume_flg;
extern bool online_flg;
extern bool meter_last_error;
extern bool clrc_check_flg;
extern bool keypad_check_flg;
extern bool lcd_check_flg;
extern bool ack_receive_flg;
extern bool eot_receive_flg;
extern bool nak_receive_flg;
extern bool data_receive_flg;
extern bool dcr_ini_flg;
extern bool tag_Random_flg;
extern bool authorize_flg;
extern bool meter_com_err;
extern bool only_card_read_flg;
extern bool plate_ini_flg;
extern bool card_kontrol;
extern bool checkinternet;
extern bool ok;
extern bool Allow_card_read ;
extern bool Card_reader_chose;
extern bool Preset_Control_flg;
extern bool Send_authorize_flg;
extern bool Send_authorize_flg_2;
extern bool Preset_Ack_flg;
extern bool Update_flg;
extern bool ask_nozzle_total_volume;
extern bool ask_nozzle_total_amount;
extern bool End_total_report;
extern bool update_message_sent;
extern bool ask_last_screen;
extern bool bater_low_flg;
extern bool pump_sta;
extern bool reboot_ok;
extern bool ask_km;
extern bool askSellingStyle;
extern bool askPlateSale;
extern bool next;
extern bool plateDB;
extern bool kmConfirm_flg;
extern bool customerCard;
extern bool customerCardLock;
extern bool customerCardRead;
extern bool ask_km_after;
extern bool serbestMusteri;
extern bool update_isImmediate_flg; // güncelleme acil mi ?
extern bool update_download_flg; // Güncelleme indiriliyor mu?
extern bool givecopyflg;
extern bool SuspendReply;
extern bool ResumeReply;
extern bool USC_is_exist;
extern bool Paid_Confirm;
extern bool Change_status_to_IDLE;
extern bool Send_Card_reader;
extern bool TAG_read;
extern bool card_authorize;
extern bool Not_resume_TAG;
extern bool Wrong_trans;
extern bool Wrong_trans_for_save;
extern bool TAG_KM_read;
extern bool save_TS_Time_flg;
extern bool update_whitelist;
extern bool update_person;
extern bool limit_error;
extern bool USC_com_error_flg;
extern bool ttspause;
extern bool Tag_read_flg;
extern bool KM_Confirmation_flg;
extern bool Driver_wait_flg;
extern bool Person_wait_flg;
extern bool auttype_3_4;
extern bool Plate_screen_flg;
extern bool plate_entered_with_button;
extern bool allow_button_plate;
extern bool Card_read_flg;
extern bool Volumetotal_received;
extern bool Amounttotal_received;
extern bool Confirmation_flg_ok;
extern bool system_started;
extern bool Person_is_Ok;
extern bool virtual_pump;
extern bool kontrol_id;
extern bool Confirmation_flg_without_km;
extern bool test;
extern bool config_error;
extern bool configuration;
extern bool protect_flg;
extern bool system_lock;
extern bool activation_flg;
extern bool connection_is_exist;
extern bool cannotchange_frame;
extern bool test_modu;
extern bool printer_check_flg;
extern bool rfid_check_flg;
extern bool print_test_mode;
extern bool USC_com_lock;
extern bool clrc_up_flg;
extern bool buzzer;
extern bool wait_km_flg;
extern bool saat_tarih_error;
extern bool password_is_deactive;
extern bool pump_sta_2;
extern bool saving_the_sales;
extern bool routinedebug;
extern bool read_configuration;
extern bool write_configuration;
extern bool write_configuration_active;
extern bool nozzle_configuration;
extern bool probe_configuration;
extern bool setting_configuration;
extern bool config_kontrol;
extern bool wrong_product;
extern bool passive_id;
extern bool deleted_id;
extern bool odometre_active;
extern bool odometre; //info alındı.
extern int immediatelyUpdateBalanceLimit;// 0 ile başlıyor, 1 ile satış varken girildi , 2 iken satış kaydedildi artık işlem yapılabilir.


extern QString Liter_preset;
extern QString table_name;
extern QString CustomerID;
extern QString previous_CustomerID;
extern QString Temp_CustomerID;
extern QString Keypad_data;
extern QString platem;
extern QString printing_header_data[4];
extern QString fisdate;
extern QString fisclock;
extern QString Product_name;
extern QString kdvoran; // KDV oranı
extern QString type;
extern QString End_info[3];
extern QString SettingsFile;
extern QString Amount_for_Database;
extern QString Volume_for_Database;
extern QString Unitprice_for_Database;
extern QString Float_setting;
extern QString Metre;
extern QString vehiclekmtr;
extern QString savePath;
extern QString savePath2;
extern QString targetPath;
extern QString targetPath2;
extern QString sale_type;
extern QString Myversion;
extern QString MyVersionDate;
extern QString downloadIniFile;
extern QString downloadExFile;
extern QString saveIniFile;
extern QString currentPath;
extern QString myIniFile;
extern QString zipFile;
extern QString checkFile;
extern QString checkFile2;
extern QString read_iniUpdate; // version.txt den okunan ini file versiyonu
extern QString licence;
extern QString macAddress;
extern QString server;
extern QString imei;
extern QString databasePath;
extern QString logDatem;
extern QString Static_ip;
extern QString sifre;
extern QString Psifre;
extern QString check_dowloadIniFile;
extern QString settingPath;
extern QString gateWay;
extern QString totalLT;
extern QString saved_day;
extern QString TS_date;
extern QString TS_time;
extern QString Nozzle_Up_Time;
extern QString Nozzle_Down_Time;
extern QString RFID_Read_Time;
extern QString RFID_Result_Time;
extern QString Authorize_Time;
extern QString Filling_Start_Time;
extern QString Paid_Confrim_Time;
extern QString Paid_Fiscal_Time;
extern QString commokpswrd;
extern QString otopswrd;
extern QString priceuppswrd;
extern QString SalesID;
extern QString Saved_Customer_ID;
extern QString Saved_Driver_ID;
extern QString Saved_Person_ID;
extern QString Temp_ID[5]; //1=MTS/TTS id 2=Driver 3=Person
extern QString Version_config_ini;
extern QString StatusLine;
extern QString ErrorLine;
extern QString version;
extern QString usc_ip;
extern QString usc_port;
extern QString read_acc_auto_adr;
extern QString read_acc_nozzle_adr;
extern QString read_acc_channel;
extern QString read_acc_wait_time;
extern QString new_unit_price[6];
extern QString odometre_KM;
extern QString odometre_time;
extern QString balanceLimit;
extern QString customerPlate;
extern QString dataTime;

extern QString idChecking_String;
extern QString waitingPayment_String;
extern QString inadequateLimit_String;
extern QString unregisteredPlate_String;
extern QString waitingFilling_String;
extern QString filling_String;
extern QString wrongId_String;
extern QString wrongProduct_String;
extern QString passiveId_String;
extern QString deletedId_String;
extern QString fillingWasStopped_String;
extern QString enterPassword_String;
extern QString password_String;
extern QString fieldModeActive_String;
extern QString enterUnitPrice_String;
extern QString newUnitPrice_String;
extern QString unitPrice_String;
extern QString updated_String;
extern QString startingSystem_String;
extern QString printerPaperOut_String;
extern QString automationError_String;
extern QString textPopup_String;
extern QString cancelPopup_String;
extern QString acceptPopup_String;
extern QString freePlate_String;
extern QString saleControl_String;
extern QString plateSale_String;

// **************************CHROMIUM SETTINS ************************************** //
extern QString incognito;
extern QString disableGpu;
extern QString disableD3d11;
extern QString disableLocalStorage;
extern QString disableOffnePages;
extern QString disablePluginPowerSaver;
extern QString disablePluginsDiscovery;
extern QString disableSync;
extern QString disableWebgl;
extern QString noExperiments;
extern QString disableSuggestionsService;
extern QString disableSavePasswordBuble;
extern QString disableAppcontainer;
extern QString browserStart;
extern QString keyboard;
extern QString kioskMode;
extern QString noNotifications;
extern QString touch;
extern QString fullScreen;
extern QString noErrorDialogs;
extern QString disableTranslate;
extern QString fastStart;
extern QString noInfoBar;
extern QString app;
extern QString appTo;
extern QString appToEmpty;
extern QString appToFull;
extern QString keyBlock;
extern QProcess process;
// **************************CHROMIUM SETTINS ************************************** //

extern QByteArray line1;
extern QByteArray line2;
extern QByteArray line3;
extern QByteArray line4;
extern QByteArray stringBCD;
extern QByteArray Stringbcd[5];
extern QByteArray data_with_dot;
extern QByteArray Licence_plate_code;
extern QByteArray Customer_plate;
extern QByteArray Keypad_data_for_LCD;
extern QByteArray Receipt_customer_plate;
extern QByteArray dcr_pool;
extern QByteArray dcr_ini;
extern QByteArray dcr_ini2;
extern QByteArray priceArray;
extern QByteArray ReadTempData;
extern QByteArray ReadTempData2;
extern QByteArray ReadTempData_RFID;
extern QByteArray tempBuffer;
extern QByteArray RandomBuffer;
extern QByteArray Decoding_Key;
extern QByteArray RandomArray;
extern QByteArray ReadTAG;
extern QByteArray temp_Amount;
extern QByteArray temp_Volume;
extern QByteArray saved_unit_price;
extern QByteArray error_gprs;
extern QByteArray SendPrintData;
extern QByteArray kdvttr;
extern QByteArray kdv_temp_buff;
extern QByteArray SendPrintData;
extern QByteArray SendPrintDataCOPY;
extern QByteArray Unit_price;
extern QByteArray Unit_price_for_save[5];
extern QByteArray printer_temp_buff;
extern QByteArray printer_temp_buff_for_database;
extern QByteArray space_buffer;
extern QByteArray ReadUSCData;
extern QByteArray ReadPrintData;
extern QByteArray ReadPrintTempData;
extern QByteArray Lastprice[5];
extern QByteArray MACaddr;
extern QByteArray serial_No;
extern QByteArray Temp_TAGID;
extern QByteArray Coding_key;
extern QByteArray acc_otomasyon_adresi;
extern QByteArray acc_nozzle_adresi;
extern QByteArray acc_channel_no_adresi;
extern QByteArray acc_pause_time;
extern QByteArray probe_address[];
extern QByteArray ReadData_probe;

extern int tempcard_reader_address;
extern int probe_mesaj_tick;
extern int probe_address_select;
extern int select_version;
extern int probe_count;
extern int keyboardclear;
extern int bcd[5];
extern int LCD_status;
extern int lock_status;
extern int LCD_status_last;
extern int LCD_status_last2;
extern int number_of_salessave;
extern int last_screen_flg;
extern int reboot_control_flg;
extern int tik_count;
extern int counttimer;
extern int count;
extern int  ZetNO;
extern int vardiyano;
extern int Product_KDV;
extern int fisno;
extern int update_check_flg; // Güncelleme var mı?
extern int timeoutTime;
extern int send_meter_data_count;
extern int screen_UP_nozzle;
extern int ID_count;
extern int CLRC_ID_Count;
extern int total_nozzle_number;
extern int virtual_pumpno;
extern int sales_send_count;
extern int plate_enter_count;
extern int pump_lock_count;
extern int lock_day;
extern int getverison_count;
extern int USC_com_count;
extern int Sale_suspend_count;
extern int updateisstarting;
extern int licenceCheck;
extern int wait_km_second;
extern int system_shutdown_count;
extern int read_configuration_success;
extern int write_configuration_success;
extern int try_read_count;
extern int try_write_count;
extern int write_config_mesaj;
extern int tts_pause_count;
extern int setting_nozzle_number;
extern unsigned authorize_count;
extern unsigned authorize_count2;
extern unsigned int int_amount;
extern unsigned int int_kdv;
extern unsigned int kdv_result;
extern unsigned int tag_read_count;
extern unsigned int Tag_read_zero_count;
extern unsigned int USC_ok_read_count;
extern unsigned int tag_suspend_count;
extern unsigned int tag_resume_count;
extern unsigned int TS_no;
extern unsigned int USC_com_error_count;
extern unsigned int locktimer;
extern unsigned int keylocktimer;
extern unsigned int wait_km_count;
extern unsigned int watchdog_count;

extern long int daily_sold_volume;


extern quint8 clrc_ErrCode;
extern quint8 tca_ErrCode;
extern quint8 token;
extern quint8 Preset_Token;
extern quint8 Pump_Addr;
extern quint8 siteControler_Addr;
extern quint8 siteControler_status;
extern quint8 request_flg;
extern quint8 tokenACK;
extern quint8 randomToken;
extern quint8 tokenX;
extern quint8 time_Random;
extern quint8 RF_Reader_ErrorCode;
extern quint8 Confirmation_flg;
extern quint8 Nozzle_up_flg;
extern quint8 Number_of_Nozzle;
extern quint8 ZeroID_count;
extern quint8 Activation_Nozzle;
extern quint8 cash_register_pump_no;
extern quint8 product_id[6];
extern quint8 tank_no[6];

extern char break_message_count;
extern char Filling_comp_flg;
extern char Filling_comp_flg_for_gprs;
extern char Tag_read_count_flg;
extern char buzzer_on_flg;
extern char Enter_plate_flg;
extern char Enter_flg;
extern char Cancel_flg;
extern char Clear_flg;
extern char Plate_control_flg;
extern char Sales_start;
extern char pool_sta_flg;
extern char USC_last_index;
extern char last_index_RFID;
extern char unit_price_dot_index;
extern char amount_dot_index;
extern char volume_dot_index;
extern char delete_last_sale_flg;
extern unsigned char Payload_type;

extern long Crypto_BUF[32];
extern long Crypto_BUF_meter[32];
//____________________________Site Controler_Variables_____________________________

#define ctrl_pool       0x20
#define ctrl_data       0x30
#define ctrl__iap       0x40
#define ctrl_nak        0x50
#define ctrl_ack        0xC0
#define ctrl_ackpool    0xE0

#define TNO_CD1         0x01    //Command to pump
#define TNO_CD2         0x02    //Allowed Nozzle Number
#define TNO_CD3         0x03    //Preset Volume
#define TNO_CD4         0x04    //Preset Amount
#define TNO_CD5         0x05    //Price Update
#define TNO_CD6         0x06    //Reserved
#define TNO_CD7         0x07    // - " -
#define TNO_CD8         0x08    // - " -
#define TNO_CD9         0x09    //Set Pump Parameters
#define TNO_CD          0x0A    // Reserved
#define TNO_CD11        0x0B    // - " -
#define TNO_CD12        0x0C    // - " -
#define TNO_CD13        0x0D    //Set Filling Type
#define TNO_CD14        0x0E    //Suspend Request
#define TNO_CD15        0x0F    //Resume Request
#define TNO_CD101       0x65    //Request Volume Total Counters
#define TNO_CD102       0x66    //Request Amount Total Counters
#define TNO_CD103       0x67    //Request Temperatures
#define TNO_CD104       0x68    //Request RF-Tag Data

#define ETX             0x03    //End of text
#define SF              0xFA    //Stop Flag

//______________CD1 DCC Variables __________________

#define CD1_DCC0        0x00    //Return Status
#define CD1_DCC2        0x02    //Return Pump Parameters
#define CD1_DCC3        0x03    //Return Pump Identity
#define CD1_DCC4        0x04    //Return Filling Information
#define CD1_DCC5        0x05    //Reset
#define CD1_DCC6        0x06    //Autorize
#define CD1_DCC8        0x08    //Stop
#define CD1_DCC10       0x0A    //Switch Off

//____________Card Reader General Variables______________________//

#define ctrl_CMD1       0x52    // Read Tag ID
#define ctrl_CMD2       0x43    // Clear Screen
#define ctrl_CMD3       0x49    // Show Stored Screen
#define ctrl_CMD4       0x54    // Write text to LCD

#define STX             0x02    // Start of Text




//________________General_DCR_Parametres______________
typedef struct DCR_parameters{
    quint8 DCR_lenght;
    quint8 DCR_status;
    quint8 DCR_old_status;
    quint8 DCR_Alarm_Code;
    quint8 DCR_DPVol;
    quint8 DCR_DPAmount;
    quint8 DCR_DPunp;
    quint8 DCR_Nozzle_Number;
    quint8 Active_Nozzle;
    quint8 DCR_Count;
    quint8 DCR_Nozzle_Info;

    QByteArray DCR_TotVol;
    QByteArray DCR_TotV1;
    QByteArray DCR_TotV2;
    QByteArray DCR_volume;
    QByteArray DCR_amount;
    QByteArray DCR_Filling_Price;
    QByteArray DCR_Res1;
    QByteArray DCR_Res2;
    QByteArray DCR_Res3;
    QByteArray DCR_Mamo;
    QByteArray DCR_Grade;
    QByteArray DCR_PID;
    QByteArray DCR_TotAmo;
    QByteArray DCR_TotA1;
    QByteArray DCR_TotA2;
    QByteArray DCR_Temp;
    QByteArray DCR_RFTag;


}DCRpar;

typedef union Delta_t{
    long LongInt;
    unsigned long LongWrd;
    unsigned char Byte[4];
}Delta;

extern DCRpar DCR_par;
extern Delta Delta_m;

extern Delta Delta_meter;
extern quint8 USCtoken;
extern quint8 crc_msb;
extern quint8 USC_crc_lsb;
extern quint8 USC_crc_msb;
extern quint8 RFIDcrc_lsb;
extern quint8 RFIDcrc_msb;
extern quint8 crc_lsb;
extern quint8 crc_lsb_ind;
extern quint8 crc_msb_ind;
extern quint8 USC_crc_lsb_ind;
extern quint8 USC_crc_msb_ind;
extern quint8 pumpNo;
extern quint8 nozzleNo;
extern quint8 TotalnozzleNo;
extern quint8 temp_PNZdata;
extern quint8 ABU_addr;
extern quint8 PPN; // Parameter Page No 0=All
extern quint8 DCRP; // DCR Parameter 0->>No Fiscal, 1->>48 Hours, 2->> Fiscal Mode
extern quint8 RNO; // Reader No
extern quint8 BUZ; // Buzzer 0->> Off , nn=beep duratcion
extern quint8 DNO; // Display Number
extern quint8 LNO; // Line Number
extern quint8 KNO; // Keypad Number
extern quint8 DEV; // Device Number // 00->>DCR, 01->>Pump 1, 02->>Pump 2, 03->>Pump 3, 04->>Pump 4, 05->>ECR,
extern quint8 DCR; // DCR Number
extern quint8 TMO; // SCU TimeOut 10 to 1000 second
extern quint8 Mode; // Pump Mode 01->> Entry,  00->>Exit
extern quint8 Size;
extern quint8 RST_Type; // reset type 0->> Software Reset , 1->> Hardware Reset
extern quint8 SHF; // 0->> Read Shift Totals, 1->> Set Shift Totals
extern quint8 answer_flg;
extern quint8 Sale_Error_code;
extern quint8 Payment_Error_code;
extern int CD_message_LNG;

extern char last_index;

extern bool poll_receive_flg;


extern QByteArray tempPNZ;
extern QByteArray SDAT_forECR;
extern QByteArray REC_Number;
extern QByteArray DDAT; // Display Data
extern QByteArray PASS; // Password
extern QByteArray CMD_forBypassMode;
extern QString DCR_Date;
extern QString DCR_Time;

// --------------------- Control Bytes -----------------------------//
#define ctrl_poll                          0x20
#define ctrl_data                          0x30
#define ctrl_iap                           0x40
#define ctrl_nak                           0x50
#define ctrl_ack                           0xC0
#define ctrl_eot                           0x70
#define ctrl_AckPoll                       0xE0

#define ETX                                0x03    // End of Text
#define SF                                 0xFA    // Stop Flag
#define DLE                                0x10    // Data Link Escape


//---------------------- Slave Controls Variables --------------------//

#define TNO_DC127                          0x7F    //Reserced
#define TNO_DC128                          0x80    //Pump Status
#define TNO_DC129                          0x81    //Reserved
#define TNO_DC130                          0x82    //Reserced
#define TNO_DC131                          0x83    //End of Fiiling Report
#define TNO_DC132                          0x84    //Reserved
#define TNO_DC133                          0x85    //ECR Mifare Data
#define TNO_DC134                          0x86    //Data Transfer from ECR
#define TNO_DC135                          0x87    //Nozzle Totalizer
#define TNO_DC136                          0x88    //Reserved
#define TNO_DC137                          0x89    //DCR Parameters
#define TNO_DC138                          0x8A    //Reserved
#define TNO_DC139                          0x8B    //Sale Record
#define TNO_DC140                          0x8C    //Reserved
#define TNO_DC141                          0x8D    //VID Status
#define TNO_DC142                          0x8E    //VID Data
#define TNO_DC143                          0x8F    //CID Status
#define TNO_DC144                          0x90    //CID Data
#define TNO_DC145                          0x91    //Reserved
#define TNO_DC146                          0x92    //Keypad Buffer
#define TNO_DC147                          0x93    //Error Event Report
#define TNO_DC148                          0x94    //DCR Status
#define TNO_DC149                          0x95    //Software Update Status
#define TNO_DC150                          0x96    //Reserved
#define TNO_DC151                          0x97    //ECR Plate
#define TNO_DC152                          0x98    //Request Slip Data
#define TNO_DC153                          0x99    //PPU Data
#define TNO_DC154                          0x9A    //Reserved
#define TNO_DC155                          0x9B    //Reserved
#define TNO_DC156                          0x9C    //Keypad Status
#define TNO_DC157                          0x9D    //Shift Totalizer
#define TNO_DCE2                           0xE2    //plate IETT
#define TNO_DCE0                           0xE0    //Enf-of-filling IETT
// ------------------------Status Byte Variables ---------------------------//

#define IDLE_sta                           0x00
#define CALL_sta                           0x01
#define BUSY_sta                           0x02
#define PAUSED_sta                         0x03
#define UNPAID_sta                         0x04
#define ERROR_sta                          0x05
#define AUTHORIZED_sta                     0x06
#define WAIT_sta                           0x07
#define PAID_sta                           0x08
#define KYB_CALL_sta                       0x09
#define INACTIVE_sta                       0x0A

//------------------------- Filling Type ----------------------------------------//

#define FTYP_Cash                          0x00
#define FTYP_TTS                           0x01
#define FTYP_MTS                           0x02
#define FTYP_Barrel                        0x03
#define FTYP_Transfer                      0x04
#define FTYP_Test                          0x05
#define FTYP_Manuel                        0x06
#define FTYP_Undefined                     0x07
#define FTYP_CreditCar                     0x08
#define FTYP_Loyalyt                       0x09



//------------------------- Authorize Type ----------------------------------------//

#define ATYP_Cash                          0x00
#define ATYP_TTS                           0x01
#define ATYP_MTS                           0x02
#define ATYP_Barrel                        0x03
#define ATYP_Transfer                      0x04
#define ATYP_Test                          0x05
#define ATYP_Manuel                        0x06
#define ATYP_Unauthorize                   0x07
#define ATYP_CreditCar                     0x08
#define ATYP_Loyalyt                       0x09



// -------------------------------Define Pump & Nozzle ---------------------------//

// ------------------------------- language --------------------------------------//
extern QString records_String;
extern QString clean_String;
extern QString cancel_String;
extern QString enter_String;
extern QString readyToSale_String;
extern QString enterPlate_String;
extern QString pleaseCheckPumpConnection_String;
extern QString pumpCommunicationError_String;
extern QString enterMenuPassword_String;
extern QString menu_String;
extern QString enterKilometer_String;
extern QString whitelist_String;
extern QString attendant_String;
extern QString sales_String;
extern QString recordedSettings_String;
extern QString back_String;
extern QString wrongPassword_String;
extern QString readAccesspoint_String;
extern QString writeAccesspoint_String;
extern QString visOpen_String;
extern QString deviceAdress_String;
extern QString pumpNozzleNumber_String;
extern QString cardReaderAdress_String;
extern QString askKm_String;
extern QString litreDecPoint_String;
extern QString amountDecPoint_String;
extern QString uPriceDecPoint_String;
extern QString save_String;
extern QString deviceSet_String;
extern QString receiptSet_String;
extern QString productSet_String;
extern QString testMode_String;
extern QString accesspoint_String;
extern QString printer_String;
extern QString cardReader_String;
extern QString startTest_String;
extern QString nozzle1Taken_String;
extern QString pleaseReadCard_String;
extern QString pleaseReadAttendantCard_String;
extern QString attendantCard_String;
extern QString customerCard_String;
extern QString visTag_String;
extern QString wrongCard_String;
extern QString blackList_String;
extern QString loyalty_String;
extern QString error_String;
extern QString pleaseReadDriverCard_String;
extern QString nozzleSettingsAreMissing_String;
extern QString waitingSettingsFromLocwiz_String;
extern QString automationErrorEnterPassword_String;
extern QString autoAdr01_String;
extern QString accesspointSettingsReadingSuccessful_String;
extern QString nozAdr01_String;
extern QString channel01_String;
extern QString stopZ08_String;
extern QString otoAdr_String;
extern QString nozzAdr_String;
extern QString channelNo_String;
extern QString stopZ_String;
extern QString nozzle_String;
extern QString menuPass_String;
extern QString manualAllowPassword_String;
extern QString updateSuccessful_String;
extern QString systemInitializing_String;
extern QString updateFailed_String;
extern QString noNewUpdatesFound_String;
extern QString updateDeviceNotFound_String;
extern QString saleToReady_String;
extern QString invalidLicence_String;
extern QString enterDateTime_String;
extern QString accesspointSettingsReadingFailed_String;
extern QString otoAdr1_String;
extern QString nozAdr1_String;
extern QString channel1_String;
extern QString stopZ1_String;
extern QString accesspointSettingsUploadSuccessful_String;
extern QString accesspointSettingsUploadFailed_String;
extern QString accesspointSettingsReadingPleaseWait_String;
extern QString settingsSaved_String;
extern QString systemWillBeRestarted_String;
extern QString deviceIsLocked_String;
extern QString uscActive_String;
extern QString deviceSettingsAreMissing_String;
extern QString systemLocked_String;
extern QString loadingAccesspointSettingsPleaseWait_String;
extern QString fieldMode_String;
extern QString activateLicence_String;
extern QString backUpToSdCard_String;
extern QString takeFromSdCard_String;
extern QString updateNow_String;
extern QString printReport_String;
extern QString serviceOpen_String;
extern QString serviceClosed_String;
extern QString keyboardClear_String;
extern QString keyboardSpace_String;
extern QString keyboardBack_String;
extern QString keyboardEnter_String;
extern QString taken_String;
extern QString invalidNozzle_String;

extern QString incorrectDate_String;
extern QString incorrectTime_String;
extern QString date_String;
extern QString time_String;
extern QString setDateTime_String;
extern QString dateFormat_String;
extern QString systemStarting_String;
extern QString enterValidDate_String;
extern QString dateSetting_String;
extern QString networkSettings_String;
extern QString waiting_String;
extern QString kmControle_String;
extern QString pleaseEnterKm_String;
extern QString ipAdress_String;
extern QString macAdress_String;
extern QString version_String;


extern QByteArray idTaken_ByteArray;
extern QByteArray pleaseReadAttendantCard_ByteArray;
extern QByteArray checkingPlate_ByteArray;
extern QByteArray pleaseTakeANozzleAndReadAttendantCard_ByteArray;
extern QByteArray idControl_ByteArray;
extern QByteArray checkingCard_ByteArray;
extern QByteArray pleaseTakeANozzle_ByteArray;
extern QByteArray incorrectOperation_ByteArray;
extern QByteArray idError_ByteArray;
extern QByteArray fillingFailReadTag_ByteArray;
// ******************************lANGUAGE ****************************************** //

typedef struct pump_parameters
{
    quint8 max_nozzle_number;
    quint8 sideController_Addr;
    quint8 messageLNG;
    quint8 status;
    quint8 laststatus;
    quint8 PrdctNozzleNo[10];
    QByteArray          Fiscal_ID;      // Fiscal ID Number
    QByteArray          Fiscal_R_Number;      // Fiscal R Number
    QString         KM;
    QByteArray      PLATE; // Plate Number önce plaka durumunda kaydedilir
    QString         runningTime;
    typedef struct nozzle_parameters
    {
        QByteArray      unitprice;
        QByteArray      unitprice_for_send;
        QByteArray      Defaultunitprice;
        QString         Product_name;
        quint8          status;
        QByteArray      PPU; // Nozzle Per Unit
        QByteArray      filling_volume;
        QByteArray      filling_amount;
        QByteArray      PVAL;  // Preset Value
        QByteArray      APVAL;  // Preset Value
        QByteArray      PLATE; // Plate Number
        QByteArray      Temp_PLATE; // Plate Number
        QByteArray      FLEET; // Custamer Name
        QByteArray      filling_price;
        QByteArray      filling_price_for_USC;
        QByteArray      filling_volume_for_USC;
        QByteArray      filling_amount_for_USC;
        QString         filling_volume_for_screen;
        QString         filling_amount_for_screen;
        QByteArray         T_Volume;
        QByteArray         T_Amount;
        QByteArray         BT_Volume; //Begin volume
        QByteArray         BT_Amount; //Begin amount
        QByteArray         USC_T_Volume;
        QByteArray         USC_T_Volume_temp;
        QByteArray         USC_T_Amount;
        QByteArray         T_Quantity;
        quint8          VLD;  // Validation // 00 ->> Accepted  01->> Black List
        quint8          ATYP; // Authorization Type
        quint8          FTYP; // Fiiling Type
        quint8          Last_FTYP; // Fiiling Type
        quint8          CTYP; // CARD Type
        quint8          STYP; // SALES Type
        quint8          PATYP; // PAYMENT Type
        quint8          DFTYP; // driver  Fiiling Type
        quint8          PTYP; // Preset type // 0->>Unlimited 1->>Money 2->>liter
        quint8          PrdctNo; // product No
        quint8          TankNo; // product No
        quint8          nozzle_id;
        quint8          CMD_resumeORsuspend; // Resume or Suspen Command 0->>Suspend 1->>Resume
        QByteArray          ECR_MifareData;      // 0 ->> Read 1->>Clear
        QByteArray          Coding_Key;
        QByteArray          Coding_Key_from_USC_to_ID;
        QByteArray          Coding_Key_from_USC_to_KM;
        QByteArray          Cyrpto_key_from_master;
    } defination_nozzle;
    defination_nozzle nozzle[15];

} defination_pump;

extern defination_pump pump[4];

typedef struct ECR_parameters
{
    quint8 PMP_No; // Pump No
    quint8 Slip_Type; // 0->> Loyalty, 1--> POS, 2->> Info
    quint8 TLine; // Total Line Number
    quint8 CHR_Type; // 0=Normal, 1=Bold
    quint8 LINE_No; // Line Number
    QByteArray INFO; // Data Block
}defination_ECR;

extern defination_ECR ECR_par;

typedef struct Probe_parameters
{
    QByteArray adres; // Pump No
    QString status;
    QString fuel_level;
    QString water_level;
    QString temperature;
    QString minLevel;
    QString maxLevel;
    QString minTemperature;
    QString maxTemperature;
    QString tankSize;
    QString tempFuelLevel;
    QString changeStatus;
    QString probeOffset;
    QString probe_maxWaterLevel;
    QString probe_minWaterLevel;
    QString probe_waterOffset;
    int unsuccess_answer;

}defination_Probe;

extern defination_Probe Probe[10];

class SaveFile
{
public:
    SaveFile() {}
    QReadWriteLock mutexset_value;
    QReadWriteLock mutexlog;
    QVariant value(QString deger)
    {
        mutexset_value.lockForRead();
        QVariant var;
        QString data;
        QFile file("/var/www/html/setting_save.txt");
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
        mutexset_value.unlock();
        return var;
    }
    void setValue(QString tag , QVariant value)
    {
        mutexset_value.lockForWrite();
        QJsonObject file_object;
        QJsonArray save_read;
        QJsonArray save_write;
        QJsonObject save_Data;
        QString data;
        QFile file("/var/www/html/setting_save.txt");
        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
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

        save_Data[tag]=value.toString();

        save_write.append(save_Data);

        file_object["SETTINGS"]=save_write;
        QJsonDocument doc(file_object);
        QSaveFile Savefile("/var/www/html/setting_save.txt");
        if(Savefile.open(QSaveFile::WriteOnly))
        {
            Savefile.write(doc.toJson(QJsonDocument::Compact));
        }
        Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        Savefile.commit();
        system("sync");
        mutexset_value.unlock();
    }

    void setPriceStatus(QByteArray price , int status)
    {
        mutexset_value.lockForWrite();
        QJsonObject save_Data;
        QString data;
        QFile file("/var/www/html/screen.json");
        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        system("sync");
        if(file.open(QFile::ReadWrite))
        {
            data=file.readAll();
        }
        file.close();
        QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
        save_Data = doc2.object();
        QString edit_price=QString::fromUtf8(price).mid(0,price.length()-unit_price_dot_index)+"."+QString::fromUtf8(price).mid(price.length()-unit_price_dot_index,unit_price_dot_index);
        QString file_price=QString::number(edit_price.toDouble(),'f',unit_price_dot_index);
        if(file_price.split(".").at(1).length()<3)
        {
            for(int i=0;i<3-file_price.split(".").at(1).length();i++)
            {
                file_price.append("0");
            }

        }
        file_price=file_price.replace(".",",");
        save_Data["unitPrice"]=file_price;
        if(save_Data["volume"].toString()=="")
            save_Data["volume"]=QString::fromStdString("0,00");
        if(save_Data["amount"].toString()=="")
            save_Data["amount"]=QString::fromStdString("0,00");
        save_Data["status"]=status;
        QJsonDocument doc(save_Data);
        QSaveFile Savefile("/var/www/html/screen.json");
        if(Savefile.open(QSaveFile::WriteOnly))
        {
            Savefile.write(doc.toJson(QJsonDocument::Compact));
        }
        Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        Savefile.commit();
        system("sync");
        mutexset_value.unlock();
    }

    void setVolumeAmount(QByteArray amount, QByteArray volume,int status,QByteArray Plate,QString product_name)
    {
        mutexset_value.lockForWrite();
        QJsonObject save_Data;
        QString data;
        QFile file("/var/www/html/screen.json");
        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        system("sync");
        if(file.open(QFile::ReadWrite))
        {
            data=file.readAll();
        }
        file.close();
        QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
        save_Data = doc2.object();

        save_Data["volume"]=QString::fromUtf8(volume);
        save_Data["amount"]=QString::fromUtf8(amount);
        save_Data["plate"]=QString::fromUtf8(Plate);
        save_Data["product_name"]=product_name;
        save_Data["status"]=status;


        QJsonDocument doc(save_Data);
        QSaveFile Savefile("/var/www/html/screen.json");
        if(Savefile.open(QSaveFile::WriteOnly))
        {
            Savefile.write(doc.toJson(QJsonDocument::Compact));
        }
        Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        Savefile.commit();
        system("sync");
        mutexset_value.unlock();
    }

//    void setProbeINfo(QString adres,QString Status, QString Fuel, QString Water, QString Temperature )
//    {
//        mutexset_value.lockForWrite();
//        QJsonArray save_read;
//        QJsonObject save_Data;
//        QString data;
//        QFile file("/var/www/html/probe_info.josn");
//        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
//        system("sync");
//        if(file.open(QFile::ReadWrite))
//        {
//            data=file.readAll();
//        }
//        file.close();
//        QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
//        save_Data = doc2.object();
//        save_read= save_Data["PROBES"].toArray();
//        QJsonArray new_data;
//        int data_status=0;
//        for(int i=0;i<save_read.size();i++)
//        {
//            QJsonObject file_object=save_read.at(i).toObject();
//            if(adres==file_object["Probe_Adres"].toString())
//            {
//                file_object["Probe_Status"]=Status;
//                file_object["Probe_Fuel"]=Fuel;
//                file_object["Probe_Water"]=Water;
//                file_object["Probe_Temperature"]=Temperature;
//                new_data.append(file_object);
//                data_status=1;
//            }
//            else
//            {
//                new_data.append(file_object);
//            }
//        }

//        if(data_status==0)
//        {
//            QJsonObject file_object;
//            file_object["Probe_Adres"]=adres;
//            file_object["Probe_Status"]=Status;
//            file_object["Probe_Fuel"]=Fuel;
//            file_object["Probe_Water"]=Water;
//            file_object["Probe_Temperature"]=Temperature;
//            new_data.append(file_object);
//        }

//        QJsonObject file_to_write;
//        file_to_write["PROBES"]=new_data;
//        QJsonDocument doc(file_to_write);
//        QSaveFile Savefile("/var/www/html/probe_info.json");
//        if(Savefile.open(QSaveFile::WriteOnly))
//        {
//            Savefile.write(doc.toJson(QJsonDocument::Compact));
//        }
//        Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
//        Savefile.commit();
//        system("sync");
//        mutexset_value.unlock();
//    }

    void setProbeINfo(QString adres,QString Status, QString Fuel, QString Water, QString Temperature,QString minLevel,QString maxLevel, QString minTemperature, QString maxTemperature, QString tankSize,QString changeStatus,QString maxWaterLevel, QString minWaterLevel)
    {
        mutexset_value.lockForWrite();
        QJsonArray save_read;
        QJsonObject save_Data;
        QString data;
        QFile file("/var/www/html/probe_info.json");
        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        system("sync");
        if(file.open(QFile::ReadWrite))
        {
            data=file.readAll();
        }
        file.close();
        QJsonDocument doc2=QJsonDocument::fromJson(data.toUtf8());
        save_Data = doc2.object();
        save_read= save_Data["PROBES"].toArray();
        QJsonArray new_data;
        int data_status=0;
        for(int i=0;i<save_read.size();i++)
        {
            QJsonObject file_object=save_read.at(i).toObject();
            if(adres==file_object["Probe_Adres"].toString())
            {
                file_object["Probe_Status"]=Status;
                file_object["Probe_Fuel"]=Fuel;
                file_object["Probe_Water"]=Water;
                file_object["Probe_Temperature"]=Temperature;
                file_object["MIN_LEVEL"]=minLevel;
                file_object["MAX_LEVEL"]=maxLevel;
                file_object["MIN_TEMPERATURE"]=minTemperature;
                file_object["MAX_TEMPERATURE"]=maxTemperature;
                file_object["TANK_SIZE"]=tankSize;
                file_object["MAXWATER_LEVEL"]=maxWaterLevel;
                file_object["MINWATER_LEVEL"]=minWaterLevel;
                file_object["Change_Status"]=changeStatus;
                new_data.append(file_object);
                data_status=1;
            }
            else
            {
                new_data.append(file_object);
            }
        }

        if(data_status==0)
        {
            QJsonObject file_object;
            file_object["Probe_Adres"]=adres;
            file_object["Probe_Status"]=Status;
            file_object["Probe_Fuel"]=Fuel;
            file_object["Probe_Water"]=Water;
            file_object["Probe_Temperature"]=Temperature;
            new_data.append(file_object);
        }

        QJsonObject file_to_write;
        file_to_write["PROBES"]=new_data;
        QJsonDocument doc(file_to_write);
        QSaveFile Savefile("/var/www/html/probe_info.json");
        if(Savefile.open(QSaveFile::WriteOnly))
        {
            Savefile.write(doc.toJson(QJsonDocument::Compact));
        }
        Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        Savefile.commit();
        system("sync");
        mutexset_value.unlock();
    }

    void setValue2(QString tag[] , QVariant value[])
    {
        mutexset_value.lockForWrite();
        QJsonObject file_object;
        QJsonArray save_read;
        QJsonArray save_write;
        QJsonObject save_Data;
        QString data;
        QFile file("/var/www/html/setting_save.txt");
        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
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
        for(int jjj=0;jjj<21;jjj++)
        {
            save_Data[tag[jjj]]=value[jjj].toString();
        }
        save_write.append(save_Data);

        file_object["SETTINGS"]=save_write;
        QJsonDocument doc(file_object);
        QSaveFile Savefile("/var/www/html/setting_save.txt");
        if(Savefile.open(QSaveFile::WriteOnly))
        {
            Savefile.write(doc.toJson(QJsonDocument::Compact));
        }
        Savefile.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        Savefile.commit();
        system("sync");
        mutexset_value.unlock();
    }
    void setlog(QString message)
    {
        mutexlog.lockForWrite();
        QString current_date=QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm");
        QFile file("/var/www/html/logger.txt");
        if(file.open(QFile::Append))
        {
            QTextStream in(&file);

            in<<current_date<<" "<<message<<"  "<<"\n";
        }
        file.setPermissions(QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        file.close();
        mutexlog.unlock();
    }

};

extern SaveFile settings;


#endif // GLOBAL_H
