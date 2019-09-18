#ifndef SCPROUTINE_H
#define SCPROUTINE_H
#include <QThread>
#include "mainwindow.h"
#include "database.h"


class ScpRoutine :public QThread
{
    Q_OBJECT
public:
    ScpRoutine();
    QTimer *rprtmr;
    QTimer *sendtmr;
    Database *database;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  void send_CD1(quint8 DCC);
    void send_CD3(quint8 PMPN, quint8 NZL); //Preset Volume
    void send_CD4(quint8 PMPN, quint8 NZL); // Preset Amount
    void send_CD5(); //price update
    void send_CD9(quint8 DPVol, quint8 Dpamo, quint8 Dpund); // set pump parametres
    void send_CD13(quint8 fillingType); // Set filling type
    void send_CD14(quint8 nozzleNo); // Suspend Request
    void send_CD15(quint8 nozzleNo); // Resume request
    void send_CD101(quint8 nozzleNo); // Request volume total counters
    void send_CD102(quint8 nozzleNo); // Request amount total counters
    void send_CD103(quint8 nozzleNo); // Request temperatures
    void send_CD104(quint8 nozzleNo); // Request Rf-Tag Data
    void pumpStatus();
    void saveTableName();
    void Message_parsing();
    void read_meter_data();
    void decode_meter(QByteArray data);
    void save_pump_status();
    void save_filling_price();
    void Change_Pump_Sta();
    void send_print_test();
    void send_CMD1(quint8 NZL);
    void send_CMDActivation(quint8 NZL);
    void decode_TagID();
    void send_printer_sale_data();
    void test_print();
    QByteArray ascii_to_hex(QString ascii_Value);
    QByteArray converthextoascii(QByteArray data);
    QByteArray set_BufferSize(QByteArray Buffer,int size);
    QByteArray set_BufferSize2(QByteArray Buffer,int size);
    QByteArray set_BufferSize3(QByteArray Buffer,int size);
    QByteArray set_ASCIIBufferSize(QByteArray Buffer,int size);
    int hex_to_dec(quint8 hex_Value);

    void send_CMD1_to_USC(quint8 mode);
    void send_DC86_to_USC();
    void send_CMD2_to_USC();
    void send_CMD3_to_USC();
    void send_CMD4_to_USC();
    void send_CMDActivation_to_USC();
    void send_DC127();
    void send_EOT(quint8 sideController_Addr);
    void send_USCACK(quint8 sideController_Addr);
    void send_DC128(quint8 sideController_Addr, quint8 Status_Byte, quint8 PMPN, quint8 NZL, QByteArray Filling_Amount);
    void send_DC130(quint8 sideController_Addr,quint8 PNZ);
    void send_DC131(quint8 sideController_Addr, quint8 PMPN, quint8 NZL);
    void send_DC133(quint8 sideController_Addr,QByteArray MifareData);
    void send_DC134(quint8 sideController_Addr, QByteArray EDAT);
    void send_DC135(quint8 sideController_Addr, quint8 PMPN, quint8 NZL);
    void send_DC137(quint8 sideController_Addr,quint8 PMPN,quint8 NZL);
    void send_DC139(quint8 sideController_Addr,quint8 PNZ,QByteArray RNO,quint8 FTYP,quint8 TYP,QByteArray PPU,QByteArray VOL,QByteArray AMO,QByteArray DATE,QByteArray TIME,QByteArray PLATE);
    void send_DC141(quint8 sideController_Addr,quint8 RNO,quint8 VSTA);
    void send_DC142(quint8 sideController_Addr,quint8 RNO,quint8 VSTA,QByteArray VID,QByteArray VDAT);
    void send_DC143(quint8 sideController_Addr,quint8 RNO,quint8 CSTA);
    void send_DC144(quint8 sideController_Addr,quint8 RNO,quint8 CSTA,QByteArray CID,QByteArray CDAT);
    void send_DC146(quint8 sideController_Addr, quint8 KNO, QByteArray KDAT);
    void send_DC147(quint8 sideController_Addr,quint8 DEV,quint8 ECODE);
    void send_DC148(quint8 sideController_Addr,quint8 DCR,quint8 STA,QByteArray SER,quint8 FMOD,QByteArray VER,QBitArray CRC16);
    void send_DC151(quint8 sideController_Addr, quint8 PMPN, quint8 NZL);
    void send_DCE2(quint8 sideController_Addr, quint8 PMPN, quint8 NZL);
    void send_DCE0(quint8 sideController_Addr, quint8 PMPN, quint8 NZL);
    void send_DC152(quint8 sideController_Addr,quint8 PMP,quint8 TYP,quint8 LN);
    void send_DC153(quint8 sideController_Addr,quint8 PMP,QBitArray PRI1,QBitArray PRI2,QBitArray PRI3,QBitArray PRI4,QBitArray PRI5);
    void send_DC156(quint8 sideController_Addr,quint8 PNO,quint8 PTYP,QByteArray PVAL,quint8 PAYM,quint8 TAGT,QByteArray PTAG,QByteArray CTAG);
    void send_DC157(quint8 sideController_Addr,quint8 PNZ,QByteArray DATE,QByteArray TIME,QByteArray CVOL,QByteArray CAMO,QByteArray SAMO,QByteArray SVOL,QByteArray DVOL,QByteArray DAMO);
    void send_message_probe();
    void read_probe_data();
    void read_SCU_data();
    void SCU_Message_parsing();
    void sent_data_to_USC();
    void make_for_Crypto_buf_USC(uint *ptr, int length);
    void make_for_buf_USC(uint *ptr, int length);
    void Encode_USC(long* data, char dataLength, quint32 delta,long *keyptr);
    void saveID();
    void saveID_old();
    void ReadsavedID();
    void send_read_acc_config();
    void send_write_acc_config();
    void Message_parsing_probe(QByteArray ReadData_probe);
    void convert_hex_to_ascii(QByteArray &data, unsigned char len,unsigned char dot_ind);//
    void save_to_sdcard();
    void send_odometre(int NZL);
    //quint8 currentTime();


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public slots:
    void routine();
    void routine_2();
    void routine_3();
    void routine_4();
    // void ReadGps();
    void send_poll();
    void read_RFID();

};
extern ScpRoutine *RoutineArray[5];

#endif // SCPROUTINE_H
