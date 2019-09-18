#include "scproutine.h"
#include "config.h"
//#include "serial.h"
#include "database.h"
#include <QDebug>
#include <QTimer>
#include "QApplication"
#include"global.h"
#include "crc_16.h"
#include"syslog.h"
#include <math.h>


ScpRoutine *RoutineArray[5];
QTimer *timer;

bool ReadTempData_clear=true;
QByteArray SendData;
QByteArray SendData2;
QByteArray SendData3;
QByteArray SendData4;
QByteArray ReadData2;
QByteArray ReadData_RFID;
QByteArray ascii;
QString price;
quint8 count_TTS=1;
unsigned cc;
void send_CD1(quint8 DCC);
unsigned int data_lenght;
void saveSettings();
quint8 Temp_Pump_no;
uint databuf[16];
quint32 delta;
QByteArray convert_format(QByteArray data,unsigned char dot_ind);
long key_USC[4] = { 0x32464741, 0x11131519, 0x26232528, 0x42454649 };
char NUM_ITERATIONS = 2;
char Record_number[2];
int dec_to_bcd(int dec);
void saveUnitPrice(quint8 NZL);
void saveVolumeTotal(quint8 PMPN,quint8 NZL);
void saveAmountTotal(quint8 PMPN ,quint8 NZL);
void saveFilling(quint8 PMPN,quint8 NZL);
void saveTsNo();
QString getMacAddress();
ScpRoutine::ScpRoutine()
{
    cc++;
    if(cc==1)
    {
        database=new Database();

        //if(pump_sta==false){database->kontrolSalesStatus();}//satışta gitmediysen önceki satış verilerini al
        Enter_flg=0;
        Nozzle_up_flg=0;
        Tag_read_count_flg=0;
        pool_sta_flg=0;
        Preset_Ack_flg=false;
        Send_authorize_flg=false;
        meter_com_err=false;
        saving_the_sales=false;

    }

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                                                     SCU
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray ScpRoutine::ascii_to_hex(QString ascii_Value)
{
    QByteArray ByteArray1=ascii_Value.toUtf8();
    QByteArray ByteArray2=ByteArray1.toHex();
    return ByteArray2;

}

QByteArray ScpRoutine::converthextoascii(QByteArray data)
{

    QByteArray xx=QByteArray::fromHex(data.toHex());
    return xx.data();

}

QByteArray ScpRoutine::set_BufferSize(QByteArray Buffer, int size) // add prepend(0x00)
{
    int fark=size-Buffer.length();
    if(Buffer.length()==size){
        return Buffer;
    }
    else{
        for(int i=0;i<fark;i++){
            Buffer.prepend('0x20');
        }
        return Buffer;
    }

}

QByteArray ScpRoutine::set_BufferSize3(QByteArray Buffer, int size) // add prepend(0x00)
{
    int fark=size-Buffer.length();
    if(Buffer.length()==size){
        return Buffer;
    }
    else{
        for(int i=0;i<fark;i++){
            Buffer.prepend('0x30');
        }
        return Buffer;
    }

}

QByteArray ScpRoutine::set_BufferSize2(QByteArray Buffer, int size) // add prepend(0x00)
{
    int fark=size-Buffer.length();
    if(Buffer.length()==size){
        return Buffer;
    }
    else{
        for(int i=0;i<fark;i++){
            Buffer.append(" ");
        }
        return Buffer;
    }

}

QByteArray ScpRoutine::set_ASCIIBufferSize(QByteArray Buffer, int size) // add prepend(0x00)
{
    int fark=size-Buffer.length();
    if(Buffer.length()==size){
        return Buffer;
    }
    else{
        for(int i=0;i<fark;i++){
            Buffer.prepend('0x30');
        }
        return Buffer;
    }

}


int ScpRoutine::hex_to_dec(quint8 hex_Value)
{
    int temp_LNG=0;
    int unit_digit=hex_Value&0x0F;
    int tens_digit=hex_Value>>4;
    temp_LNG=(tens_digit*16+unit_digit);
    return temp_LNG;
}
void ScpRoutine::send_EOT(quint8 sideController_Addr)
{

    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_eot+USCtoken;
    SendData[2]=SF;
    SCPArray[4]->send_withoutCRC(SendData);
}
void ScpRoutine::send_USCACK(quint8 sideController_Addr)
{

    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_ack+USCtoken;
    SendData[2]=SF;
    SCPArray[4]->send_withoutCRC(SendData);
}
void ScpRoutine::send_DC127()
{
    if(test)qDebug()<<"send_DC127";
}


void ScpRoutine::make_for_Crypto_buf_USC(uint *ptr, int length)
{
    char j;
    long tmp1,tmp11,tmp12,tmp13;
    for (j=0;j<15;j++)
    {
        Crypto_BUF[j]=0;
    }
    for (j=0;j<(length/4);j++)
    {
        tmp1=0;
        tmp11=0;
        tmp12=0;
        tmp13=0;

        tmp1=ptr[(j*4)+0];
        tmp11=ptr[(j*4)+1];
        tmp11<<=8;
        tmp12=ptr[(j*4)+2];
        tmp12<<=16;
        tmp13=ptr[(j*4)+3];
        tmp13<<=24;
        Crypto_BUF[j]=tmp1+tmp11+tmp12+tmp13;

    }

}

void ScpRoutine:: Encode_USC(long* data, char dataLength, quint32 delta,long *keyptr)
{
    char i=0;
    long x1=0,x1a=0,x1b=0,sum1a,dat;
    long x2=0,x2a=0,x2b=0;
    long sum;
    long sumqwe=0;
    sumqwe=0;
    char iterationCount;
    sumqwe=0x12345678;
    while(i<dataLength)
    {
        sum = 0;
        x1=data[i];
        sumqwe=x1;
        x2=data[i+1];
        sumqwe=x2;
        iterationCount = 2;
        while(iterationCount > 0)
        {
            x2a=(x2<<4);
            x2b=(x2>>5)&0x07ffffff;
            x1 += (((x2a) ^ (x2b)) + x2) ^ (sum + keyptr[(sum&0x03)]);//key_USC
            sum+=delta;
            sumqwe=sum;
            x1a=(x1<<4);
            x1b=(x1>>5)&0x07ffffff;
            sum1a=sum>>11;
            x2 += (((x1a) ^ (x1b)) + x1) ^ (sum + keyptr[((sum>>11)&0x03)]);
            dat=(sum + keyptr[((sum>>11)&0x03)]);
            iterationCount--;
        }
        data[i]=x1;
        data[i+1]=x2;
        i+=2;
    }
}


void ScpRoutine:: make_for_buf_USC(uint *ptr, int length)
{
    char j;
    long tmp1,tmp11,tmp12,tmp13;
    for (j=1;j<length;j++)
    {
        ptr[j]=0;
    }
    for (j=0;j<(length/4);j++)
    {
        tmp1=Crypto_BUF[j];
        tmp11=Crypto_BUF[j];
        tmp12=Crypto_BUF[j];
        tmp13=Crypto_BUF[j];

        ptr[(j*4)+0]=(char)0x000000ff & tmp1;
        tmp11>>=8;
        ptr[(j*4)+1]=(char)0x000000ff & tmp11;
        tmp12>>=16;
        ptr[(j*4)+2]=(char)0x000000ff & tmp12;
        tmp13>>=24;
        ptr[(j*4)+3]=(char)0x000000ff & tmp13;

    }

}

void ScpRoutine::send_CMD1_to_USC(quint8 mode)
{
    quint8 nn,mm;
    //0001184707100512
    //    TAGID[DCR_par.DCR_Nozzle_Number][0]=0;
    //    TAGID[DCR_par.DCR_Nozzle_Number][1]=0;
    //    TAGID[DCR_par.DCR_Nozzle_Number][2]=0;
    //    TAGID[DCR_par.DCR_Nozzle_Number][3]=1;
    //    TAGID[DCR_par.DCR_Nozzle_Number][4]=1;
    //    TAGID[DCR_par.DCR_Nozzle_Number][5]=8;
    //    TAGID[DCR_par.DCR_Nozzle_Number][6]=4;
    //    TAGID[DCR_par.DCR_Nozzle_Number][7]=7;
    //    TAGID[DCR_par.DCR_Nozzle_Number][8]=0;
    //    TAGID[DCR_par.DCR_Nozzle_Number][9]=7;
    //    TAGID[DCR_par.DCR_Nozzle_Number][10]=1;
    //    TAGID[DCR_par.DCR_Nozzle_Number][11]=0;
    //    TAGID[DCR_par.DCR_Nozzle_Number][12]=0;
    //    TAGID[DCR_par.DCR_Nozzle_Number][13]=5;
    //    TAGID[DCR_par.DCR_Nozzle_Number][14]=1;
    //    TAGID[DCR_par.DCR_Nozzle_Number][15]=2;

    for(int jj=0; jj<16;jj++)
    {
        databuf[jj]=TAGID[DCR_par.DCR_Nozzle_Number][jj];
    }
    if(test)qDebug()<<"SATIS_DEBUG send_CMD1_to_USC-TAGID"<<DCR_par.DCR_Nozzle_Number;
    delta=pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_ID.toHex().toUInt(&ok ,16);
    SendData.clear();
    SendData[0]=STX;
    SendData[1]=0x52;
    SendData[2]=CardReader_Addr[0]; //0x30
    SendData[3]=CardReader_Addr[1]; //0x31
    SendData[4]=NozzleRFID_fromMaster;
    if(mode==1)
    {
        pump[pumpNo].nozzle[(NozzleRFID_fromMaster-0x30)].Cyrpto_key_from_master=set_ASCIIBufferSize(pump[pumpNo].nozzle[(NozzleRFID_fromMaster-0x30)].Cyrpto_key_from_master,8);
        for(nn=0;nn<8;nn++)
        {
            SendData[5+nn]=pump[pumpNo].nozzle[(NozzleRFID_fromMaster-0x30)].Cyrpto_key_from_master[nn];
        }
        if(test)qDebug()<<">>> SATIS_DEBUG send_CMD1_to_USC-carddd"<<CustomerID;
        if(test) qDebug()<<">>> SATIS_DEBUG send_CMD1_to_USC-carddd"<<CustomerID;
        settings.setlog("SATIS_DEBUG send_CMD1_to_USC-carddd "+QString::number(DCR_par.DCR_Nozzle_Number)+"  "+CustomerID);
    }
    else if(mode==2)
    {
        make_for_Crypto_buf_USC(databuf, 16);
        Encode_USC(Crypto_BUF, 4, delta, key_USC);
        make_for_buf_USC(databuf, 16);
        mm=0;
        for (nn = 0; nn < 16; nn++)
        {
            SendData[5+mm] = (((databuf[nn] >> 4) & 0x0f) + 0x30); mm++;
            SendData[5+mm] = (((databuf[nn]) & 0x0f) + 0x30);      mm++;
        }
        if(test)qDebug()<<">>>send_CMD1_to_USC--taggg"<<TAGID[DCR_par.DCR_Nozzle_Number].toHex();
        if(test) qDebug()<<">>>send_CMD1_to_USC--taggg"<<TAGID[DCR_par.DCR_Nozzle_Number].toHex();
        settings.setlog("SATIS_DEBUG send_CMD1_to_USC--taggg "+QString::number(DCR_par.DCR_Nozzle_Number)+"  "+QString::fromUtf8(TAGID[DCR_par.DCR_Nozzle_Number].toHex()));
    }

    SCPArray[4]->writeData(SendData,SendData.length());
    if(USC_is_exist==false)
    {
        for(int jj=0; jj<16;jj++)
        {
            TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
            databuf[jj]=0;
        }
    }
}

void ScpRoutine::send_DC86_to_USC()
{
    quint8 nn,mm;
    QByteArray KM;

    for(int jj=0; jj<16;jj++)
    {
        databuf[jj]=pump[pumpNo].KM.toLatin1()[jj];
    }
    //qDebug()<<"KM_USC-TAGID"<<DCR_par.DCR_Nozzle_Number<<pump[pumpNo].KM;
    settings.setlog("SATIS_DEBUG KM_USC-TAGID "+QString::number(DCR_par.DCR_Nozzle_Number)+"  "+pump[pumpNo].KM);
    delta=pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_KM.toHex().toUInt(&ok ,16);
    SendData.clear();
    SendData[0]=STX;
    SendData[1]=0x86;
    SendData[2]=CardReader_Addr[0]; //0x30
    SendData[3]=CardReader_Addr[1]; //0x31
    SendData[4]=NozzleRFID_fromMaster;

    make_for_Crypto_buf_USC(databuf, 16);
    Encode_USC(Crypto_BUF, 4, delta, key_USC);
    make_for_buf_USC(databuf, 16);
    mm=0;
    for (nn = 0; nn < 16; nn++)
    {
        SendData[5+mm] = (((databuf[nn] >> 4) & 0x0f) + 0x30); mm++;
        SendData[5+mm] = (((databuf[nn]) & 0x0f) + 0x30);      mm++;
    }

    //qDebug()<<">>>KM_to_USC--taggg"<<SendData.toHex();

    SCPArray[4]->writeData(SendData,SendData.length());
}

void ScpRoutine::send_CMD2_to_USC()
{
    SendData.clear();
    SendData[0]=0x02;
    SendData[1]=0x43;
    SendData[2]=CardReader_Addr[0];
    SendData[3]=CardReader_Addr[1];
    SCPArray[4]->writeData(SendData,SendData.length());
}

void ScpRoutine::send_CMD3_to_USC()
{
    SendData.clear();
    SendData[0]=0x02;
    SendData[1]=0x49;
    SendData[2]=CardReader_Addr[0];
    SendData[3]=CardReader_Addr[1];
    SCPArray[4]->writeData(SendData,SendData.length());
}

void ScpRoutine::send_CMD4_to_USC()
{
    SendData.clear();
    SendData[0]=0x02;
    SendData[1]=0x54;
    SendData[2]=CardReader_Addr[0];
    SendData[3]=CardReader_Addr[1];
    SCPArray[4]->writeData(SendData,SendData.length());
}
void ScpRoutine::send_CMDActivation_to_USC()
{
    SendData.clear();
    SendData[0]=0x02;
    SendData[1]=0x89;
    SendData[2]=CardReader_Addr[0];
    SendData[3]=CardReader_Addr[1];
    SCPArray[4]->writeData(SendData,SendData.length());
}

void ScpRoutine::send_DC128(quint8 sideController_Addr, quint8 Status_Byte,quint8 PMPN,quint8 NZL,QByteArray Filling_Amount)
{
    quint8 PNZ;
    PNZ=(PMPN<<4)+(NZL&0X0F);
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC128;
    if(Status_Byte==BUSY_sta) {
        SendData[3]=0x06;
        SendData[4]=PNZ;
        SendData[5]=BUSY_sta;
        SendData[6]=Filling_Amount[0];
        SendData[7]=Filling_Amount[1];
        SendData[8]=Filling_Amount[2];
        SendData[9]=Filling_Amount[3];
        SCPArray[4]->writeData(SendData,10);
    }

    else{
        SendData[3]=0x02;
        SendData[4]=PNZ;
        SendData[5]=Status_Byte;
        SCPArray[4]->writeData(SendData,6);
    }


}


void ScpRoutine::send_DC130(quint8 sideController_Addr,quint8 PNZ)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC130;
    SendData[3]=0x01;
    SendData[4]=PNZ;
    SCPArray[4]->writeData(SendData,5);
}

void ScpRoutine::send_DC131(quint8 sideController_Addr,quint8 PMPN,quint8 NZL)
{
    quint8 PNZ;
    PNZ=(PMPN<<4)+(NZL&0X0F);
    QByteArray PLATE;
    QByteArray FID;
    QByteArray RNO;
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC131;

    if(pump[PMPN].nozzle[NZL].FTYP==FTYP_Manuel){ // Manual
        SendData[3]=0x0D;
        SendData[4]=PNZ;
        SendData[5]=pump[PMPN].nozzle[NZL].FTYP;
        SendData[6]=pump[PMPN].nozzle[NZL].filling_amount[0];
        SendData[7]=pump[PMPN].nozzle[NZL].filling_amount[1];
        SendData[8]=pump[PMPN].nozzle[NZL].filling_amount[2];
        SendData[9]=pump[PMPN].nozzle[NZL].filling_amount[3];
        SendData[10]=pump[PMPN].nozzle[NZL].filling_volume[0];
        SendData[11]=pump[PMPN].nozzle[NZL].filling_volume[1];
        SendData[12]=pump[PMPN].nozzle[NZL].filling_volume[2];
        SendData[13]=pump[PMPN].nozzle[NZL].filling_volume[3];
        SendData[14]=pump[PMPN].nozzle[NZL].filling_price[0];
        SendData[15]=pump[PMPN].nozzle[NZL].filling_price[1];
        SendData[16]=pump[PMPN].nozzle[NZL].filling_price[2];

        SCPArray[4]->writeData(SendData,17);
    }
    else{
        SendData[3]=0x3B;
        SendData[4]=PNZ;
        SendData[5]=pump[PMPN].nozzle[NZL].FTYP;
        SendData[6]=pump[PMPN].nozzle[NZL].filling_amount[0];
        SendData[7]=pump[PMPN].nozzle[NZL].filling_amount[1];
        SendData[8]=pump[PMPN].nozzle[NZL].filling_amount[2];
        SendData[9]=pump[PMPN].nozzle[NZL].filling_amount[3];
        SendData[10]=pump[PMPN].nozzle[NZL].filling_volume[0];
        SendData[11]=pump[PMPN].nozzle[NZL].filling_volume[1];
        SendData[12]=pump[PMPN].nozzle[NZL].filling_volume[2];
        SendData[13]=pump[PMPN].nozzle[NZL].filling_volume[3];
        SendData[14]=pump[PMPN].nozzle[NZL].filling_price[0];
        SendData[15]=pump[PMPN].nozzle[NZL].filling_price[1];
        SendData[16]=pump[PMPN].nozzle[NZL].filling_price[2];

        PLATE=set_BufferSize2(pump[PMPN].nozzle[NZL].PLATE,32);
        for(int i=17;i<49;i++){
            SendData[i]=PLATE[i-17];
        }

        FID=converthextoascii(pump[PMPN].Fiscal_ID);
        FID=set_BufferSize(FID,12);
        for(int i=49;i<61;i++){
            SendData[i]=FID[i-49];
        }
        //RNO=converthextoascii(pump[PMPN].nozzle[PMPN].Fiscal_R_Number);
        //RNO=set_BufferSize(RNO,2);
        SendData[61]=pump[PMPN].Fiscal_R_Number[1];
        SendData[62]=pump[PMPN].Fiscal_R_Number[0];

        SCPArray[4]->writeData(SendData,63);
        if(test) qDebug()<<"send_DC131"<<SendData.toHex();
    }


}

void ScpRoutine::send_DC133(quint8 sideController_Addr,QByteArray MifareData)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC133;
    SendData[3]=0x14;
    MifareData=converthextoascii(MifareData);
    MifareData=set_BufferSize(MifareData,20);
    for (int i = 4; i < 24; i++) {
        SendData[i]=MifareData[i-4];
    }
    SCPArray[4]->writeData(SendData,28);


}

void ScpRoutine::send_DC134(quint8 sideController_Addr,QByteArray EDAT)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC134;
    SendData[3]=0x30;
    EDAT=converthextoascii(EDAT);
    EDAT=set_BufferSize(EDAT,48);
    for(int i=4;i<52;i++){
        SendData[i]=EDAT[i-4];
    }
    SCPArray[4]->writeData(SendData,56);
}

void ScpRoutine::send_DC135(quint8 sideController_Addr,quint8 PMPN,quint8 NZL)
{
    quint8 PNZ;
    QByteArray tempTSN;
    PNZ=(Temp_Pump_no<<4)+(NZL&0X0F);

    if(routinedebug)qDebug()<<"send_DC135-Paid_Confirm: "<<Paid_Confirm;
    if((Paid_Confirm==true ) || (saving_the_sales==true))
    {
        if(routinedebug)qDebug()<<"USC-son-ekran-cevabı";
        long begintotal=pump[PMPN].nozzle[NZL].BT_Volume.toHex().toLong();
        long fillingfortotal=pump[PMPN].nozzle[NZL].filling_volume.toHex().toLong();
        long USCendtotal=pump[PMPN].nozzle[NZL].USC_T_Volume.toHex().toLong();
        if(routinedebug)qDebug()<<"begintotal: "<<QString::number(begintotal);
        if(routinedebug)qDebug()<<"fillingfortotal: "<<QString::number(fillingfortotal);
        if(routinedebug)qDebug()<<"USCendtotal: "<<QString::number(USCendtotal);
        if(USCendtotal>=begintotal)
        {
            if(((USCendtotal-begintotal)-fillingfortotal<=50) && ((USCendtotal-begintotal)-fillingfortotal>=-50))
            {
                if(routinedebug)qDebug()<<"total-doğru"<<pump[PMPN].nozzle[NZL].USC_T_Volume.toHex();
                SendData[5]=pump[PMPN].nozzle[NZL].USC_T_Volume[0];
                SendData[6]=pump[PMPN].nozzle[NZL].USC_T_Volume[1];
                SendData[7]=pump[PMPN].nozzle[NZL].USC_T_Volume[2];
                SendData[8]=pump[PMPN].nozzle[NZL].USC_T_Volume[3];
            }
            else
            {
                dec_to_bcd(pump[PMPN].nozzle[NZL].BT_Volume.toHex().toInt()+pump[PMPN].nozzle[NZL].filling_volume.toHex().toInt());
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[0]=bcd[3];
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[1]=bcd[2];
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[2]=bcd[1];
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[3]=bcd[0];
                SendData[5]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[0];
                SendData[6]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[1];
                SendData[7]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[2];
                SendData[8]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[3];
                if(routinedebug)qDebug()<<"total-hesaplandı"<<pump[PMPN].nozzle[NZL].USC_T_Volume_temp.toHex();

            }

        }
        else
        {
            if((((100000000-begintotal) + USCendtotal-fillingfortotal)<=50) && (((100000000-begintotal) + USCendtotal-fillingfortotal)>=-50))
            {
                if(routinedebug)qDebug()<<"total-doğru"<<pump[PMPN].nozzle[NZL].USC_T_Volume.toHex();
                SendData[5]=pump[PMPN].nozzle[NZL].USC_T_Volume[0];
                SendData[6]=pump[PMPN].nozzle[NZL].USC_T_Volume[1];
                SendData[7]=pump[PMPN].nozzle[NZL].USC_T_Volume[2];
                SendData[8]=pump[PMPN].nozzle[NZL].USC_T_Volume[3];
            }
            else
            {
                dec_to_bcd(pump[PMPN].nozzle[NZL].BT_Volume.toHex().toInt()+pump[PMPN].nozzle[NZL].filling_volume.toHex().toInt());
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[0]=bcd[3];
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[1]=bcd[2];
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[2]=bcd[1];
                pump[PMPN].nozzle[NZL].USC_T_Volume_temp[3]=bcd[0];
                SendData[5]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[0];
                SendData[6]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[1];
                SendData[7]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[2];
                SendData[8]=pump[PMPN].nozzle[NZL].USC_T_Volume_temp[3];
                if(routinedebug)qDebug()<<"total-hesaplandı"<<pump[PMPN].nozzle[NZL].USC_T_Volume_temp.toHex();
            }

        }

    }
    else
    {
        if(routinedebug)qDebug()<<"paid-confrim-öncesi-totol"<<pump[PMPN].nozzle[NZL].USC_T_Volume.toHex();
        SendData[5]=pump[PMPN].nozzle[NZL].USC_T_Volume[0];
        SendData[6]=pump[PMPN].nozzle[NZL].USC_T_Volume[1];
        SendData[7]=pump[PMPN].nozzle[NZL].USC_T_Volume[2];
        SendData[8]=pump[PMPN].nozzle[NZL].USC_T_Volume[3];

    }

    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC135;
    SendData[3]=0x0F;
    SendData[4]=PNZ;
    SendData[9]=pump[PMPN].nozzle[NZL].USC_T_Amount[0];
    SendData[10]=pump[PMPN].nozzle[NZL].USC_T_Amount[1];
    SendData[11]=pump[PMPN].nozzle[NZL].USC_T_Amount[2];
    SendData[12]=pump[PMPN].nozzle[NZL].USC_T_Amount[3];
    SendData[13]=pump[PMPN].nozzle[NZL].USC_T_Amount[4];
    SendData[14]=pump[PMPN].nozzle[NZL].USC_T_Amount[5];
    tempTSN=converthextoascii(QByteArray::number(TS_no));
    tempTSN=set_BufferSize(tempTSN,4);
    for(int i=15;i<19;i++){
        SendData[i]=tempTSN[i-15];
    }
    SCPArray[4]->writeData(SendData,19);
    if(test)qDebug()<<"ScpRoutine::send_DC135"<<SendData.toHex();
    if(test)qDebug()<<"ScpRoutine::send_DC135****"<<NZL<<pump[PMPN].nozzle[NZL].USC_T_Volume.toHex();

}

void ScpRoutine::send_DC137(quint8 sideController_Addr,quint8 PMPN,quint8 NZL)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC137;
    SendData[3]=0x25;
    SendData[4]=0x10; //DCR status

    SendData.append(QDate::currentDate().toString("yyMMdd"));
    SendData.append(QDateTime::currentDateTime().toString("HHmmss"));


    for(int i=11;i<16;i++){
        SendData[i]=0x30;
    }

    SendData[16]=ABU_address;

    for(int i=17;i<25;i++){
        SendData[i]=0x30;
    }


    for(int i=25;i<39;i++){
        SendData[i]=0x30;
    }
    SendData[39]=0x04;// mepsan
    SendData[40]=0x11;

    SCPArray[4]->writeData(SendData,41);
}

void ScpRoutine::send_DC139(quint8 sideController_Addr, quint8 PNZ, QByteArray RNO, quint8 FTYP, quint8 TYP, QByteArray PPU, QByteArray VOL, QByteArray AMO, QByteArray DATE, QByteArray TIME,QByteArray PLATE)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC139;
    SendData[3]=0x37;
    SendData[4]=PNZ;
    SendData[5]=RNO[0];
    SendData[6]=RNO[1];
    SendData[7]=FTYP;
    SendData[8]=TYP;
    SendData[9]=PPU[0];
    SendData[10]=PPU[1];
    SendData[11]=PPU[2];
    SendData[12]=PPU[3];
    SendData[13]=VOL[0];
    SendData[14]=VOL[1];
    SendData[15]=VOL[2];
    SendData[16]=VOL[3];
    SendData[17]=AMO[0];
    SendData[18]=AMO[1];
    SendData[19]=AMO[2];
    SendData[20]=AMO[3];
    SendData[20]=DATE[0];
    SendData[21]=DATE[1];
    SendData[22]=DATE[2];
    SendData[23]=TIME[0];
    SendData[24]=TIME[1];
    SendData[25]=TIME[2];
    PLATE=converthextoascii(PLATE);
    PLATE=set_BufferSize(PLATE,32);
    for(int i=26;i<58;i++){
        SendData[i]=PLATE[i-26];
    }
    SCPArray[4]->writeData(SendData,58);

}

void ScpRoutine::send_DC141(quint8 sideController_Addr, quint8 RNO, quint8 VSTA)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC141;
    SendData[3]=0x02;
    SendData[4]=RNO;
    SendData[5]=VSTA;
    SCPArray[4]->writeData(SendData,6);
}

void ScpRoutine::send_DC142(quint8 sideController_Addr, quint8 RNO, quint8 VSTA, QByteArray VID,QByteArray VDAT)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC142;
    SendData[3]=0x4A;
    SendData[4]=RNO;
    SendData[5]=VSTA;
    SendData[6]=VID[0];
    SendData[7]=VID[1];
    SendData[8]=VID[2];
    SendData[9]=VID[3];
    SendData[10]=VID[4];
    SendData[11]=VID[5];
    SendData[12]=VID[6];
    SendData[13]=VID[7];
    VDAT=converthextoascii(VDAT);
    VDAT=set_BufferSize(VDAT,64);
    for(int i=14;i<78;i++){
        SendData[i]=VDAT[i-14];
    }
    SCPArray[4]->writeData(SendData,78);


}

void ScpRoutine::send_DC143(quint8 sideController_Addr, quint8 RNO, quint8 CSTA)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC143;
    SendData[3]=0x02;
    SendData[4]=RNO;
    SendData[5]=CSTA;
    SCPArray[4]->writeData(SendData,6);

}

void ScpRoutine::send_DC144(quint8 sideController_Addr, quint8 RNO, quint8 CSTA, QByteArray CID,QByteArray CDAT)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC144;
    SendData[3]=0x4A;
    SendData[4]=RNO;
    SendData[5]=CSTA;
    SendData[6]=CID[0];
    SendData[7]=CID[1];
    SendData[8]=CID[2];
    SendData[9]=CID[3];
    SendData[10]=CID[4];
    SendData[11]=CID[5];
    SendData[12]=CID[6];
    SendData[13]=CID[7];
    CDAT=converthextoascii(CDAT);
    CDAT=set_BufferSize(CDAT,64);
    for(int i=14;i<78;i++){
        SendData[i]=CDAT[i-14];
    }
    SCPArray[4]->writeData(SendData,78);


}

void ScpRoutine::send_DC146(quint8 sideController_Addr, quint8 KNO,QByteArray KDAT)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC146;
    SendData[3]=0x21;
    SendData[4]=KNO;
    KDAT=converthextoascii(KDAT);
    KDAT=set_BufferSize(KDAT,32);
    for(int i=5;i<37;i++){
        SendData[i]=KDAT[i-5];
    }
    SCPArray[4]->writeData(SendData,37);


}

void ScpRoutine::send_DC147(quint8 sideController_Addr, quint8 DEV, quint8 ECODE)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC147;
    SendData[3]=0x02;
    SendData[4]=DEV;
    SendData[5]=ECODE;
    SCPArray[4]->writeData(SendData,6);
}

void ScpRoutine::send_DC148(quint8 sideController_Addr, quint8 DCR, quint8 STA, QByteArray SER, quint8 FMOD, QByteArray VER, QBitArray CRC16)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC148;
    SendData[3]=0x0C;
    SendData[4]=DCR;
    SendData[5]=STA;
    SendData[6]=SER[0];
    SendData[7]=SER[1];
    SendData[8]=SER[2];
    SendData[9]=SER[3];
    SendData[10]=FMOD;
    SendData[11]=VER[0];
    SendData[12]=VER[1];
    SendData[13]=VER[2];
    SendData[14]=CRC16[0];
    SendData[15]=CRC16[1];
    SCPArray[4]->writeData(SendData,16);
}

void ScpRoutine::send_DC151(quint8 sideController_Addr, quint8 PMPN, quint8 NZL)
{
    quint8 PNZ;
    QByteArray PLATE;
    PNZ=(PMPN<<4)+(NZL&0X0F);
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC151;
    SendData[3]=0x21;
    SendData[4]=PNZ;
    PLATE=set_BufferSize2(pump[PMPN].nozzle[NZL].PLATE,32);
    for(int i=5;i<37;i++){
        SendData[i]=PLATE[i-5];
    }
    SCPArray[4]->writeData(SendData,37);

}

void ScpRoutine::send_DCE2(quint8 sideController_Addr, quint8 PMPN, quint8 NZL)
{
    quint8 PNZ;
    QByteArray PLATE;
    PNZ=(PMPN<<4)+(NZL&0X0F);
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DCE2;
    SendData[3]=0x22;
    SendData[4]=PNZ;
    PLATE=set_BufferSize2(pump[PMPN].nozzle[NZL].PLATE,32);
    for(int i=5;i<37;i++){
        SendData[i]=PLATE[i-5];
    }
    SendData[37]=NewPlateExist;
    SCPArray[4]->writeData(SendData,38);
    if(test)qDebug()<<"send_DCE2-PLATE"<<PLATE<<SendData.toHex();
    settings.setlog("SCP_DEBUG SEND-PLATE-TO-USC  "+QString::fromUtf8(SendData.toHex()));
    settings.setlog("SATIS_DEBUG SEND-PLATE-TO-USC "+QString::fromUtf8(PLATE.toHex()));
}

void ScpRoutine::send_DCE0(quint8 sideController_Addr, quint8 PMPN, quint8 NZL)
{
    quint8 PNZ;
    QByteArray PLATE;
    QByteArray FID;
    QByteArray tempTSN;
    PNZ=(PMPN<<4)+(NZL&0X0F);
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DCE0;
    SendData[3]=0x50;
    SendData[4]=PNZ;
    SendData[5]=Record_number[0];
    SendData[6]=Record_number[1];
    SendData[7]=pump[PMPN].nozzle[NZL].FTYP;
    SendData[8]=pump[PMPN].nozzle[NZL].filling_amount[0];
    SendData[9]=pump[PMPN].nozzle[NZL].filling_amount[1];
    SendData[10]=pump[PMPN].nozzle[NZL].filling_amount[2];
    SendData[11]=pump[PMPN].nozzle[NZL].filling_amount[3];
    SendData[12]=pump[PMPN].nozzle[NZL].filling_volume[0];
    SendData[13]=pump[PMPN].nozzle[NZL].filling_volume[1];
    SendData[14]=pump[PMPN].nozzle[NZL].filling_volume[2];
    SendData[15]=pump[PMPN].nozzle[NZL].filling_volume[3];
    SendData[16]=pump[PMPN].nozzle[NZL].filling_price[0];
    SendData[17]=pump[PMPN].nozzle[NZL].filling_price[1];
    SendData[18]=pump[PMPN].nozzle[NZL].filling_price[2];


    PLATE=set_BufferSize2(pump[PMPN].nozzle[NZL].PLATE,32);
    if(test)qDebug()<<"send_DCE0-PLATE"<<pump[PMPN].nozzle[NZL].PLATE;
    settings.setlog("SATIS_DEBUG send_DCE0-PLATE  "+QString::fromUtf8(pump[PMPN].nozzle[NZL].PLATE.toHex()));
    for(int i=19;i<51;i++){
        SendData[i]=PLATE[i-19];
    }

    FID=converthextoascii(pump[PMPN].Fiscal_ID);
    FID=set_BufferSize(FID,12);
    for(int i=51;i<63;i++){
        SendData[i]=FID[i-51];
    }

    SendData[63]=pump[PMPN].Fiscal_R_Number[1];
    SendData[64]=pump[PMPN].Fiscal_R_Number[0];


    dec_to_bcd(TS_date.toInt());
    SendData[65]=bcd[2];
    SendData[66]=bcd[1];
    SendData[67]=bcd[0];

    dec_to_bcd(TS_time.toInt());
    SendData[68]=bcd[2];
    SendData[69]=bcd[1];
    SendData[70]=bcd[0];


    tempTSN=converthextoascii(QByteArray::number(TS_no));
    tempTSN=set_BufferSize(tempTSN,4);
    for(int i=71;i<75;i++){
        SendData[i]=tempTSN[i-71];
    }


    SendData[75]=Sale_Error_code;

    SendData.append(converthextoascii(serial_No));

    SCPArray[4]->writeData(SendData,83);
    if(test)qDebug()<<"send_DCE0-SALES-INFORMATION "<<SendData.toHex();
    settings.setlog("SCP_DEBUG send_DCE0-SALES-INFORMATION   "+QString::fromUtf8(SendData.toHex()));
}

void ScpRoutine::send_DC152(quint8 sideController_Addr, quint8 PMP, quint8 TYP, quint8 LN)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC152;
    SendData[3]=0x03;
    SendData[4]=PMP;
    SendData[5]=TYP;
    SendData[6]=LN;
    SCPArray[4]->writeData(SendData,7);
}

void ScpRoutine::send_DC153(quint8 sideController_Addr, quint8 PMP, QBitArray PRI1, QBitArray PRI2, QBitArray PRI3, QBitArray PRI4, QBitArray PRI5)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC153;
    SendData[3]=0x10;
    SendData[4]=PMP;
    SendData[5]=PRI1[0];
    SendData[6]=PRI1[1];
    SendData[7]=PRI1[2];
    SendData[8]=PRI2[0];
    SendData[9]=PRI2[1];
    SendData[10]=PRI2[2];
    SendData[11]=PRI3[0];
    SendData[12]=PRI3[1];
    SendData[13]=PRI3[2];
    SendData[14]=PRI4[0];
    SendData[15]=PRI4[1];
    SendData[16]=PRI4[2];
    SendData[17]=PRI5[0];
    SendData[18]=PRI5[1];
    SendData[19]=PRI5[2];
    SCPArray[4]->writeData(SendData,20);
}

void ScpRoutine::send_DC156(quint8 sideController_Addr, quint8 PNO, quint8 PTYP, QByteArray PVAL, quint8 PAYM, quint8 TAGT, QByteArray PTAG, QByteArray CTAG)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC156;
    SendData[3]=0x18;
    SendData[4]=PNO;
    SendData[5]=PTYP;
    SendData[6]=PVAL[0];
    SendData[7]=PVAL[1];
    SendData[8]=PVAL[2];
    SendData[9]=PVAL[3];
    SendData[10]=PAYM;
    SendData[11]=TAGT;
    SendData[12]=PTAG[0];
    SendData[13]=PTAG[1];
    SendData[14]=PTAG[2];
    SendData[15]=PTAG[3];
    SendData[16]=PTAG[4];
    SendData[17]=PTAG[5];
    SendData[18]=PTAG[6];
    SendData[19]=PTAG[7];
    SendData[20]=CTAG[0];
    SendData[21]=CTAG[1];
    SendData[22]=CTAG[2];
    SendData[23]=CTAG[3];
    SendData[24]=CTAG[4];
    SendData[25]=CTAG[5];
    SendData[26]=CTAG[6];
    SendData[27]=CTAG[7];
    SCPArray[4]->writeData(SendData,28);

}

void ScpRoutine::send_DC157(quint8 sideController_Addr, quint8 PNZ, QByteArray DATE, QByteArray TIME, QByteArray CVOL, QByteArray CAMO, QByteArray SAMO,QByteArray SVOL,QByteArray DVOL, QByteArray DAMO)
{
    SendData[0]=sideController_Addr;
    SendData[1]=ctrl_data+USCtoken;
    SendData[2]=TNO_DC157;
    SendData[3]=0x25;
    SendData[4]=PNZ;
    SendData[5]=DATE[0];
    SendData[6]=DATE[1];
    SendData[7]=DATE[2];
    SendData[8]=TIME[0];
    SendData[9]=TIME[1];
    SendData[10]=TIME[2];
    SendData[11]=CVOL[0];
    SendData[12]=CVOL[1];
    SendData[13]=CVOL[2];
    SendData[14]=CVOL[3];
    SendData[15]=CAMO[0];
    SendData[16]=CAMO[1];
    SendData[17]=CAMO[2];
    SendData[18]=CAMO[3];
    SendData[19]=SVOL[0];
    SendData[20]=SVOL[1];
    SendData[21]=SVOL[2];
    SendData[22]=SVOL[3];
    SendData[23]=SAMO[0];
    SendData[24]=SAMO[1];
    SendData[25]=SAMO[2];
    SendData[26]=SAMO[3];
    SendData[27]=DVOL[0];
    SendData[28]=DVOL[1];
    SendData[29]=DVOL[2];
    SendData[30]=DVOL[3];
    SendData[31]=DAMO[0];
    SendData[32]=DAMO[1];
    SendData[33]=DAMO[2];
    SendData[34]=DAMO[3];
    SendData[35]=DAMO[4];
    SendData[36]=DAMO[5];

    SCPArray[4]->writeData(SendData,37);


}

void ScpRoutine::read_SCU_data()
{
    //    Crypto Crypt;
    //    SendData.clear();
    //    SCPArray[4]->readSCUData(ReadUSCData);
    //    if(ReadTempData_clear==true){ReadTempData_clear=false; ReadTempData.clear();}
    //    if(ReadUSCData.length() > 0)
    //    {
    //        ReadTempData.append(ReadUSCData);
    //        ReadUSCData.clear();
    //        qDebug()<<"ReadTempData_with_="<<ReadTempData.toHex();
    //        if(ReadTempData.contains("mlb"))
    //        {
    //            //qDebug()<<"ReadTempData_with_crypted"<<ReadTempData.toHex();
    //            ReadTempData.remove(ReadTempData.lastIndexOf("mlb"),ReadTempData.length()-(ReadTempData.lastIndexOf("mlb")-1));
    //           // qDebug()<<"ReadTempData_with_crypted2"<<ReadTempData.toHex();
    //            ReadTempData=QString::fromStdString(Crypt.decrypt(QString::fromLatin1(ReadTempData).toStdString().c_str())).toLatin1();
    //            QByteArray Read_Data;
    //            Read_Data=ReadTempData;
    //            ReadTempData=QByteArray::fromHex(Read_Data);
    //            USC_last_index=ReadTempData.lastIndexOf(0xFA);
    //            //qDebug()<<"ReadTempData_RAW"<<ReadTempData.toHex()<<QString::number(USC_last_index);
    //            SCU_Message_parsing();
    //            ReadTempData_clear=true;
    //        }
    //    }

    SendData.clear();
    SCPArray[4]->readSCUData(ReadUSCData);
    if(ReadTempData_clear==true){ReadTempData_clear=false; ReadTempData.clear();}
    if(ReadUSCData.length() > 0)
    {
        ReadTempData.append(ReadUSCData);
        ReadUSCData.clear();
        //if(test)qDebug()<<"ReadTempData_with_FA"<<ReadTempData.toHex();
        if(ReadTempData.contains(0xFA))
        {
            USC_last_index=ReadTempData.lastIndexOf(0xFA);
            //qDebug()<<"ReadTempData_with_FA"<<ReadTempData.toHex();

            SCU_Message_parsing();
            //ReadTempData_clear=true;
            ReadTempData.clear();
        }
    }


}

void ScpRoutine::sent_data_to_USC()
{

    switch (answer_flg) {
    case 0://EOT
        send_EOT(ABU_addr);
        break;
    case 1://STATUS
        send_DC128(ABU_addr, pump[pumpNo].status,pumpNo,DCR_par.DCR_Nozzle_Number,pump[pumpNo].nozzle[nozzleNo].filling_amount);
        break;
    case 2://send_end_of_filling_report
        send_DC131(ABU_addr,pumpNo,nozzleNo);
        break;
    case 3://ECR_Mifare_data
        send_DC133(ABU_addr,pump[pumpNo].nozzle[nozzleNo].ECR_MifareData);
        break;
    case 4://Total_report
        send_DC135(ABU_addr,pumpNo,TotalnozzleNo);
        break;
    case 5://DCR_Parameters
        send_DC137(ABU_addr,pumpNo,nozzleNo);
        break;
    case 6://ECR_Plate
        send_DC151(ABU_addr,pumpNo,nozzleNo);
        break;
    case 7://ECR_Plate_2
        send_DCE2(ABU_addr,pumpNo,nozzleNo);
        break;
    case 8://END_of_filling_2
        send_DCE0(ABU_addr,pumpNo,nozzleNo);
        break;
    default:
        break;
    }

}

void ScpRoutine::SCU_Message_parsing()
{

    if(ReadTempData[USC_last_index]==0xFA) // FA check
    {
        if((USC_com_lock==false) && (reboot_ok!=true))
        {
            if(ReadTempData[0]==ABU_addr) // Adress check
            {
                USC_com_error_count=0;

                if((USC_is_exist==false) || (USC_com_error_flg==true))
                {
                    USC_com_error_flg=false;
                    TEXT_fromMaster.clear();
                    //pump[pumpNo].status=pump[pumpNo].laststatus;
                    if((Nozzle_up_flg==1)&& ((pump[pumpNo].laststatus==2) || (pump[pumpNo].laststatus==6))){Sale_resume_flg=true;}
                    if(LCD_status==15){ LCD_status=LCD_status_last;}
                    USC_is_exist=true;
                    cannotchange_frame=false;
                    system_started=false;
                    if(test)qDebug()<<"USC_is_exist=true"<<USC_is_exist;
                }

                if((ReadTempData[1]>=0x20)&&(ReadTempData[1]<=0x2F)){        //Check poll

                    USCtoken=ReadTempData[1]-0x20;
                    poll_receive_flg=true;
                    sent_data_to_USC();
                    answer_flg=0;   ///send_EOT
                }
                else if((ReadTempData[1]>=0xC0)&&(ReadTempData[1]<=0xCF))    //Check ACK
                {

                }

                else if((ReadTempData[1]>=0x50)&&(ReadTempData[1]<=0x5F))  //Check NAK
                {

                }
                else if((ReadTempData[1]>=0x30)&&(ReadTempData[1]<=0x3F)){  //Check DATA
                    data_receive_flg=true;

                    // check CRC value
                    if((ReadTempData[USC_last_index-2]==0xfa) && (ReadTempData[USC_last_index-3]==0x10)){
                        USCcalcrc(ReadTempData.data(),USC_last_index-4,0);
                        USC_crc_lsb_ind=USC_last_index-4;
                        USC_crc_msb_ind=USC_last_index-2;
                    }
                    else if((ReadTempData[USC_last_index-3]==0xfa)&&(ReadTempData[USC_last_index-4]==0x10))
                    {
                        USCcalcrc(ReadTempData.data(),USC_last_index-4,0);
                        USC_crc_lsb_ind=USC_last_index-3;
                        USC_crc_msb_ind=USC_last_index-2;

                    }
                    else if((ReadTempData[USC_last_index-2]==0xfa)&&(ReadTempData[USC_last_index-3]==0x10) &&(ReadTempData[USC_last_index-4]==0x10) &&(ReadTempData[USC_last_index-5]==0x10))
                    {
                        USCcalcrc(ReadTempData.data(),USC_last_index-4,0);
                        USC_crc_lsb_ind=USC_last_index-4;
                        USC_crc_msb_ind=USC_last_index-2;

                    }
                    else
                    {
                        USCcalcrc(ReadTempData.data(),USC_last_index-3,0);
                        USC_crc_lsb_ind=USC_last_index-3;
                        USC_crc_msb_ind=USC_last_index-2;
                    }

                    USCtoken=ReadTempData[1]-0x30;
                    if((USC_crc_lsb==ReadTempData[USC_crc_lsb_ind])&&(USC_crc_msb==ReadTempData[USC_crc_msb_ind])){ // CRC is true
                        //USC_is_exist=true;
                        switch (ReadTempData[2]) {
                        case 0x7F:                                  //Preset Amount/Volume
                            if(ReadTempData[3]==0x06){
                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                pump[pumpNo].nozzle[nozzleNo].PTYP=ReadTempData[5];
                                if(pump[pumpNo].nozzle[nozzleNo].PTYP!=0)
                                {
                                    online_flg=true;
                                    if(pump[pumpNo].nozzle[nozzleNo].PTYP==1)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[0]=ReadTempData[6];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[1]=ReadTempData[7];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[2]=ReadTempData[8];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[3]=ReadTempData[9];
                                    }
                                    if(pump[pumpNo].nozzle[nozzleNo].PTYP==2)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[0]=ReadTempData[6];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[1]=ReadTempData[7];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[2]=ReadTempData[8];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[3]=ReadTempData[9];
                                    }
                                }
                                if(test)qDebug()<<"*****************Preset Amount/Volume******************"<<pump[pumpNo].nozzle[nozzleNo].PVAL;
                                settings.setlog("SATIS_DEBUG  ***Preset Amount/Volume***   "+QString::number(pump[pumpNo].nozzle[nozzleNo].PTYP)+"   "+QString::fromUtf8(pump[pumpNo].nozzle[nozzleNo].PVAL.toHex())+"   "+QString::fromUtf8(pump[pumpNo].nozzle[nozzleNo].APVAL.toHex()));
                            }

                            break;

                        case 0x80:                                  // Request Pump Status
                            if(ReadTempData[3]==0x01){
                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                answer_flg=1; ///send status
                                //qDebug()<<"*****************Request Pump Status******************";
                            }
                            break;
                        case 0x82:                                  // Paid Confirmed
                            if(ReadTempData[3]==0x01){

                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                Payment_Error_code=0;
                                Paid_Confirm=true;
                                RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+2,1,"1");
                                //qDebug()<<"RFID_Read_Time-paid_confirm"<<RFID_Read_Time;
                                answer_flg=1; ///send status
                                Paid_Confrim_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                                Paid_Confrim_Time.prepend("20");
                                Paid_Fiscal_Time=Paid_Confrim_Time;
                                if(test)qDebug()<<"Paid_Confrim_Time"<<Paid_Confrim_Time;
                                ////syslog(LOG_DEBUG,"SATIS_DEBUG  ***Paid Confirmed** %s",Paid_Confrim_Time.toLocal8Bit().constData());

                            }
                            break;
                        case 0x83:                                   // Request End of Filling Report
                            if(ReadTempData[3]==0x01){

                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                answer_flg=2; ///send_end_of_filling_report
                                if(test)qDebug()<<"*****************Request End of Filling Report******************";

                            }
                            break;
                        case 0x84:                                   // Suspend & Resume Message
                            if(ReadTempData[3]==0x02){

                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                pump[pumpNo].nozzle[nozzleNo].CMD_resumeORsuspend=ReadTempData[5];
                                if(pump[pumpNo].nozzle[nozzleNo].CMD_resumeORsuspend==0){Sale_suspend_flg=true;}
                                if(pump[pumpNo].nozzle[nozzleNo].CMD_resumeORsuspend==1){Sale_resume_flg=true;}
                                answer_flg=1; ///send status
                                if(test)qDebug()<<"*****************Suspend & Resume Message******************";
                                ////syslog(LOG_DEBUG,"SATIS_DEBUG  ***Suspend & Resume Message*** %d",pump[pumpNo].nozzle[nozzleNo].CMD_resumeORsuspend);
                            }
                            break;
                        case 0x85:                                   // Request ECR Mifare Data
                            if(ReadTempData[3]==0x01){
                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                //pump[pumpNo].nozzle[nozzleNo].ECR_MifareData=ReadTempData[5];
                                answer_flg=3; ///ECR Mifare Data
                                if(test)qDebug()<<"*****************SRequest ECR Mifare Datae******************";
                                ////syslog(LOG_DEBUG,"SATIS_DEBUG  ***SRequest ECR Mifare Datae*** ");

                            }
                            break;
                        case 0x86:                                   // Data Transfer to ECR
                            if(ReadTempData[3]==0x30){
                                for(int i=4;i<52;i++){
                                    SDAT_forECR[i-4]=ReadTempData[i];
                                }
                                if(test)qDebug()<<"*****************Data Transfer to ECR******************";
                                ////syslog(LOG_DEBUG,"SATIS_DEBUG  ***Data Transfer to ECR*** ");
                            }
                            break;
                        case 0x87:                                   // Request Totalizer
                            if(ReadTempData[3]==0x01){

                                TotalnozzleNo=ReadTempData[4]&0x0F;
                                Temp_Pump_no=ReadTempData[4]>>4;//
                                answer_flg=4; ///nozzle total
                                if(routinedebug)qDebug()<<"*****************Request Totalizer******************";
                                ////syslog(LOG_DEBUG,"SATIS_DEBUG  ***Request Totalizer*** ");
                            }
                            break;
                        case 0x88:                                   // Authorize & Unauthorize
                            ////syslog(LOG_DEBUG,"SCP_DEBUG  ***AUTHORIZE-ALL-MESSAGE*** %s",ReadTempData.toHex().constData());
                            if(ReadTempData[10]==ATYP_TTS || ReadTempData[10]==ATYP_MTS || ReadTempData[10]==ATYP_Loyalyt)
                            {
                                if(ReadTempData[3]==0x32){
                                    nozzleNo=ReadTempData[4]&0x0F;
                                    //pumpNo=ReadTempData[4]>>4; //sonradan
                                    pumpNo=1;

                                    pump[pumpNo].nozzle[nozzleNo].PTYP=ReadTempData[5];
                                    pump[pumpNo].nozzle[nozzleNo].FTYP=ReadTempData[10];
                                    pump[pumpNo].nozzle[nozzleNo].CTYP=ReadTempData[10];
                                    for(int pl=0; pl<12;pl++)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].PLATE[pl]=0x20;
                                    }
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[0]=ReadTempData[11];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[1]=ReadTempData[12];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[2]=ReadTempData[13];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[3]=ReadTempData[14];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[4]=ReadTempData[15];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[5]=ReadTempData[16];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[6]=ReadTempData[17];
                                    pump[pumpNo].nozzle[nozzleNo].PLATE[7]=ReadTempData[18];
                                    for(int i=19;i<51;i++){
                                        pump[pumpNo].nozzle[nozzleNo].FLEET[i-19]=ReadTempData[i];
                                    }

                                    pump[pumpNo].nozzle[nozzleNo].VLD=ReadTempData[51];
                                    pump[pumpNo].nozzle[nozzleNo].Last_FTYP=ReadTempData[52];
                                    pump[pumpNo].nozzle[nozzleNo].ATYP=ReadTempData[53];
                                }
                            }
                            else
                            {
                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                pump[pumpNo].nozzle[nozzleNo].PTYP=ReadTempData[5];
                                pump[pumpNo].nozzle[nozzleNo].FTYP=ReadTempData[10];
                                pump[pumpNo].nozzle[nozzleNo].CTYP=ReadTempData[10];
                                pump[pumpNo].nozzle[nozzleNo].Last_FTYP=0;
                                pump[pumpNo].nozzle[nozzleNo].ATYP=0;
                                if(pump[pumpNo].nozzle[nozzleNo].PTYP!=0)
                                {
                                    online_flg=true;
                                    if(pump[pumpNo].nozzle[nozzleNo].PTYP==1)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[0]=ReadTempData[6];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[1]=ReadTempData[7];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[2]=ReadTempData[8];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[3]=ReadTempData[9];
                                    }
                                    if(pump[pumpNo].nozzle[nozzleNo].PTYP==2)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[0]=ReadTempData[6];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[1]=ReadTempData[7];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[2]=ReadTempData[8];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[3]=ReadTempData[9];
                                    }
                                }
                                else
                                {
                                    online_flg=false;
                                }
                                Confirmation_flg=1;
                                ask_nozzle_total_volume=true;
                                Confirmation_flg_without_km=true;
                                Sale_Error_code=0;

                            }
                            if(pump[pumpNo].nozzle[nozzleNo].VLD!=1)
                            {
                                if(pump[pumpNo].nozzle[nozzleNo].PTYP!=0)
                                {
                                    online_flg=true;
                                    if(pump[pumpNo].nozzle[nozzleNo].PTYP==1)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[0]=ReadTempData[6];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[1]=ReadTempData[7];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[2]=ReadTempData[8];
                                        pump[pumpNo].nozzle[nozzleNo].APVAL[3]=ReadTempData[9];
                                    }
                                    if(pump[pumpNo].nozzle[nozzleNo].PTYP==2)
                                    {
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[0]=ReadTempData[6];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[1]=ReadTempData[7];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[2]=ReadTempData[8];
                                        pump[pumpNo].nozzle[nozzleNo].PVAL[3]=ReadTempData[9];
                                    }
                                }
                                else
                                {
                                    online_flg=false;
                                }
                                Confirmation_flg=1;
                                Confirmation_flg_without_km=true;
                                ask_nozzle_total_volume=true;
                                Sale_Error_code=0;
                            }
                            RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+1,1,"1");
                            //qDebug()<<"RFID_Read_Time-authorize"<<RFID_Read_Time;

                            if(test)qDebug()<<"*****************Authorize & Unauthorizer******************";
                            if(test)qDebug()<<"**Preset type**"<<pump[pumpNo].nozzle[nozzleNo].PTYP<<online_flg;
                            if(test)qDebug()<<"**Preset PVAL**"<<pump[pumpNo].nozzle[nozzleNo].PVAL.toHex();
                            if(test)qDebug()<<"**Preset AVAL**"<<pump[pumpNo].nozzle[nozzleNo].APVAL.toHex();
                            if(test)qDebug()<<"**PLATE**"<<pump[pumpNo].nozzle[nozzleNo].PLATE;
                            if(test)qDebug()<<"**Fleet**"<<pump[pumpNo].nozzle[nozzleNo].FLEET;//customer name
                            if(test)qDebug()<<"**validation**"<<pump[pumpNo].nozzle[nozzleNo].VLD;
                            if(test)qDebug()<<"**CTYP**"<<pump[pumpNo].nozzle[nozzleNo].CTYP;
                            if(test)qDebug()<<"**lastFTYP**"<<pump[pumpNo].nozzle[nozzleNo].Last_FTYP;
                            if(test)qDebug()<<"**ATYP**"<<pump[pumpNo].nozzle[nozzleNo].ATYP;
                            if(test)qDebug()<<"**VLD**"<<pump[pumpNo].nozzle[nozzleNo].VLD;
                            if(test)qDebug()<<"**Confirmation_flg**"<<Confirmation_flg<<Confirmation_flg_without_km<<online_flg;

                            /*syslog(LOG_DEBUG,"SATIS_DEBUG  *****************Authorize & Unauthorizer****************** ");
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***Preset TYPE*** %d",pump[pumpNo].nozzle[nozzleNo].PTYP);
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***Preset PVAL*** %s",pump[pumpNo].nozzle[nozzleNo].PVAL.toHex().constData());
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***Preset APVAL*** %s",pump[pumpNo].nozzle[nozzleNo].APVAL.toHex().constData());
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***PLATE*** %s",pump[pumpNo].nozzle[nozzleNo].PLATE.toHex().constData());
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***Fleet*** %s",pump[pumpNo].nozzle[nozzleNo].FLEET.toHex().constData());
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***Validation*** %d",pump[pumpNo].nozzle[nozzleNo].VLD);
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***CTYP*** %d",pump[pumpNo].nozzle[nozzleNo].CTYP);
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***LastFTYP*** %d",pump[pumpNo].nozzle[nozzleNo].Last_FTYP);
                            syslog(LOG_DEBUG,"SATIS_DEBUG ***ATYP*** %d",pump[pumpNo].nozzle[nozzleNo].ATYP);*/
                            answer_flg=1; ///send status
                            break;

                        case 0x89:                                   // Request DCR Parameters
                            if(ReadTempData[3]==0x01){
                                PPN=ReadTempData[4];
                            }
                            answer_flg=5; ///DCR Parameters
                            break;

                        case 0x8A:                                   // Set DCR Parameters
                            if(ReadTempData[3]==0x02){
                                PPN=ReadTempData[4];
                                DCRP=ReadTempData[5];
                            }
                            break;

                        case 0x8B:                                   // Request Sale Record
                            if(ReadTempData[3]==0x02){
                                REC_Number[0]=ReadTempData[4];
                                REC_Number[1]=ReadTempData[5];
                            }
                            break;

                        case 0x8C:                                   // Update PPU
                            CD_message_LNG=hex_to_dec(ReadTempData[3]);
                            for(int i=0;i<CD_message_LNG;i=i+4){
                                nozzleNo=ReadTempData[i+4]&0x0F;
                                pumpNo=ReadTempData[i+4]>>4;
                                pump[pumpNo].nozzle[nozzleNo].unitprice_for_send[0]=ReadTempData[i+5];
                                pump[pumpNo].nozzle[nozzleNo].unitprice_for_send[1]=ReadTempData[i+6];
                                pump[pumpNo].nozzle[nozzleNo].unitprice_for_send[2]=ReadTempData[i+7];
                                //_for_save[nozzleNo]=QByteArray::number(pump[pumpNo].nozzle[nozzleNo].unitprice.toHex().toInt()).insert(QString::number(pump[pumpNo].nozzle[nozzleNo].unitprice.toHex().toInt()).length()-unit_price_dot_index,".");
                                //saveUnitPrice(nozzleNo);

                                if(test)qDebug()<<"*****************UPDATE U-PRICE******************"<<pump[pumpNo].nozzle[nozzleNo].unitprice_for_send.toHex();
                                //syslog(LOG_DEBUG,"SATIS_DEBUG ***UPDATE U-PRICE*** %s",pump[pumpNo].nozzle[nozzleNo].unitprice.toHex().constData());
                            }
                            Update_flg=true;
                            break;
                        case 0x8D:                                   // Request VID Status
                            if(ReadTempData[3]==0x01){
                                RNO=ReadTempData[4];
                            }
                            break;

                        case 0x8E:                                   // Request VID Data
                            if(ReadTempData[3]==0x01){
                                RNO=ReadTempData[4];
                            }
                            break;

                        case 0x8F:                                   // Request CID Status
                            if(ReadTempData[3]==0x01){
                                RNO=ReadTempData[4];
                            }
                            break;

                        case 0x90:                                   // Request CID Data
                            if(ReadTempData[3]==0x01){
                                RNO=ReadTempData[4];
                            }
                            break;

                        case 0x91:                                  // Update Info Display
                            if(ReadTempData[3]==0x1B){
                                BUZ=ReadTempData[4];
                                DNO=ReadTempData[5];
                                LNO=ReadTempData[6];
                                for(int i=7;i<31;i++){
                                    DDAT[i-7]=ReadTempData[i];
                                }
                            }

                            break;

                        case 0x92:                                   // Read Keypad Buffer
                            if(ReadTempData[3]==0x01){
                                KNO=ReadTempData[4];
                            }
                            break;

                        case 0x93:                                   // Request Error Report
                            if(ReadTempData[3]==0x01){
                                DEV=ReadTempData[4];
                            }
                            break;
                        case 0x94:                                   // Request DCR Status
                            if(ReadTempData[3]==0x02){
                                DCR=ReadTempData[4];
                                TMO=ReadTempData[5];
                            }
                            break;

                        case 0x96:                                   // Pump ByPass Mode
                            CD_message_LNG=hex_to_dec(ReadTempData[3]);
                            PASS[0]=ReadTempData[4];
                            PASS[1]=ReadTempData[5];
                            PASS[2]=ReadTempData[6];
                            PASS[3]=ReadTempData[7];
                            Mode=ReadTempData[8];
                            Size=ReadTempData[9];
                            for(int i=10;i<(Size+10);i++){
                                CMD_forBypassMode[i-10]=ReadTempData[i];
                            }


                            break;

                        case 0x97:                                   // Request ECR Plate
                            if(ReadTempData[3]==0x01){
                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                            }
                            answer_flg=6;//ECR_Plate
                            break;

                        case 0x98:                                   // Slip Data to ECR
                            if(ReadTempData[3]==0x25){
                                ECR_par.PMP_No=ReadTempData[4];
                                ECR_par.Slip_Type=ReadTempData[5];
                                ECR_par.TLine=ReadTempData[6];
                                ECR_par.CHR_Type=ReadTempData[7];
                                ECR_par.LINE_No=ReadTempData[8];
                                for(int i=9;i<41;i++){
                                    ECR_par.INFO[i-9]=ReadTempData[9];
                                }

                            }

                            break;

                        case 0x99:                                   // Request Unit Price
                            if(ReadTempData[3]==0x01){
                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                            }
                            break;

                        case 0x9A:                                   // DCR Reset
                            if(ReadTempData[3]==0x01){
                                RST_Type=ReadTempData[4];
                            }
                            break;

                        case 0x9B:                                   // Update  DCR Date/Time
                            if(ReadTempData[3]==0x06){
                                DCR_Date[0]=ReadTempData[4];
                                DCR_Date[1]=ReadTempData[5];
                                DCR_Date[2]=ReadTempData[6];
                                DCR_Time[0]=ReadTempData[7];
                                DCR_Time[1]=ReadTempData[8];
                                DCR_Time[2]=ReadTempData[9];
                                QProcess proc;
                                proc.start("date -s \""+DCR_Date[1]+"/"+DCR_Date[2]+"/"+DCR_Date[0]+" "+DCR_Time+":"+DCR_Time+":00"+"\"");
                                proc.waitForFinished();
                                system("hwclock -w");
                            }
                            break;

                        case 0x9C:                                   // Request Keypad Status
                            if(ReadTempData[3]==0x01){
                                pumpNo=ReadTempData[4];
                            }
                            break;

                        case 0x9D:                                   // Request Shift Totalizer
                            if(ReadTempData[3]==0x02){

                                nozzleNo=ReadTempData[4]&0x0F;
                                //pumpNo=ReadTempData[4]>>4; //sonradan
                                pumpNo=1;
                                SHF=ReadTempData[5];
                            }
                            break;
                        case 0xE0:                                   // End-of-filling-IETT version
                            nozzleNo=ReadTempData[4]&0x0F;
                            //pumpNo=ReadTempData[4]>>4; //sonradan
                            pumpNo=1;
                            Record_number[0]=ReadTempData[5];
                            Record_number[1]=ReadTempData[6];

                            if((Record_number[0]==0)&&(Record_number[1]==0))
                            {
                                answer_flg=8; ///send end_of_filling
                            }
                            break;
                        case 0xE2:                                   // Request Plate
                            nozzleNo=ReadTempData[4]&0x0F;
                            //pumpNo=ReadTempData[4]>>4; //sonradan
                            pumpNo=1;
                            answer_flg=7; ///send new_plate

                            break;
                        default:
                            if(test)qDebug()<<"USC-Bilinmeyen Mesaj Tipi"<<ReadTempData.toHex();
                            //syslog(LOG_DEBUG,"SCP_DEBUG ***USC-Bilinmeyen Mesaj Tipi*** %s",ReadTempData.toHex().constData());
                            break;

                        } // Switch ReadTempData[2]
                        send_USCACK(ABU_addr);
                    } // Check CRC
                } // Check Data Message
            }
            else if(ReadTempData[0]==0x02){
                system_started=false;
                if(ReadTempData[2]==CardReader_Addr[0] && ReadTempData[3]==CardReader_Addr[1]){ // Adress değişkende tutulabilir
                    if((ReadTempData[USC_last_index-2]==0xfa) && (ReadTempData[USC_last_index-3]==0x10)){
                        USCcalcrc(ReadTempData.data(),USC_last_index-4,0);
                        USC_crc_lsb_ind=USC_last_index-4;
                        USC_crc_msb_ind=USC_last_index-2;
                    }
                    else if((ReadTempData[USC_last_index-3]==0xfa)&&(ReadTempData[USC_last_index-4]==0x10))
                    {
                        USCcalcrc(ReadTempData.data(),USC_last_index-4,0);
                        USC_crc_lsb_ind=USC_last_index-3;
                        USC_crc_msb_ind=USC_last_index-2;

                    }
                    else if((ReadTempData[USC_last_index-2]==0xfa)&&(ReadTempData[USC_last_index-3]==0x10) &&(ReadTempData[USC_last_index-4]==0x10) &&(ReadTempData[USC_last_index-5]==0x10))
                    {
                        USCcalcrc(ReadTempData.data(),USC_last_index-4,0);
                        USC_crc_lsb_ind=USC_last_index-4;
                        USC_crc_msb_ind=USC_last_index-2;

                    }
                    else
                    {
                        USCcalcrc(ReadTempData.data(),USC_last_index-3,0);
                        USC_crc_lsb_ind=USC_last_index-3;
                        USC_crc_msb_ind=USC_last_index-2;
                    }

                    USCtoken=ReadTempData[1]-0x30;
                    if((USC_crc_lsb==ReadTempData[USC_crc_lsb_ind])&&(USC_crc_msb==ReadTempData[USC_crc_msb_ind])){ // CRC is true


                        switch(ReadTempData[1]) { // Transaction Data
                        case 0x52: // Read Tag ID

                            //SendData3.clear();
                            //SendData3=ReadTempData;
                            NozzleRFID_fromMaster=ReadTempData[4];
                            //qDebug()<<"*****************Read Tag ID******************"<<NozzleRFID_fromMaster;
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_ID[0]=ReadTempData[5];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_ID[1]=ReadTempData[6];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_ID[2]=ReadTempData[7];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_ID[3]=ReadTempData[8];
                            if(NozzleRFID_fromMaster==0x30){pump[pumpNo].nozzle[0].Cyrpto_key_from_master=CustomerID.toLatin1(); send_CMD1_to_USC(1);}
                            else{send_CMD1_to_USC(2);}

                            break;

                        case 0x43: // Clear Screen
                            if(test)qDebug()<<"*****************Clear Screen******************";
                            DummyRFID_fromMaster=ReadTempData[4];
                            if(test)qDebug()<<"TEXT_fromMaster.clear"<<TEXT_fromMaster.length()<<TEXT_fromMaster;
                            for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                            {
                                TEXT_fromMaster[jj]=0x20;
                            }
                            send_CMD2_to_USC();
                            break;

                        case 0x49: // Show Stored Screen
                            StoredScreenRFID_fromMaster=ReadTempData[4];
                            qDebug()<<"*****************Show Stored Screen******************"<<StoredScreenRFID_fromMaster;
                            ////syslog(LOG_DEBUG,"SATIS_DEBUG ***Show Stored Screen*** %d",StoredScreenRFID_fromMaster);
                            send_CMD3_to_USC();
                            if(StoredScreenRFID_fromMaster==1){Allow_card_read=1;}//usc read kart gönderdiğinde

                            break;

                        case 0x54: // Write text to LCD
                        {
                            if(test)qDebug()<<"TEXT_fromMaster.clear"<<TEXT_fromMaster.length()<<TEXT_fromMaster;
                            for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                            {
                                TEXT_fromMaster[jj]=0x20;
                            }
                            SCR_X_fromMaster=ReadTempData[4];
                            SCR_Y_fromMaster=ReadTempData[5];
                            Text_With_fromMaster=ReadTempData[6];
                            Pixel_OnOff_fromMaster=ReadTempData[7];
                            TextLen_fromMaster[0]=ReadTempData[8];
                            TextLen_fromMaster[1]=ReadTempData[9];

                            int LocalLen=(((TextLen_fromMaster[0])*10)+(TextLen_fromMaster[1]));
                            for(int i=0;i<LocalLen;i++){
                                TEXT_fromMaster[i]=ReadTempData[i+10];
                            }
                            LCD_refresh_flg=1;
                            send_CMD4_to_USC();
                            if(test)qDebug()<<"*****************Write text to LCD******************"<<TEXT_fromMaster<<LocalLen;
                            ////syslog(LOG_DEBUG,"SATIS_DEBUG ****Write text to LCD*** %s %d",QString::fromUtf8(TEXT_fromMaster).toLocal8Bit().constData(),LocalLen);
                            break;
                        }
                        case 0x86: // Odometre
                        {
                            NozzleRFID_fromMaster=ReadTempData[4];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_KM[0]=ReadTempData[5];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_KM[1]=ReadTempData[6];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_KM[2]=ReadTempData[7];
                            pump[pumpNo].nozzle[NozzleRFID_fromMaster-0x30].Coding_Key_from_USC_to_KM[3]=ReadTempData[8];
                            send_DC86_to_USC();
                            break;
                        }
                        case 0x89: // nozzle activation
                        {
                            Activation_Nozzle=ReadTempData[4]-0x30;
                            send_CMDActivation_to_USC();
                            activation_flg=true;
                            break;
                        }
                        default:
                            if(test)qDebug()<<"USC-Card-Reader-Bilinmeyen Mesaj tipi"<<ReadTempData.toHex();
                            ///syslog(LOG_DEBUG,"SCP_DEBUG USC-Card-Reader-Bilinmeyen Mesaj tipi %s",ReadTempData.toHex().constData());
                            break;
                        }
                        LCD_refresh_flg=1;
                    }

                }


                system_started=false;
            } // Check FA
        }
    }
}


void ScpRoutine::routine_4()//USC
{
    if(upc_locvis==false)
    {
        if(system_lock==false)
        {
            read_SCU_data();
            if(USC_is_exist==true)
            {
                USC_com_error_count++;
                if(reboot_ok==true){USC_com_error_count=6001;}
                //qDebug()<<"USC_com_error_count"<<USC_com_error_count;
                if((USC_com_error_count>6200) && (USC_com_error_flg==true))
                {
                    USC_com_error_count=0;
                }
                if((USC_com_error_count>6000) && (USC_com_error_flg==false)) //USC GİTTİ.
                {
                    pump[pumpNo].laststatus=pump[pumpNo].status;
                    if(((pump[pumpNo].status==2) || (pump[pumpNo].status==6)) && (password_is_deactive==false)){Sale_suspend_flg=true;}
                    USC_com_error_count=0;
                    USC_com_error_flg=true;
                    LCD_status_last=LCD_status;
                    LCD_status=15;
                    LCD_refresh_flg=1;
                    if(test) qDebug()<<"USC YOK.";
                    ////if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_USC_DEBUG  ************************USC-haberlesme hatası***************");
                    USC_com_count++;
                    if(Nozzle_up_flg==1){USC_com_lock=true;}
                }
            }
        }
        else
        {
            if(LCD_status!=13 && licenceCheck==0)
            {
                lock_status=LCD_status;
            }
            LCD_status=13;
            LCD_refresh_flg=1;
        }
    }
    else
    {
        if(probe_mesaj_tick%10==0)
        {
            send_message_probe();
            probe_mesaj_tick=0;
        }
        read_probe_data();
        probe_mesaj_tick++;
        for(int i=0;i<probe_count;i++)
        {
            if(Probe[i].unsuccess_answer>30)
            {
                if(Probe[i].status!="0")
                {
                    database->UpdateProbe(QString::fromUtf8(Probe[i].adres.toHex()).toUpper(),"0","-","-","-");
                    settings.setProbeINfo(QString::fromUtf8(Probe[i].adres.toHex()).toUpper(),"0","-","-","-","-","-","-","-","-","0","-","-");
                }
                Probe[i].unsuccess_answer=31;
                Probe[i].status="0";
                Probe[i].fuel_level="-";
                Probe[i].water_level="-";
                Probe[i].temperature="-";
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////
///
///                                               PROBE
///
void ScpRoutine::send_message_probe()
{
    probe_address_select++;
    if(probe_address_select>=probe_count)
    {
        probe_address_select=0;
    }
    SendData4.clear();
    SendData4[0]=Probe[probe_address_select].adres[0];
    SendData4[1]=0x2C;
    SCPArray[4]->write_probe_data(SendData4);


}


void ScpRoutine::read_probe_data()
{
    ReadData_probe.clear();
    SCPArray[4]->readData_serial_probe(ReadData_probe);
    if(ReadData_probe.length()>0)
    {
        //qDebug()<<"Read probe : "<<ReadData_probe.toHex();
        int result=0;
        for(int i=2;i<ReadData_probe.length()-5;i++)
        {
            result+=ReadData_probe[i];
        }
        result=0x10000-result;
        //qDebug()<<"result : "<<QByteArray::number(result).toHex();
        if(ReadData_probe.mid(ReadData_probe.length()-5,5)==QByteArray::number(result))
        {
            //qDebug()<<"crs is true";
           if(test) qDebug()<<"Probe Message"<<ReadData_probe.toHex();
            Message_parsing_probe(ReadData_probe);
        }
        else
        {
            //qDebug()<<"crs is false";
            Probe[probe_address_select].unsuccess_answer++;
        }
    }
    else
    {
        Probe[probe_address_select].unsuccess_answer++;
    }
}

void ScpRoutine::Message_parsing_probe(QByteArray ReadData_probe)
{
    if(test)qDebug()<<"Selected Address "<<probe_address_select<<QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
    // database->ProbeValues();

    Probe[probe_address_select].unsuccess_answer=0;
    Probe[probe_address_select].status="1";
    Probe[probe_address_select].fuel_level=QString::fromUtf8(ReadData_probe.mid(3,7));

    if(fabs(Probe[probe_address_select].fuel_level.toDouble(&ok)-Probe[probe_address_select].tempFuelLevel.toDouble(&ok))>=5.0){
        Probe[probe_address_select].changeStatus="1";
        Probe[probe_address_select].tempFuelLevel=Probe[probe_address_select].fuel_level;
    }
    else{
        Probe[probe_address_select].changeStatus="0";
    }
    Probe[probe_address_select].water_level=QString::fromUtf8(ReadData_probe.mid(11,7));
    Probe[probe_address_select].water_level=QString::number(Probe[probe_address_select].water_level.toDouble(&ok)+Probe[probe_address_select].probe_waterOffset.toDouble(&ok),'f',2);
    Probe[probe_address_select].temperature=QString::fromUtf8(ReadData_probe.mid(19,6));
    if(test)qDebug()<<"Fuel Level"<<Probe[probe_address_select].fuel_level;
    Probe[probe_address_select].fuel_level=QString::number(Probe[probe_address_select].fuel_level.toDouble(&ok)+Probe[probe_address_select].probeOffset.toDouble(&ok),'f',2);

    if(test)qDebug()<<"offset Fuel Level"<<Probe[probe_address_select].fuel_level;
    if(test)qDebug()<<"Offset Water Level"<<Probe[probe_address_select].water_level;
    if(test)qDebug()<<"Temperature Level"<<Probe[probe_address_select].temperature;
    database->UpdateProbe(QString::fromUtf8(Probe[probe_address_select].adres.toHex()).toUpper(),Probe[probe_address_select].status,Probe[probe_address_select].fuel_level,Probe[probe_address_select].water_level,Probe[probe_address_select].temperature);
    settings.setProbeINfo(QString::fromUtf8(Probe[probe_address_select].adres.toHex()).toUpper(),Probe[probe_address_select].status,Probe[probe_address_select].fuel_level,Probe[probe_address_select].water_level,Probe[probe_address_select].temperature,Probe[probe_address_select].minLevel,Probe[probe_address_select].maxLevel,Probe[probe_address_select].minTemperature,Probe[probe_address_select].maxTemperature,Probe[probe_address_select].tankSize,Probe[probe_address_select].changeStatus,Probe[probe_address_select].probe_maxWaterLevel,Probe[probe_address_select].probe_minWaterLevel);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                                                     METER
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_ack()
{
    SendData2.clear();
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_ack+token;
    SendData2[2]=SF; //0xFA
    SCPArray[2]->send_ack(SendData2);
    request_flg=1;
}

void send_nak()
{
    SendData2.clear();
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_nak+token;
    SendData2[2]=SF;
    SCPArray[2]->send_ack(SendData2);
    request_flg=1;
}

void ScpRoutine::send_poll()
{

    if(Pump_Filling_status!=1)
    {
        pool_sta_flg++;
        if(pool_sta_flg%8==0)
        {
            send_CD1(0x00); // status
        }
        else if(pool_sta_flg%45==0)
        {
            total_nozzle_number++;
            if(total_nozzle_number>Number_of_Nozzle)
            {
                total_nozzle_number=1;
            }
            if(routinedebug)qDebug()<<"Boşta-total-sorgusu"<<saving_the_sales;
            if(saving_the_sales!=true)
            {
                ask_nozzle_total_volume=true;
            }
            else
            {
                if(routinedebug)qDebug()<<"kayıt-esnasında-totol-sorgusu-iptal";
                SCPArray[2]->send_poll(dcr_pool);
            }

        }
        //        else if(pool_sta_flg%53==0)
        //        {
        //             send_CD1(0x04); // ekran
        //        }

        else
        {
            SCPArray[2]->send_poll(dcr_pool);
        }
        if(pool_sta_flg>=250)
        {
            pool_sta_flg=0;
        }
    }
    else
    {
        if(test) qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>send_pollll"<<dcr_pool.toHex();
        SCPArray[2]->send_poll(dcr_pool);
    }
    if(Preset_Ack_flg==true)
    {authorize_count++;}
    if(authorize_count>12)
    {authorize_count=3;}
    if(Send_authorize_flg_2==true)
    {authorize_count2++;}
    if(authorize_count2>12)
    {authorize_count2=3;}
    send_meter_data_count++;
    if(send_meter_data_count>=25)//metre hatası
    {
        if(send_meter_data_count==25)
        {
            LCD_status_last=LCD_status;
            LCD_status=1;
            LCD_refresh_flg=1;
            meter_last_error=true;
        }
        meter_com_err=true;

    }
    if(send_meter_data_count>30){send_meter_data_count=26;}

}
void send_CD1(quint8 DCC)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data + token;
    SendData2[2]=TNO_CD1;
    SendData2[3]=1; //Data_len
    SendData2[4]=DCC;
    SCPArray[2]->writeMeterData(SendData2,5);
    if(test){if(DCC==0x04)qDebug()<<"filling-info";}

}

void send_CD2(quint8 NozzleNumber)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data +token;
    SendData2[2]=TNO_CD2;
    SendData2[3]=1; // Data_len
    SendData2[4]=NozzleNumber;
    SCPArray[2]->writeMeterData(SendData2,5);
}
void ScpRoutine::send_CD3(quint8 PMPN, quint8 NZL)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    Preset_Token=token;
    SendData2[2]=TNO_CD3;
    SendData2[3]=4;

    SendData2.append(pump[PMPN].nozzle[NZL].PVAL[0]);
    SendData2.append(pump[PMPN].nozzle[NZL].PVAL[1]);
    SendData2.append(pump[PMPN].nozzle[NZL].PVAL[2]);
    SendData2.append(pump[PMPN].nozzle[NZL].PVAL[3]);

    SCPArray[2]->writeMeterData(SendData2,8);
    if(test)qDebug()<<"ScpRoutine::send_CD3-volume-preset"<<SendData2.toHex();
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-send_CD3-volume-preset %s",SendData2.toHex().constData());
    settings.setlog("SATIS_DEBUG PUMP-send_CD3-volume-preset : "+QString::fromUtf8(SendData2.toHex()));
}

void ScpRoutine::send_CD4(quint8 PMPN, quint8 NZL)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD4;
    SendData2[3]=4;

    SendData2.append(pump[PMPN].nozzle[NZL].APVAL[0]);
    SendData2.append(pump[PMPN].nozzle[NZL].APVAL[1]);
    SendData2.append(pump[PMPN].nozzle[NZL].APVAL[2]);
    SendData2.append(pump[PMPN].nozzle[NZL].APVAL[3]);

    SCPArray[2]->writeMeterData(SendData2,8);
    if(test)qDebug()<<"ScpRoutine::send_CD4-amount-preset"<<SendData2.toHex();
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-send_CD4-amount-preset %s",SendData2.toHex().constData());
    settings.setlog("SATIS_DEBUG PUMP-send_CD4-amount-preset : "+QString::fromUtf8(SendData2.toHex()));
}

void ScpRoutine::send_CD5()
{
    quint8 jj;
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD5;
    SendData2[3]=3*Number_of_Nozzle; // ürün  3

    if(USC_is_exist==false)
    {
        for(jj=1;jj<=Number_of_Nozzle;jj++)
        {
            SendData2.append(pump[pumpNo].nozzle[jj].unitprice[0]);
            SendData2.append(pump[pumpNo].nozzle[jj].unitprice[1]);
            SendData2.append(pump[pumpNo].nozzle[jj].unitprice[2]);
        }
    }
    else
    {
        for(jj=1;jj<=Number_of_Nozzle;jj++)
        {
            SendData2.append(pump[pumpNo].nozzle[jj].unitprice_for_send[0]);
            SendData2.append(pump[pumpNo].nozzle[jj].unitprice_for_send[1]);
            SendData2.append(pump[pumpNo].nozzle[jj].unitprice_for_send[2]);
        }
    }

    SCPArray[2]->writeMeterData(SendData2,SendData2.length());
    if(test)qDebug()<<"*****************UPDATE Meter-PRICE******************"<<SendData2.toHex()<<pump[pumpNo].nozzle[2].unitprice_for_send.toHex();
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-UPDATE U-PRICE %s",SendData2.toHex().constData());

    settings.setlog("SATIS_DEBUG PUMP-UPDATE U-PRICE : "+QString::fromUtf8(SendData2.toHex()));
}

void ScpRoutine::send_CD9(quint8 DPVol, quint8 Dpamo, quint8 Dpund)
{
    int i,j,k,m,n;
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD9;
    SendData2[3]=0x33; //boyut 51
    for(i=1;i<=22;i++){
        SendData2[3+i]=0;
    }
    j=SendData2.length();
    SendData2[j]=DPVol;
    SendData2[j+1]=Dpamo;
    SendData2[j+2]=Dpund;
    for(k=1;k<=5;k++){
        SendData2[j+k+2]=0;
    }
    j=SendData2.length();
    for(m=1;m<=4;m++){
        SendData2[j+m]=0;
    }
    j=SendData2.length();
    for(n=1;n<=17;n++){
        SendData2[j+n]=0;
    }
    SCPArray[2]->writeMeterData(SendData2,SendData2.length());
}

void ScpRoutine::send_CD13(quint8 fillingType)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD13;
    SendData2[3]=0x01;
    SendData2[4]=fillingType;
    SCPArray[2]->writeMeterData(SendData2,5);

}

void ScpRoutine::send_CD14(quint8 nozzleNo)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD14;
    SendData2[3]=0x01;
    SendData2[4]=nozzleNo;
    SCPArray[2]->writeMeterData(SendData2,5);
    if(test)qDebug()<<"SUSPEND-NOzzle:"<<nozzleNo;
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-SUSPEND-NOzzle %d",nozzleNo);
    settings.setlog("SATIS_DEBUG PUMP-SUSPEND-NOzzle : "+QString::fromUtf8(SendData2.toHex()));
}

void ScpRoutine::send_CD15(quint8 nozzleNo)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD15;
    SendData2[3]=0x01;
    SendData2[4]=nozzleNo;
    SCPArray[2]->writeMeterData(SendData2,5);
    if(test)qDebug()<<"RESUME-NOzzle:"<<nozzleNo;
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-RESUME-NOzzle %d",nozzleNo);
    settings.setlog("SATIS_DEBUG PUMP-RESUME-NOzzle : "+QString::fromUtf8(SendData2.toHex()));
}

void ScpRoutine::send_CD101(quint8 nozzleNo)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD101;
    SendData2[3]=0x01;
    SendData2[4]=nozzleNo;
    SCPArray[2]->writeMeterData(SendData2,5);
    if(test)qDebug()<<"Request-volume-total";
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-Request-volume-total");
    settings.setlog("SATIS_DEBUG PUMP-Request-volume-total");
}

void ScpRoutine::send_CD102(quint8 nozzleNo)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD101;
    SendData2[3]=0x01;
    SendData2[4]=0x10 | nozzleNo;
    SCPArray[2]->writeMeterData(SendData2,5);
    if(test)qDebug()<<"Request-amount-total"<<SendData2.toHex();
    ////syslog(LOG_DEBUG,"SATIS_DEBUG PUMP-Request-amount-total");

    settings.setlog("SATIS_DEBUG PUMP-Request-amount-total");
}

void ScpRoutine::send_CD103(quint8 nozzleNo)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD103;
    SendData2[3]=0x01;
    SendData2[4]=nozzleNo;
    SCPArray[2]->writeMeterData(SendData2,5);
}

void ScpRoutine::send_CD104(quint8 nozzleNo)
{
    SendData2[0]=Pump_Addr;
    SendData2[1]=ctrl_data+token;
    SendData2[2]=TNO_CD103;
    SendData2[3]=0x01;
    SendData2[4]=nozzleNo;
    SCPArray[2]->writeMeterData(SendData2,5);
}

void ScpRoutine::send_CMD1(quint8 NZL)
{
    SendData3[0]=STX;
    SendData3[1]=0x52;
    SendData3[2]=CardReader_Addr[0];
    SendData3[3]=CardReader_Addr[1];
    SendData3[4]=0x30+NZL;
    SendData3[5]=pump[pumpNo].nozzle[NZL].Coding_Key[0];
    SendData3[6]=pump[pumpNo].nozzle[NZL].Coding_Key[1];
    SendData3[7]=pump[pumpNo].nozzle[NZL].Coding_Key[2];
    SendData3[8]=pump[pumpNo].nozzle[NZL].Coding_Key[3];
    for(int i=0;i<4;i++){
        Decoding_Key[i]=pump[pumpNo].nozzle[NZL].Coding_Key[i];
    }

    SCPArray[3]->writeRFIDData(SendData3,9);
}

void ScpRoutine::send_CMDActivation(quint8 NZL)
{
    SendData3[0]=STX;
    SendData3[1]=0x89;
    SendData3[2]=CardReader_Addr[0];
    SendData3[3]=CardReader_Addr[1];
    SendData3[4]=0x30+NZL;
    SCPArray[3]->writeRFIDData(SendData3,5);
}


void currentTime(quint8 NZL)
{

    if(tag_Random_flg==1){
        for(int i=0;i<4;i++){
            QDateTime dateTime=QDateTime::currentDateTime();
            QString timeString=dateTime.toString("ss");
            time_Random=timeString.toInt();
            qsrand(time_Random);
        }
        for(int i=0;i<4;i++) {
            RandomBuffer[i]=RandomArray[qrand()%10]+time_Random;
            pump[pumpNo].nozzle[NZL].Coding_Key[i]=RandomBuffer[i];
        }
    }
}




void ScpRoutine::Message_parsing()
{
    request_flg=1;
    int parse_index;
    quint8 nn;
    parse_index=0;
    if(ReadTempData2[last_index]==0xFA) // FA check
    {

        if(ReadTempData2[0]==siteControler_Addr) // Adress check
        {

            //Check ACK
            if((ReadTempData2[1]>=0xC0)&&(ReadTempData2[1]<=0xCF)){
                ack_receive_flg=true;


                if(Send_authorize_flg==true)
                {
                    Send_authorize_flg=false;
                    Preset_Ack_flg=true;
                }

                if(update_message_sent==true)
                {
                    update_message_sent=false;
                    Update_flg=false;
                }
                if(ask_last_screen==true)
                {
                    ask_last_screen=false;
                    Volumetotal_received=false;
                    Amounttotal_received=false;
                    if(routinedebug)qDebug()<<"son-ekran-alındı"<<Volumetotal_received<<Amounttotal_received;
                    ask_nozzle_total_volume=true; //total sor
                    End_total_report=true;
                    Filling_comp_flg=1;
                    if(ask_km_after==false)
                    {
                        if(USC_is_exist==false)
                        {
                            RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+2,1,"0");
                            //qDebug()<<"RFID_Read_Time-paid_confirm"<<RFID_Read_Time;
                            Paid_Confirm=true;
                            Paid_Confrim_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                            Paid_Confrim_Time.prepend("20");
                            Paid_Fiscal_Time=Paid_Confrim_Time;
                            //qDebug()<<"Paid_Confrim_UBK"<<Paid_Confrim_Time;
                            ////syslog(LOG_DEBUG,"SATIS_DEBUG USC-FALSE-Paid_Confrim %s",Paid_Confrim_Time.toLocal8Bit().constData());
                            settings.setlog("SATIS_DEBUG USC-FALSE-Paid_Confrim : "+Paid_Confrim_Time);
                        }
                    }
                }
            }

            //Check EOT
            if((ReadTempData2[1]>=0x70)&&(ReadTempData2[1]<=0x7F)){
                eot_receive_flg=true;
            }

            //Check NAK
            if((ReadTempData2[1]>=0x50)&&(ReadTempData2[1]<=0x5F))
            {
                nak_receive_flg=true;

                if(Send_authorize_flg==true)
                {
                    Preset_Ack_flg=false;
                }

                if(update_message_sent==true)
                {
                    update_message_sent=false;
                    Update_flg=true;
                }


            }

            //Check DATA
            if((ReadTempData2[1]>=0x30)&&(ReadTempData2[1]<=0x3F)){
                data_receive_flg=true;

                // check CRC value
                if((ReadTempData2[last_index-2]==0xfa) && (ReadTempData2[last_index-3]==0x10)){

                    switch (select_version) {
                    case 1:calcrc(ReadTempData2.data(),last_index-4,0); // not crypted
                        break;
                    case 2:calcrc(ReadTempData2.data(),last_index-4,0x6204); // Different CRC
                        break;
                    case 3:calcrc(ReadTempData2.data(),last_index-4,0); //crypted
                        break;
                    default:
                        break;
                    }

                    crc_lsb_ind=last_index-4;
                    crc_msb_ind=last_index-2;
                    parse_index=crc_lsb_ind-1;
                }
                else if((ReadTempData2[last_index-3]==0xfa)&&(ReadTempData2[last_index-4]==0x10))
                {
                    switch (select_version) {
                    case 1:calcrc(ReadTempData2.data(),last_index-4,0); // not crypted
                        break;
                    case 2:calcrc(ReadTempData2.data(),last_index-4,0x6204); // Different CRC
                        break;
                    case 3:calcrc(ReadTempData2.data(),last_index-4,0); //crypted
                        break;
                    default:
                        break;
                    }
                    crc_lsb_ind=last_index-3;
                    crc_msb_ind=last_index-2;
                    parse_index=crc_lsb_ind-1;

                }
                else if((ReadTempData2[last_index-2]==0xfa)&&(ReadTempData2[last_index-3]==0x10) &&(ReadTempData2[last_index-4]==0x10) &&(ReadTempData2[last_index-5]==0x10))
                {

                    switch (select_version) {
                    case 1:calcrc(ReadTempData2.data(),last_index-4,0); // not crypted
                        break;
                    case 2:calcrc(ReadTempData2.data(),last_index-4,0x6204); // Different CRC
                        break;
                    case 3:calcrc(ReadTempData2.data(),last_index-4,0); //crypted
                        break;
                    default:
                        break;
                    }

                    crc_lsb_ind=last_index-4;
                    crc_msb_ind=last_index-2;
                    parse_index=crc_lsb_ind-3;

                }
                else
                {
                    switch (select_version) {
                    case 1:calcrc(ReadTempData2.data(),last_index-3,0); // not crypted
                        break;
                    case 2:calcrc(ReadTempData2.data(),last_index-3,0x6204); // Different CRC
                        break;
                    case 3:calcrc(ReadTempData2.data(),last_index-3,0); //crypted
                        break;
                    default:
                        break;
                    }

                    crc_lsb_ind=last_index-3;
                    crc_msb_ind=last_index-2;
                    parse_index=crc_lsb_ind;
                }

                token=ReadTempData2[1]-0x30;



                if((crc_lsb==ReadTempData2[crc_lsb_ind])&&(crc_msb==ReadTempData2[crc_msb_ind])) // CRC is true
                {
                    data_lenght=2;
                    while(data_lenght<parse_index)
                    {

                        switch (ReadTempData2[data_lenght]) {

                        case 0x01: {                                 //Pump Status
                            DCR_par.DCR_status=ReadTempData2[data_lenght+2];
                            request_flg=2;
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            pumpStatus();
                            //qDebug()<<">>>>>>>>>>>>>>>><pump-status"<<DCR_par.DCR_status;
                        } break;
                        case 0x02:                                //Filled Volume and Amount
                        { for(int i=(data_lenght+2);i<8;i++)
                                DCR_par.DCR_volume[i-4]=ReadTempData2[i];
                            for(int i=(data_lenght+6);i<12;i++){
                                DCR_par.DCR_amount[i-8]=ReadTempData2[i];
                            }
                            if(DCR_par.DCR_amount>=temp_Amount){
                                temp_Amount=DCR_par.DCR_amount;
                            }
                            if(DCR_par.DCR_volume>=temp_Volume){
                                temp_Volume=DCR_par.DCR_volume;
                            }
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount=temp_Amount;
                            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume=temp_Volume;

                            convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount,4,amount_dot_index);
                            QByteArray amount_screen=printer_temp_buff;
                            convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume,4,volume_dot_index);
                            QByteArray volume_screen=printer_temp_buff;
                            qDebug()<<"filling";
                            if(Pump_Filling_status==1)
                            {
                                customerCardLock=false;
                                qDebug()<<"authorized";
                                settings.setValue("setting_nozzle_number",DCR_par.DCR_Nozzle_Number);
                                amount_screen=amount_screen.replace(".",",");
                                volume_screen=volume_screen.replace(".",",");
                                settings.setVolumeAmount(amount_screen,volume_screen,2,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Product_name);

                            }
                            if(routinedebug)qDebug()<<"filling_amount-filling_volume"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount.toHex()<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume.toHex();
                            if((Pump_Filling_status==1) && (Nozzle_up_flg==1) && (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount!=0)){DCR_par.DCR_status=0x04;}

                            //                            if(Filling_comp_flg==1)
                            //                            {
                            //                                filling_info_ok=1;
                            //                                qDebug()<<"filling_Info_OK";
                            //                            }

                            LCD_refresh_flg=1;
                            request_flg=2;
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;
                        }
                        case 0x03:                                    //Nozzle Status and filling price
                            if(test) qDebug()<<ReadTempData2.toHex();
                            DCR_par.DCR_Nozzle_Info=ReadTempData2[data_lenght+5];
                            if(test) qDebug()<<">>DCR_par.DCR_Nozzle_Info<<1"<<(DCR_par.DCR_Nozzle_Info & 0x0F)<<ReadTempData2.toHex()<<protect_flg<<Nozzle_up_flg;
                            if((DCR_par.DCR_Nozzle_Info & 0x0F)!=0)
                            {

                                if((Nozzle_up_flg!=1) && (protect_flg==false))
                                {

                                    DCR_par.DCR_Nozzle_Number=DCR_par.DCR_Nozzle_Info & 0x0F;
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price[0]=ReadTempData2[data_lenght+2];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price[1]=ReadTempData2[data_lenght+3];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price[2]=ReadTempData2[data_lenght+4];
                                }
                                if(Nozzle_up_flg==1)
                                {

                                    DCR_par.DCR_Nozzle_Number=DCR_par.DCR_Nozzle_Info & 0x0F;
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price[0]=ReadTempData2[data_lenght+2];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price[1]=ReadTempData2[data_lenght+3];
                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_price[2]=ReadTempData2[data_lenght+4];
                                }

                                if((DCR_par.DCR_Nozzle_Info>>4>0) && (protect_flg==false))
                                {

                                    if(Nozzle_up_flg!=1)
                                    {

                                        if(DCR_par.DCR_Nozzle_Number>Number_of_Nozzle)
                                        {
                                            StoredScreenRFID_fromMaster=8;
                                            TEXT_fromMaster=invalidNozzle_String.toUtf8();
                                        }
                                        else
                                        {

                                            //StoredScreenRFID_fromMaster=1; //sonradan
                                            Allow_card_read=1;
                                            clrc_up_flg=1;
                                            Nozzle_Up_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                                            Nozzle_Up_Time.prepend("20");
                                            QString tempSTR=nozzle_String+"-"+QString::number(DCR_par.DCR_Nozzle_Number);
                                            qDebug()<<"NOZZLE UP";
                                            wrong_product=false;
                                            passive_id=false;
                                            deleted_id=false;
                                            database->NozzleValues();
                                            settings.setPriceStatus(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].unitprice.toHex(),1);
                                            settings.setVolumeAmount("0,00","0,00",1,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Product_name);
                                            if(pump_sta_2!=true){ask_nozzle_total_volume=true;}
                                            TEXT_fromMaster=tempSTR.toLatin1();
                                            settings.setlog("-----------NOZZLE_UP_Time-------"+Nozzle_Up_Time+nozzle_String+QString::number(DCR_par.DCR_Nozzle_Number));
                                            if(test)qDebug()<<"Nozzle_Up_Time"<<Nozzle_Up_Time<<DCR_par.DCR_Nozzle_Number<<DCR_par.DCR_Nozzle_Info;

                                            if((USC_is_exist==false) || (USC_com_error_flg==true))
                                            {
                                                USC_com_lock=true;
                                            }
                                            else
                                            {
                                                USC_com_lock=false;
                                            }
                                            if(plate_entered_with_button==true)
                                            {
                                                for(int pl=0; pl<12;pl++)
                                                {
                                                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                                                }
                                                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=pump[pumpNo].PLATE;
                                            }
                                            plate_enter_count=0;
                                            USC_ok_read_count=0;
                                            tag_read_count=0;
                                            Tag_read_zero_count=0;
                                            Keypad_data_for_LCD.clear();
                                            ReadTAG.clear();
                                            pump[pumpNo].KM.clear();
                                            vehiclekmtr.clear();
                                            for(int jj=0; jj<16;jj++)
                                            {
                                                TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                                                databuf[jj]=0;
                                            }
                                            Temp_TAGID.clear();
                                        }

                                    }


                                    Nozzle_up_flg=1;
                                    qDebug()<<Card_read_flg<<"hadiir"<<customerCardLock;

                                    if(Pump_Filling_status!=1 && customerCardLock==false) //DOLUMDA DEĞİLSE
                                    {//qDebug()<<Pump_Filling_status<<"hadii2";
                                        if(USC_is_exist==false)
                                        {
                                            if(reboot_ok!=true && StoredScreenRFID_fromMaster!=8)
                                            {
                                                qDebug()<<"NOZZLE UP-1";
                                                //StoredScreenRFID_fromMaster=1;
                                                Allow_card_read=1;
                                                if(limit_error==false) {if(Confirmation_flg!=1 && Card_read_flg==false) StoredScreenRFID_fromMaster=1; }
                                            }
                                        }
                                    }
                                    LCD_refresh_flg=1;
                                    request_flg=2;

                                }

                                if(DCR_par.DCR_Nozzle_Info>>4==0)
                                {

                                    DCR_par.DCR_Nozzle_Number=DCR_par.DCR_Nozzle_Info & 0x0F;
                                    Allow_card_read=0;
                                    LCD_refresh_flg=1;
                                    request_flg=2;
                                    if(Nozzle_up_flg==1)
                                    {
                                        qDebug()<<"DOWNNNNNNNNNNNNNNNNNNNNNN";
                                        customerCardLock=false;
                                        Nozzle_Down_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                                        Nozzle_Down_Time.prepend("20");
                                        if(test)qDebug()<<"Nozzle_Down_Time"<<Nozzle_Down_Time;

                                        settings.setlog("------------SATIS_DEBUG NOZZLE_DOWN_TIME------"+Nozzle_Down_Time+nozzle_String+":"+QString::number(DCR_par.DCR_Nozzle_Number) );

                                        convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount,4,amount_dot_index);
                                        QByteArray amount_screen=printer_temp_buff;
                                        convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume,4,volume_dot_index);
                                        QByteArray volume_screen=printer_temp_buff;
                                        amount_screen=amount_screen.replace(".",",");
                                        volume_screen=volume_screen.replace(".",",");
                                        settings.setVolumeAmount(amount_screen,volume_screen,0,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Product_name);

                                        Confirmation_flg=0;
                                        Wrong_trans=false;
                                        card_authorize=false;
                                        Not_resume_TAG=false;
                                        Person_wait_flg=false;
                                        Driver_wait_flg=false;
                                        Confirmation_flg_without_km=false;
                                        Send_authorize_flg_2=false;
                                        cannotchange_frame=false; ///sonradan
                                        TEXT_fromMaster.clear();
                                        if(Pump_Filling_status!=1)
                                        {
                                            if(test) qDebug()<<"Nozzle Down and No Filling";
                                            StoredScreenRFID_fromMaster=7;
                                            //if(meter_last_error==false)
                                            //{

                                            protect_flg=false;
                                            USC_com_lock=false;
                                            Customer_plate.clear();
                                            Keypad_data_for_LCD.clear();
                                            CustomerID.clear();
                                            Temp_CustomerID.clear();
                                            qDebug()<<"cl2";
                                            vehiclekmtr.clear();
                                            Saved_Customer_ID.clear();
                                            Saved_Driver_ID.clear();
                                            Saved_Person_ID.clear();
                                            ID_count=0;
                                            Cancel_flg=1;
                                            Enter_flg=0;
                                            tag_read_count=4;
                                            Tag_read_count_flg=0;
                                            Tag_read_flg=0;

                                            for(nn=0; nn<=Number_of_Nozzle; nn++)
                                            {
                                                pump[pumpNo].nozzle[nn].Cyrpto_key_from_master.clear();
                                            }
                                            for(int jj=0; jj<16;jj++)
                                            {
                                                TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                                                databuf[jj]=0;
                                            }

                                            ReadTAG.clear();
                                            //}
                                            //meter_last_error=false;
                                            if(LCD_status!=1 && LCD_status!=12 && LCD_status!=14 && LCD_status!=15 && LCD_status!=16 && LCD_status!=17){
                                                delete_last_sale_flg=1;
                                            }

                                        }

                                    }
                                    if(system_started==false)
                                    {
                                        if(pump_sta==true) //satıştayken program kapandıysa
                                        {
                                            ask_last_screen=true;
                                            if(test)qDebug()<<"kapandıktan sonra satış bitirme"<<pump_sta;
                                            settings.setlog("SATIS_DEBUG UBK-POMPA SATIS KURTARMA");

                                            pump_sta=false;

                                            LCD_status=1;
                                            LCD_refresh_flg=1;
                                        }
                                    }

                                    if(Pump_Filling_status==1)
                                    {
                                        DCR_par.DCR_status=0x05;
                                    }
                                    Nozzle_up_flg=0;
                                    clrc_up_flg=0;
                                    if(DCR_par.DCR_status!=0x01){ pumpStatus();}
                                }
                            }
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x05:                                   // Alarm Code
                            DCR_par.DCR_Alarm_Code=ReadTempData2[data_lenght+2];
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            request_flg=2;
                            break;

                        case 0x07:                                     // Pump Parameters

                            for(int i=0;i<22;i++){
                                DCR_par.DCR_Res1[i]=ReadTempData2[i+(data_lenght+2)];
                            }
                            DCR_par.DCR_DPVol=ReadTempData2[data_lenght+24];
                            DCR_par.DCR_DPAmount=ReadTempData2[data_lenght+25];
                            DCR_par.DCR_DPunp=ReadTempData2[data_lenght+26];
                            for(int i=0;i<5;i++){
                                DCR_par.DCR_Res2[i]=ReadTempData2[i+(data_lenght+27)];
                            }
                            for(int i=0;i<4;i++){
                                DCR_par.DCR_Mamo[i]=ReadTempData2[i+data_lenght+32];
                            }
                            DCR_par.DCR_Res3[0]=ReadTempData2[data_lenght+36];
                            DCR_par.DCR_Res3[1]=ReadTempData2[data_lenght+37];
                            for(int i=0;i<15;i++){
                                DCR_par.DCR_Grade[i]=ReadTempData2[i+data_lenght+38];
                            }
                            request_flg=2;
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x09:                                          // Pump Identity

                            DCR_par.DCR_PID[0]=ReadTempData2[data_lenght+2];
                            DCR_par.DCR_PID[1]=ReadTempData2[data_lenght+3];
                            DCR_par.DCR_PID[2]=ReadTempData2[data_lenght+4];
                            DCR_par.DCR_PID[3]=ReadTempData2[data_lenght+5];
                            DCR_par.DCR_PID[4]=ReadTempData2[data_lenght+6];
                            request_flg=2;
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x0E:                                           // Suspend Reply
                            if(test) qDebug()<<">>DCR_par.DCR_Nozzle_Info<<2"<<ReadTempData2[data_lenght+2]<<ReadTempData2.toHex();
                            //if(ReadTempData2[data_lenght+2]!=0)
                            //{
                            //DCR_par.DCR_Nozzle_Number=ReadTempData2[data_lenght+2];
                            SuspendReply=true;
                            ResumeReply=false;
                            if(test) qDebug()<<">>ssssssssssssssssssssssSuspendReply";
                            request_flg=2;
                            //}
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x0F:                                            // Resume Reply
                            if(test) qDebug()<<">>DCR_par.DCR_Nozzle_Info<<3"<<ReadTempData2[data_lenght+2]<<ReadTempData2.toHex();
                            //if(ReadTempData2[data_lenght+2]!=0)
                            //{
                            //    DCR_par.DCR_Nozzle_Number=ReadTempData2[data_lenght+2];
                            SuspendReply=false;
                            ResumeReply=true;
                            request_flg=2;
                            //}
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x65:                                           // Volume Total Counters

                            DCR_par.DCR_Count=ReadTempData2[data_lenght+2];
                            if(DCR_par.DCR_Count<0x10)
                            {
                                for(int i=0;i<5;i++){
                                    DCR_par.DCR_TotVol[i]=ReadTempData2[i+(data_lenght+3)];
                                }
                                DCR_par.DCR_TotVol.remove(0,1);
                                if(routinedebug)qDebug()<<"volume-total"<<End_total_report<<pump_sta<<pump_sta_2;
                                if((End_total_report==false) && (pump_sta!=true)&&(pump_sta_2!=true))//
                                {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Volume=DCR_par.DCR_TotVol;
                                    if(routinedebug)qDebug()<<"başlangıc total alındı";
                                }
                                else
                                {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Volume=DCR_par.DCR_TotVol;
                                    if(routinedebug)qDebug()<<"bitis total alındı";
                                    Volumetotal_received=true;
                                }
                                pump[pumpNo].nozzle[DCR_par.DCR_Count].USC_T_Volume=DCR_par.DCR_TotVol;

                                if((settings.value("Begin_total_volume_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Volume.toHex()) || (settings.value("End_total_volume_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Volume.toHex()) || (settings.value("End_total_volume_USC_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].USC_T_Volume.toHex()))
                                {
                                    if(routinedebug)qDebug()<<"save-total-volume";
                                    saveVolumeTotal(pumpNo,DCR_par.DCR_Count);
                                }
                                request_flg=2;

                                //qDebug()<<"TOTAL-BEGIN-VOLUME-NOZZZLE:"+QString::number(DCR_par.DCR_Count)<<pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Volume.toHex()<<End_total_report;
                                //qDebug()<<"TOTAL-END-VOLUME-NOZZZLE:"+QString::number(DCR_par.DCR_Count)<<pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Volume.toHex()<<End_total_report;
                                ////syslog(LOG_DEBUG,"SCP_DEBUG TOTAL-VOLUME-NOZZZLE %d %s",DCR_par.DCR_Count,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Volume.toHex().constData());
                                settings.setlog("SCP_DEBUG TOTAL-VOLUME-NOZZZLE : "+QString::number(DCR_par.DCR_Count)+"    "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Volume.toHex()));
                                data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                                ask_nozzle_total_amount=true;
                            }
                            else
                            {
                                if((ReadTempData2[data_lenght+2] & 0X0F)!=0)
                                {
                                    DCR_par.DCR_Count=ReadTempData2[data_lenght+2] & 0X0F;

                                    for(int i=0;i<5;i++){
                                        DCR_par.DCR_TotAmo[i]=ReadTempData2[i+(data_lenght+3)];
                                    }
                                    DCR_par.DCR_TotAmo.remove(0,1);
                                    if(routinedebug)qDebug()<<"volume-amount"<<End_total_report<<pump_sta<<pump_sta_2;
                                    if((End_total_report==false) && (pump_sta!=true) && (pump_sta_2!=true))
                                    {
                                        pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Amount=DCR_par.DCR_TotAmo;
                                    }
                                    else
                                    {

                                        pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Amount=DCR_par.DCR_TotAmo;//?????Quantity eksik
                                        Amounttotal_received=true;

                                    }
                                    pump[pumpNo].nozzle[DCR_par.DCR_Count].USC_T_Amount=DCR_par.DCR_TotAmo;
                                    if((settings.value("Begin_total_amount_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Amount.toHex()) || (settings.value("End_total_amount_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Amount.toHex()) || (settings.value("End_total_amount_USC_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].USC_T_Amount.toHex()))
                                    {
                                        if(routinedebug)qDebug()<<"save-total-amount-2";
                                        saveAmountTotal(pumpNo,DCR_par.DCR_Count);
                                    }
                                    request_flg=2;

                                    if(test)qDebug()<<"TOTAL-AMOUNT-NOZZZLE:"+QString::number(DCR_par.DCR_Count)<<pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Amount.toHex();
                                    ////syslog(LOG_DEBUG,"SCP_DEBUG TOTAL-AMOUNT-NOZZZLE %d %s",DCR_par.DCR_Count,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Amount.toHex().constData());
                                    settings.setlog("SCP_DEBUG TOTAL-AMOUNT-NOZZZLE : "+QString::number(DCR_par.DCR_Count)+"    "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Amount.toHex()));

                                }
                                data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;

                            }

                            break;
                        case 0x66:                                              // Amaunt Total Counters
                            if(test) qDebug()<<">>DCR_par.DCR_Nozzle_Info<<4"<<ReadTempData2[data_lenght+2]<<ReadTempData2.toHex();
                            if(ReadTempData2[data_lenght+2]!=0)
                            {
                                DCR_par.DCR_Count=ReadTempData2[data_lenght+2];
                                for(int i=0;i<5;i++){
                                    DCR_par.DCR_TotAmo[i]=ReadTempData2[i+(data_lenght+3)];
                                }
                                DCR_par.DCR_TotAmo.remove(0,1);
                                if(routinedebug)qDebug()<<"volume-amount"<<End_total_report<<pump_sta<<pump_sta_2;
                                if((End_total_report==false) && (pump_sta!=true)&& (pump_sta_2!=true))
                                {
                                    pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Amount=DCR_par.DCR_TotAmo;
                                }
                                else
                                {

                                    pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Amount=DCR_par.DCR_TotAmo;//?????Quantity eksik
                                    Amounttotal_received=true;
                                }
                                if((settings.value("Begin_total_amount_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].BT_Amount.toHex()) || (settings.value("End_total_amount_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Amount.toHex()) || (settings.value("End_total_amount_USC_"+QString::number(DCR_par.DCR_Count))!=pump[pumpNo].nozzle[DCR_par.DCR_Count].USC_T_Amount.toHex()))
                                {
                                    if(routinedebug)qDebug()<<"save-total-Amount";
                                    saveAmountTotal(pumpNo,DCR_par.DCR_Count);
                                }
                                request_flg=2;

                                if(test)qDebug()<<"TOTAL-AMOUNT-NOZZZLE:"+QString::number(DCR_par.DCR_Count)<<pump[pumpNo].nozzle[DCR_par.DCR_Count].T_Amount.toHex();
                                ////syslog(LOG_DEBUG,"SCP_DEBUG TOTAL-AMOUNT-NOZZZLE %d %s",DCR_par.DCR_Count,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Amount.toHex().constData());
                                settings.setlog("SCP_DEBUG TOTAL-AMOUNT-NOZZZLE : "+QString::number(DCR_par.DCR_Count)+"    "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].T_Amount.toHex()));

                            }
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x67:                                                 // Temperatures
                            if(test) qDebug()<<">>DCR_par.DCR_Nozzle_Info<<5"<<ReadTempData2[data_lenght+2]<<ReadTempData2.toHex();
                            if(ReadTempData2[data_lenght+2]!=0)
                            {
                                DCR_par.DCR_Nozzle_Number=ReadTempData2[data_lenght+2];
                                DCR_par.DCR_Temp[0]=ReadTempData2[data_lenght+3];
                                DCR_par.DCR_Temp[1]=ReadTempData2[data_lenght+4];
                                request_flg=2;
                            }
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;

                        case 0x68:                                                     // RFTAG
                            if(test) qDebug()<<">>DCR_par.DCR_Nozzle_Info<<6"<<ReadTempData2[data_lenght+2]<<ReadTempData2.toHex();
                            if(ReadTempData2[data_lenght+2]!=0)
                            {
                                DCR_par.DCR_Nozzle_Number=ReadTempData2[data_lenght+2];
                                for(int i=0;i<12;i++){
                                    DCR_par.DCR_RFTag[i]=ReadTempData2[i+(data_lenght+3)];
                                }
                                request_flg=2;
                            }
                            data_lenght=data_lenght+ReadTempData2[data_lenght+1]+2;
                            break;
                        default:
                            if(test){qDebug()<<"Pompa Bilinmeyen Mesaj Tipi"<<ReadTempData2.toHex();}
                            ////syslog(LOG_DEBUG,"SCP_DEBUG Pompa Bilinmeyen Mesaj Tipi %s",ReadTempData2.toHex().constData());
                            settings.setlog("SCP_DEBUG Pompa Bilinmeyen Mesaj Tipi : "+QString::fromUtf8(ReadTempData2.toHex()));

                            request_flg=7;
                            break;
                        }
                    }

                }
            }

        }

    }
}

void ScpRoutine::Change_Pump_Sta()
{

    //qDebug()<<"---------------------DCR-Pump Status------------------"<<pump[pumpNo].status;
    //qDebug()<<"filling_amount-filling_volume"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount.toHex()<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume.toHex()<<nozzleNo;
    if((Nozzle_up_flg==1) &&(Pump_Filling_status!=1)){pump[pumpNo].status=1;}//call
    if(DCR_par.DCR_status==02){pump[pumpNo].status=6;}//Authorized
    if(DCR_par.DCR_status==04){if((pump[pumpNo].status!=2) && (pump[pumpNo].status!=3)){save_TS_Time_flg=true;} pump[pumpNo].status=2;LCD_status=8; LCD_refresh_flg=1; }//Busy  (pump[pumpNo].status!=3) pompadan pausede busy geliyo
    if((SuspendReply==true) && (Nozzle_up_flg==1)){pump[pumpNo].status=3; LCD_status=10; LCD_refresh_flg=1; Tag_read_flg=0;} //pause //Tag_read_flg=0; usc bağlantı kesildikten sonra tag resume olabilsin diye
    if((ResumeReply==true)&& (Nozzle_up_flg==1)){ResumeReply=false; pump[pumpNo].status=2;LCD_status=8; LCD_refresh_flg=1;} //resume
    if((Filling_comp_flg==1)&&(ask_last_screen==false)&&((Amounttotal_received==true)&&(Volumetotal_received==true)))
    {
        if(ask_km_after==false)
        {
            if(routinedebug)qDebug()<<"UNpaid-olduk";
            pump[pumpNo].status=4;LCD_status=5; LCD_refresh_flg=1;
            if((USC_is_exist==false) ||(USC_com_error_flg==true) ) ///sonradann
            {
                Paid_Confirm=true;
                RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+2,1,"0");
                //qDebug()<<"RFID_Read_Time-paid_confirm"<<RFID_Read_Time;
                Paid_Confrim_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                Paid_Confrim_Time.prepend("20");
                Paid_Fiscal_Time=Paid_Confrim_Time;
                if(test)qDebug()<<"Paid_Confrim_Time"<<Paid_Confrim_Time;
                ///syslog(LOG_DEBUG,"SATIS_DEBUG USC-FALSE-Paid_Confrim %s",Paid_Confrim_Time.toLocal8Bit().constData());
            }
        }
        else if(ask_km_after==true)
        {
            if((pump[pumpNo].status!=4) && (wait_km_flg!=true))
            {
                pump[pumpNo].status=7;
                LCD_status=11; // km giriniz
                LCD_refresh_flg=1;
                wait_km_flg=true;
            }
        }
    } //unpaid

    if(Change_status_to_IDLE==true)
    {
        if(test)qDebug()<<"Change_status_to_IDLE";
        Change_status_to_IDLE=false; only_card_read_flg=false; pump[pumpNo].status=0; LCD_status=1; LCD_refresh_flg=1;
        SuspendReply=false;
        if(USC_is_exist==false)
        {
            StoredScreenRFID_fromMaster=9;
        }
        limit_error=false;
        if(test)qDebug()<<"TEXT_fromMaster.clear"<<TEXT_fromMaster.length()<<TEXT_fromMaster;
        for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
        {
            TEXT_fromMaster[jj]=0x20;
        }
        NewPlateExist=0;
        Plate_control_flg=0;
        KM_Confirmation_flg=false;
        Confirmation_flg_ok=false;
        plate_entered_with_button=false;
        Card_read_flg=0;
        TAG_read=false;
    } //IDLE

    if(Cancel_flg==1)
    {
        if(test)qDebug()<<"Change_status_to_IDLE";
        settings.setlog("SCP_DEBUG Change_status_to_IDLE");
        if(USC_is_exist==false)
        {
            StoredScreenRFID_fromMaster=9;
        }

        Cancel_flg=0; pump[pumpNo].status=0; LCD_status=1; LCD_refresh_flg=1;
        for(int jj=0; jj<16;jj++)
        {
            TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
            databuf[jj]=0;
        }

        SuspendReply=false;
        limit_error=false;
        only_card_read_flg=false;
        for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
        {
            TEXT_fromMaster[jj]=0x20;
        }
        NewPlateExist=0;
        KM_Confirmation_flg=false;
        Confirmation_flg_ok=false;
        plate_entered_with_button=false;
        Plate_control_flg=0;
        Card_read_flg=0;
        ID_count=0;
        TAG_read=false;
        Saved_Customer_ID.clear();
        Saved_Driver_ID.clear();
        Saved_Person_ID.clear();
        Temp_ID[0].clear();
        Temp_ID[1].clear();
        Temp_ID[2].clear();


    }//IDLE
    if((USC_com_error_flg==true) && (USC_is_exist==true)){LCD_status=15; LCD_refresh_flg=1;}
    // qDebug()<<"---------------------DCR-Pump Status------------------"<<pump[pumpNo].status;
}

void ScpRoutine::decode_meter(QByteArray data)
{
    int j,msglen,msglen2;
    char i = 0, k = 0;
    // qDebug()<<"Decode Tag ID";
    Delta_meter.Byte[3]=Coding_key[0];
    Delta_meter.Byte[2]=Coding_key[1];
    Delta_meter.Byte[1]=Coding_key[2];
    Delta_meter.Byte[0]=Coding_key[3];

    union aBffr_t {
        unsigned long ByteArray[4];
        unsigned char Byte[32];
    } aBffr;

    unsigned long vw[4] = {0x32464741, 0x11131519, 0x26232528, 0x42454649};
    unsigned long vw0 = 0, vw1 = 0, vw2 = 0;


    i = 0;
    k = 0;
    msglen=data.length()/2;
    msglen2=data.length();
    while (i < msglen2) {

        aBffr.Byte[k] = (((unsigned char) data[i] - 48) * 16) + ((unsigned char) data[i+1] - 48);
        i += 2;
        k++;
    }

    j=k/8;
    k=0;
    while(j)
    {
        i=k*2;

        vw0 = (Delta_meter.LongWrd * 2);
        vw1 = aBffr.ByteArray[i+0];
        vw2 = aBffr.ByteArray[i+1];
        while (vw0 != 0) {
            vw2 -= ((((vw1 << 4) ^ (vw1 >> 5)) + vw1) ^ (vw0 + vw[((vw0 >> 11) & 3)]));
            vw0 -= Delta_meter.LongWrd;
            vw1 -= ((((vw2 << 4) ^ (vw2 >> 5)) + vw2) ^ (vw0 + vw[(vw0 & 3)]));
        }
        aBffr.ByteArray[i+0] = vw1;
        aBffr.ByteArray[i+1] = vw2;

        j--;
        k++;
    }
    ReadTempData2.clear();
    for (i = 0; i < msglen; i++) {

        ReadTempData2[i] = aBffr.Byte[i];
    }

    if(ReadTempData2.contains(0xFA))
    {
        //qDebug()<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<meter_coming_crypted_data1"<<ReadTempData2.toHex();
        unsigned int FAindex=ReadTempData2.lastIndexOf(0xFA);
        unsigned int zerocount=ReadTempData2.length()-(FAindex);
        //qDebug()<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<afffaaaaa"<<FAindex<<zerocount;
        //ReadTempData2=ReadTempData2.remove(FAindex,zerocount);
        if(test) qDebug()<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<meter_coming_crypted_data2"<<ReadTempData2.toHex();
    }
    else
    {
        if(test) qDebug()<<"***********************************meter_coming_error_data"<<ReadTempData2.toHex();
        ReadTempData2.clear();
    }
}
void ScpRoutine::read_meter_data()
{

    SendData2.clear();
    ReadData2.clear();
    SCPArray[2]->readMeterData(ReadData2);
    if((select_version==1) || (select_version==2))
    {
        if(ReadData2.length() > 0)
        {
            ReadTempData2.append(ReadData2);
            //qDebug()<<"ReadTempData2"<<ReadTempData2.toHex();
            ReadData2.clear();
            if(ReadTempData2.contains(0xFA))
            {
                //            if(ReadTempData2.length()>60)
                //            {
                //                if(test) qDebug()<<"----------------Datasınırıgeçti-----------------";
                //                ReadTempData2.clear();
                //            }
                //            ReadTempData2.remove(ReadTempData2.length()-1,1 );
                //            if(ReadTempData2.length()%8!=0)
                //            {
                //                if(test) qDebug()<<"------------Datasını8in katı değil--------------";
                //                ReadTempData2.clear();
                //            }
                //            decode_meter(ReadTempData2);
                //                qDebug()<<"ReadTempData2"<<ReadTempData2.toHex();
                last_index=ReadTempData2.lastIndexOf(0xFA);
                ////if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_METER_DEBUG  <<<RECEIVED: %s ",ReadTempData2.toHex().constData());
                Message_parsing();
                send_meter_data_count=0;
                if(meter_com_err==true)
                {
                    LCD_status=LCD_status_last;
                    meter_com_err=false;
                    LCD_refresh_flg=1;
                    if(system_started==false){ if(pump_sta==true) ask_last_screen=true;}
                }
                ReadTempData2.clear();
            }
        }
    }
    else
    {
        if(ReadData2.length() > 0)
        {
            ReadTempData2.append(ReadData2);
            //qDebug()<<"ReadTempData2"<<ReadTempData2.toHex();
            ReadData2.clear();
            if(ReadTempData2.contains(0xFA))
            {
                if(ReadTempData2.length()>60)
                {
                    if(test) qDebug()<<"----------------Datasınırıgeçti-----------------";
                    ReadTempData2.clear();
                }
                ReadTempData2.remove(ReadTempData2.length()-1,1 );
                if(ReadTempData2.length()%8!=0)
                {
                    if(test) qDebug()<<"------------Datasını8in katı değil--------------";
                    ReadTempData2.clear();
                }
                decode_meter(ReadTempData2);
                if(ReadTempData2.contains(0xFA))
                {
                    last_index=ReadTempData2.lastIndexOf(0xFA);
                    //qDebug()<<"<<<<<<<<<<<<"<<ReadTempData2.toHex();
                    ///if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_METER_DEBUG  <<<RECEIVED: %s ",ReadTempData2.toHex().constData());
                    Message_parsing();
                    send_meter_data_count=0;
                    if(meter_com_err==true)
                    {
                        LCD_status=LCD_status_last;
                        meter_com_err=false;
                        LCD_refresh_flg=1;
                        if(system_started==false){ if(pump_sta==true) ask_last_screen=true;}
                    }
                    ReadTempData2.clear();
                }
                else
                {
                    ReadTempData2.clear();
                }

            }

        }

    }

}


void ScpRoutine::routine_2()
{
    ///50 C0 FA  metre bekliyo
    ///


    dcr_ini_flg=true;
    if(dcr_ini_flg==false){

        for(int i=1;i<=3;i++){
            SCPArray[2]->send_ini(dcr_ini);
            read_meter_data();//serial-2 read data
        }
        dcr_ini_flg=true;
        send_CD2(0x01);
    }

    if(save_TS_Time_flg==true)
    {

        save_TS_Time_flg=false;
        //QSettings settings(settingPath, QSettings::IniFormat);
        TS_date=QDate::currentDate().toString("yyMMdd");
        TS_time=QDateTime::currentDateTime().toString("HHmmss");
        //qDebug()<<"TS_time"<<TS_time;
        settings.setlog("SCP_DEBUG TS_time "+TS_time);
        Filling_Start_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
        Filling_Start_Time.prepend("20");
        if(test)qDebug()<<"Filling_Start_Time"<<Filling_Start_Time;
        settings.setlog("SCP_DEBUG Filling_Start_Time "+Filling_Start_Time);
        settings.setValue("TS_date",TS_date);
        settings.setValue("TS_time",TS_time);
        settings.setValue("Filling_Start_Time",Filling_Start_Time);
        sync();
        system("sync");
    }
    read_meter_data();//serial- 2 read data

    if(request_flg==1)
    {

        if((ask_nozzle_total_volume==true) && (Pump_Filling_status!=1))
        {
            request_flg=10; //ask_total_volume_value_to_pump
        }

        if((ask_nozzle_total_amount==true) && (Pump_Filling_status!=1))
        {
            request_flg=11; //ask_total_amount_value_to_pump
        }

        if((Update_flg==true) && (Nozzle_up_flg==0))
        {
            Update_flg=false;
            request_flg=9;
        }
        if(ask_km==false){KM_Confirmation_flg=true;}
        if((Nozzle_up_flg==1) &&(Confirmation_flg==1)&& (pump_sta==false) && (USC_com_error_flg==false)&&(KM_Confirmation_flg==true) &&(Confirmation_flg_without_km==false))
        {

            if(Wrong_trans==false)
            {
                if(ask_nozzle_total_volume==true)
                {
                    qDebug()<<"özel müşteri"<<StoredScreenRFID_fromMaster;


                    request_flg=10; //ask_total_volume_value_to_pump
                    ask_nozzle_total_amount=true;
                    qDebug()<<"authorize-volume-total";
                }
                else if(ask_nozzle_total_amount==true)
                {
                    request_flg=11; //ask_total_amount_value_to_pump
                    qDebug()<<"authorize-amount-total"<<ask_nozzle_total_volume;
                }
                else if(Update_flg==true)
                {
                    Update_flg=false;
                    request_flg=9;
                    qDebug()<<"authorize-update-uprice";
                }
                else if( online_flg==true)
                {
                    Confirmation_flg=0;
                    KM_Confirmation_flg=false;
                    plate_entered_with_button=false;
                    request_flg=6; //preset
                    Send_authorize_flg=true;
                    qDebug()<<"authorize-preset";
                }
                else if(online_flg==false)
                {
                    Confirmation_flg=0;
                    KM_Confirmation_flg=false;
                    plate_entered_with_button=false;
                    Payment_Error_code=0; //ödeme USC =>1 UBK öderse =>0
                    if(TS_no>999999){TS_no=0;}
                    TS_no++;  //transaction noyu artır
                    fisno=TS_no;
                    dec_to_bcd(fisno);
                    pump[pumpNo].Fiscal_R_Number[1]=bcd[1];
                    pump[pumpNo].Fiscal_R_Number[0]=bcd[0];
                    saveTsNo();
                    saveID();
                    Preset_Ack_flg=false;
                    Send_authorize_flg=false;
                    request_flg=3; //Authorize
                    Send_authorize_flg_2=true;
                    authorize_count2=1;
                    for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                    {
                        TEXT_fromMaster[jj]=0x20;
                    }
                    if(USC_is_exist==false)
                    {TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;}
                    LCD_refresh_flg=1;

                }
            }
            else
            {
                TEXT_fromMaster=incorrectOperation_ByteArray;
                LCD_refresh_flg=1;
                Person_is_Ok=false;
            }

        }

        if((Preset_Ack_flg==true) &&(authorize_count>=3))
        {
            if(Nozzle_up_flg==1)
            {
                authorize_count=0;
                Payment_Error_code=0; //ödeme USC =>1 UBK öderse =>0
                if(TS_no>999999){TS_no=0;}
                TS_no++;  //transaction noyu artır
                fisno=TS_no;
                dec_to_bcd(fisno);
                pump[pumpNo].Fiscal_R_Number[1]=bcd[1];
                pump[pumpNo].Fiscal_R_Number[0]=bcd[0];
                saveTsNo();
                saveID();
                Preset_Ack_flg=false;
                request_flg=3; //Authorize
                Send_authorize_flg_2=true;
                authorize_count2=1;
                for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                {
                    TEXT_fromMaster[jj]=0x20;
                }
                if(USC_is_exist==false)
                {TEXT_fromMaster=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;}
                LCD_refresh_flg=1;
                temp_Amount.clear();
                temp_Volume.clear();
                for(int nn=0;nn<=3;nn++)
                {
                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount[nn]=0;
                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume[nn]=0;
                }
            }
            else
            {
                Change_status_to_IDLE=true;
                Preset_Ack_flg=false;
                authorize_count=0;
                Pump_Filling_status=0;
                Payment_Error_code=0;
                Tag_read_count_flg=0;
                Tag_read_flg=0;
                qDebug()<<"cl3";
                vehiclekmtr.clear();
                Saved_Customer_ID.clear();
                Saved_Driver_ID.clear();
                Saved_Person_ID.clear();
                ID_count=0;
                USC_com_lock=false;
                for(int nn=0; nn<=Number_of_Nozzle; nn++)
                {
                    pump[pumpNo].nozzle[0].Cyrpto_key_from_master.clear();
                }

                protect_flg=false;
            }

        }

        if((Send_authorize_flg_2==true) && (authorize_count2%4==0)&& (Pump_Filling_status!=1))
        {
            if(Nozzle_up_flg==1)
            {
                authorize_count2=1;
                request_flg=3; //Authorize
                LCD_refresh_flg=1;
                temp_Amount.clear();
                temp_Volume.clear();
                for(int nn=0;nn<=3;nn++)
                {
                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount[nn]=0;
                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume[nn]=0;
                }
            }
            else
            {
                Change_status_to_IDLE=true;
                Send_authorize_flg_2=false;
                authorize_count2=0;
                Payment_Error_code=0;
                Tag_read_count_flg=0;
                Tag_read_flg=0;
                qDebug()<<"cl4";
                vehiclekmtr.clear();
                Saved_Customer_ID.clear();
                Saved_Driver_ID.clear();
                Saved_Person_ID.clear();
                ID_count=0;
                USC_com_lock=false;
                for(int nn=0; nn<=Number_of_Nozzle; nn++)
                {
                    pump[pumpNo].nozzle[0].Cyrpto_key_from_master.clear();
                }

                protect_flg=false;
            }
        }

        if(Sale_suspend_flg==true)
        {
            Sale_suspend_count++;
            if(SuspendReply==true){Sale_suspend_count=0;Sale_suspend_flg=0;}
            if(Sale_suspend_count>=4){Sale_suspend_count=0;Sale_suspend_flg=0;}
            request_flg=4; //suspend
            LCD_refresh_flg=1;
        }

        if((Wrong_trans==true) && (Pump_Filling_status==true))
        {
            Wrong_trans=false;
            Wrong_trans_for_save=true;
            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1)
            {
                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP+20;
            }
            save_pump_status();
            Not_resume_TAG=true;
            Sale_suspend_flg=true;
            TEXT_fromMaster=fillingFailReadTag_ByteArray;
        }



        if(Sale_resume_flg==true)
        {
            Sale_resume_flg=false;
            request_flg=5; //resume
            LCD_refresh_flg=1;
        }

        if(ask_last_screen==true)
        {
            request_flg=8;
        }
    }
    Change_Pump_Sta();
    switch (request_flg) {
    case 1:
        send_poll();
        request_flg=0;
        break;
    case 2:
        send_ack();
        request_flg=0;
        break;
    case 3:
        send_CD1(0x06); //Authorize
        request_flg=0;
        Authorize_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
        Authorize_Time.prepend("20");
        protect_flg=true;
        DCR_par.Active_Nozzle=DCR_par.DCR_Nozzle_Number;
        if(test)qDebug()<<"Authorize_Time"<<Authorize_Time;
        settings.setlog("SATIS_DEBUG Authorize_Time "+Authorize_Time);
        break;
    case 4:
        if((select_version==1) || (select_version==2))
        {
            send_CD1(0x0B);
        }
        else
        {
            send_CD14(DCR_par.DCR_Nozzle_Number); //suspend
        }
        request_flg=0;
        break;
    case 5:
        if((select_version==1) || (select_version==2))
        {
            send_CD1(0x0C);
        }
        else
        {
            send_CD15(DCR_par.DCR_Nozzle_Number); //resume
        }
        request_flg=0;
        break;
    case 6:
        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP==1){send_CD4(pumpNo,DCR_par.DCR_Nozzle_Number);} //amount preset
        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP==2){send_CD3(pumpNo,DCR_par.DCR_Nozzle_Number);} //volume preset
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PTYP=0;
        request_flg=1;
        break;
    case 7:
        send_nak();
        break;
    case 8:
        send_CD1(0x04);
        request_flg=0;
        if(routinedebug){if(ask_last_screen==true)qDebug()<<"son-ekran-sor";}
        break;
    case 9:
        send_CD5();
        update_message_sent=true;
        request_flg=0;
        break;
    case 10:
        if(routinedebug)qDebug()<<"Ask_Totalvolume_to_pump"<<End_total_report<<total_nozzle_number<<DCR_par.DCR_Nozzle_Number;
        ///syslog(LOG_DEBUG,"SATIS_DEBUG Ask_Totalvolume_to_pump %d %d %d",End_total_report,total_nozzle_number,DCR_par.DCR_Nozzle_Number);
        if(End_total_report==true)
        {
            send_CD101(DCR_par.Active_Nozzle);
        }
        else
        {
            send_CD101(total_nozzle_number);
        }
        ask_nozzle_total_volume=false;
        request_flg=0;
        break;
    case 11:
        if(test)qDebug()<<"Ask_Totalamountl_to_pump"<<End_total_report<<total_nozzle_number<<DCR_par.DCR_Nozzle_Number;
        ///syslog(LOG_DEBUG,"SATIS_DEBUG Ask_Totalamountl_to_pump %d %d %d",End_total_report,total_nozzle_number,DCR_par.DCR_Nozzle_Number);

        if(End_total_report==true)
        {
            send_CD102(DCR_par.DCR_Nozzle_Number);
        }
        else
        {
            send_CD102(total_nozzle_number);
        }
        ask_nozzle_total_amount=false;
        request_flg=0;
        break;
    default:
        break_message_count++;
        if(break_message_count>=2)
        {break_message_count=0;request_flg=1;}
        break;
    }

}

void ScpRoutine::save_pump_status()
{
    if(test) qDebug()<<"save_pump_status"<<DCR_par.DCR_Nozzle_Number<<DCR_par.Active_Nozzle;
    //QSettings settings(settingPath, QSettings::IniFormat);
    //qDebug()<<"pump_sta"<<Pump_Filling_status;
    QString tag[21];
    QVariant value[21];

    tag[0]="pump_sta";
    value[0]=Pump_Filling_status;

    tag[1]="TS_date";
    value[1]=TS_date;

    tag[2]="TS_time";
    value[2]=TS_time;

    tag[3]="Filling_Start_Time";
    value[3]=Filling_Start_Time;

    tag[4]="DCR_par.DCR_Nozzle_Number";
    value[4]=DCR_par.Active_Nozzle;

    tag[5]="saved_unit_price";
    value[5]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price.toHex();

    tag[6]="Sale_Error_code";
    value[6]=Sale_Error_code;

    tag[7]="FTYP";
    value[7]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP;

    tag[8]="CTYP";
    value[8]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].CTYP;

    tag[9]="ATYP";
    value[9]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP;

    tag[10]="STYP";
    value[10]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP;

    tag[20]="PATYP";
    value[20]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP;

    tag[11]="last_customer_plate";
    value[11]=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE.toHex();

    tag[12]="Nozzle_Up_Time";
    value[12]=Nozzle_Up_Time;

    tag[13]="Nozzle_Down_Time";
    value[13]=Nozzle_Down_Time;

    tag[14]="RFID_Read_Time";
    value[14]=RFID_Read_Time;

    tag[15]="RFID_Result_Time";
    value[15]=RFID_Result_Time;

    tag[16]="Authorize_Time";
    value[16]=Authorize_Time;

    tag[17]="Paid_Confrim_Time";
    value[17]=Paid_Confrim_Time;

    tag[18]="Paid_Fiscal_Time";
    value[18]=Paid_Fiscal_Time;

    tag[19]="Person_is_Ok";
    value[19]=Person_is_Ok;

    settings.setValue2(tag,value);

    //    settings.setValue("pump_sta",Pump_Filling_status);
    //    settings.setValue("TS_date",TS_date);
    //    settings.setValue("TS_time",TS_time);
    //    settings.setValue("Filling_Start_Time",Filling_Start_Time);
    //    settings.setValue("DCR_par.DCR_Nozzle_Number",DCR_par.Active_Nozzle);
    //    settings.setValue("saved_unit_price",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price.toHex());
    //    settings.setValue("Sale_Error_code",Sale_Error_code);
    //    settings.setValue("FTYP",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP);
    //    settings.setValue("CTYP",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].CTYP);
    //    settings.setValue("ATYP",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP);
    //    settings.setValue("STYP",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP);
    //    settings.setValue("PATYP",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP);
    //    settings.setValue("last_customer_plate",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE.toHex());
    //    settings.setValue("Nozzle_Up_Time",Nozzle_Up_Time);
    //    settings.setValue("Nozzle_Down_Time",Nozzle_Down_Time);
    //    settings.setValue("RFID_Read_Time",RFID_Read_Time);
    //    settings.setValue("RFID_Result_Time",RFID_Result_Time);
    //    settings.setValue("Authorize_Time",Authorize_Time);
    //    settings.setValue("Paid_Confrim_Time",Paid_Confrim_Time);
    //    settings.setValue("Paid_Fiscal_Time",Paid_Fiscal_Time);
    //    settings.setValue("Person_is_Ok",Person_is_Ok);
    sync();
    system("sync");
}

void ScpRoutine::save_filling_price()
{
    //QSettings settings(settingPath, QSettings::IniFormat);

    system("sync &");
}

void ScpRoutine::pumpStatus(){
    //qDebug()<<"DCR_par.DCR_status"<<DCR_par.DCR_status<<pump_sta<<pump_sta_2;
    switch (DCR_par.DCR_status) {
    case 0x00: // Pump not programmed

        break;

    case 0x01:  // Reset
        request_flg=2;
        break;

    case 0x02:  // Authorized
        if(Pump_Filling_status==0)
        {

            Pump_Filling_status=1;
            save_pump_status();
            pump_sta_2=true;
            //cihaz fazla açılıp kapanırsa ikinci kat koruma
            //qDebug()<<"pump_sta_2_1"<<pump_sta_2;
            settings.setValue("pump_sta_2",pump_sta_2);
        }
        Pump_Filling_status=1;

        Send_authorize_flg_2=false;
        authorize_count2=0;
        break;

    case 0x04:  // Filling
        if((Nozzle_up_flg==1))
        {
            clrc_up_flg=0;
            wait_km_flg=false;
            if(Pump_Filling_status==0)
            {
                Pump_Filling_status=1;
                save_pump_status();
                pump_sta_2=true; //cihaz fazla açılıp kapanırsa ikinci kat koruma
                //qDebug()<<"pump_sta_2_1"<<pump_sta_2;
                settings.setValue("pump_sta_2",pump_sta_2);
            }
            Pump_Filling_status=1;
            LCD_refresh_flg=1;
            online_flg=false;
            Allow_card_read=1;
            if((select_version==1) || (select_version==2))
            {
                SuspendReply=false;
                ResumeReply=true;
            }
            if(test) qDebug()<<">>ssssssssssssssssssssssSuspendReply";
        }
        Send_authorize_flg_2=false;
        authorize_count2=0;
        break;

    case 0x05:  // Filling Completed
        if((Pump_Filling_status==1) && (Nozzle_up_flg==0))
        {
            if(routinedebug)qDebug()<<"Filling Completed"<<Volumetotal_received<<Volumetotal_received;
            Volumetotal_received=false;
            Amounttotal_received=false;
            ask_last_screen=true;
            End_total_report=true;
            Pump_Filling_status=0;
            LCD_refresh_flg=1;
            CustomerID.clear();
            Temp_CustomerID.clear();
            for(int jj=0; jj<16;jj++)
            {
                TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                databuf[jj]=0;
            }

            Allow_card_read=0;
            online_flg=false;
            Send_authorize_flg_2=false;
        }

        break;

    case 0x06:  // Max Amount/Volume Reached
        break;

    case 0x07:  // Switched off
        break;

    case 0x0B:                                           // Sus
        SuspendReply=true;
        ResumeReply=false;
        if(test) qDebug()<<">>ssssssssssssssssssssssSuspendReply";
        break;
    case 0x0C:
        SuspendReply=false;
        ResumeReply=true;
        if(test) qDebug()<<">>ssssssssssssssssssssssSuspendReply";
        break;
    default:
        break;
    }

}

/////////////////////////////////////serial-1/////////////////////////////////////////////////////////////
void read_Print_data()
{
    //SendPrintData.clear();
    SCPArray[1]->readData_printer(ReadPrintData);
    if(ReadPrintData.length() > 0)
    {
        ReadPrintTempData=ReadPrintData;
        if(ReadPrintTempData.toHex().indexOf("023132")>=0)
        {
            printer_check_flg=true;

        }
        else printer_check_flg=false;

        if(ReadPrintTempData.toHex().indexOf("02313206")>=0)
        {
            printer_paper=true;
        }
        else if(ReadPrintTempData.toHex().indexOf("02313216")>=0)
        {
            printer_paper=false;
        }
    }

}

void send_printer_poll()

{
    SendPrintData.clear();
    SendPrintData[0]=0x02;
    SendPrintData[1]=0x30;
    SendPrintData[2]=0x31;
    SendPrintData[3]=0x53;
    SendPrintData[4]=0xFA;
    SCPArray[1]->writePrintData(SendPrintData);
    if(test) qDebug()<<"SendPrintData"<<SendPrintData;

}

void ScpRoutine::saveID()
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("TempID_0",Temp_ID[0]);
    settings.setValue("TempID_1",Temp_ID[1]);
    settings.setValue("TempID_2",Temp_ID[2]);
    settings.setValue("Person_is_Ok",Person_is_Ok);
    sync();
    system("sync");

    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==0)      //Pompacı
    {
        qDebug()<<"pumper";
        if(USC_is_exist==false){StoredScreenRFID_fromMaster=2;}
    }


    if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1) || (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==2)) //authorize de FTYP TTS  veya MTS olarak döndüyse
    {
        if(USC_is_exist==false /*&& customerCard==true*/)
        {
            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1)
            {
                qDebug()<<"burdayken";
                if(StoredScreenRFID_fromMaster!=2){
                StoredScreenRFID_fromMaster=4;
                } else {
                    StoredScreenRFID_fromMaster=2;
                }
            }
            else
            {
                if(StoredScreenRFID_fromMaster!=2 || pumper_flg==false)
                    StoredScreenRFID_fromMaster=3;
            }
        }
    }

    if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==9) || (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==19))
    {
        if(USC_is_exist==false){qDebug()<<"pumpers";StoredScreenRFID_fromMaster=2;}
    }

}



void ScpRoutine::ReadsavedID()
{
    //QSettings settings(SettingsFile, QSettings::IniFormat);
    Temp_ID[0]=settings.value("TempID_0").toString();
    Temp_ID[1]=settings.value("TempID_1").toString();
    Temp_ID[2]=settings.value("TempID_2").toString();

    if(test)qDebug()<<"ReadsavedID"<<Temp_ID[0]<<Temp_ID[1]<<Temp_ID[2]<<Person_is_Ok;
    if(test)qDebug()<<"saveID-ftyp"<<pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP<<Person_is_Ok<<ID_count<<pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP;
    settings.setlog("SATIS_DEBUG saveID-ftyp "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP)+"   "+QString::number(Person_is_Ok)+"   "+QString::number(ID_count));
    if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==0)      //Pompacı
    {
        Saved_Person_ID=Temp_ID[0];
        Saved_Customer_ID.clear();
        Saved_Driver_ID.clear();
        if(test)qDebug()<<"Card-Type-Person";
        settings.setlog("SATIS_DEBUG Card-Type-Person "+Temp_ID[0]);
        pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP=1;
        pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=0;

    }
    else if((pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==1) || (pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==2)) //authorize de FTYP TTS  veya MTS olarak döndüyse
    {

        if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==1) //TTS
        {
            pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP=2;
            if(pumper_flg){
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=21;
            }
            else{
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=1;
            }
        }
        else if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==2) //MTS
        {
            pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP=2;
            qDebug()<<pumper_flg<<"pomper";
            if(pumper_flg) pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=22;
            else pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=2;
        }

        if((pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP==1) || (pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP==0))
        {
            if(Person_is_Ok==true)
            {
                Saved_Person_ID=Temp_ID[0];
                Saved_Customer_ID=Temp_ID[1];
                Saved_Driver_ID.clear();
                Person_is_Ok=false;
                settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person-Person_is_Ok "+Temp_ID[0]+"  "+Temp_ID[1]);
                if(test) qDebug()<<"Card-Type-Customer-Person";

            }
            else
            {
                Saved_Customer_ID=Temp_ID[0];
                Saved_Driver_ID.clear();
                Saved_Person_ID.clear();
            }
        }
        else if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP==2)
        {
            if(Person_is_Ok==true)
            {
                Saved_Person_ID=Temp_ID[0];
                Saved_Customer_ID=Temp_ID[1];
                Saved_Driver_ID.clear();
                Person_is_Ok=false;
                settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person-Person_is_Ok "+Temp_ID[0]+"  "+Temp_ID[1]);
                if(test) qDebug()<<"Card-Type-Customer-Person";

            }
            else
            {
                Saved_Customer_ID=Temp_ID[0];
                Saved_Person_ID=Temp_ID[1];
                Saved_Driver_ID.clear();
            }
        }
        else if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP==3)
        {
            if(Person_is_Ok==true)
            {
                Saved_Person_ID=Temp_ID[0];
                Saved_Customer_ID=Temp_ID[1];
                Saved_Driver_ID.clear();
                Person_is_Ok=false;
                settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person-Person_is_Ok  "+Temp_ID[0]+"  "+Temp_ID[1]);
                if(test) qDebug()<<"Card-Type-Customer-Person";

            }
            else
            {
                if(auttype_3_4==true)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Driver_ID=Temp_ID[1];
                    Saved_Person_ID=Temp_ID[2];
                }
                else
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Person_ID=Temp_ID[1];
                    Saved_Driver_ID.clear();
                }
            }
        }
        else if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP==4)
        {            if(Person_is_Ok==true)
            {
                Saved_Person_ID=Temp_ID[0];
                Saved_Customer_ID=Temp_ID[1];
                Saved_Driver_ID.clear();
                Person_is_Ok=false;
                settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person-Person_is_Ok  "+Temp_ID[0]+"  "+Temp_ID[1]);
                if(test) qDebug()<<"Card-Type-Customer-Person";

            }
            else
            {
                Saved_Customer_ID=Temp_ID[0];
                Saved_Driver_ID=Temp_ID[1];
                Saved_Person_ID.clear();
            }
        }
    }
    else if((pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==9) || (pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==19))
    {
        if(Person_is_Ok==true)
        {
            Saved_Person_ID=Temp_ID[0];
            Saved_Customer_ID=Temp_ID[1];
            Saved_Driver_ID.clear();
            Person_is_Ok=false;
            settings.setlog("SATIS_DEBUG  Card-Type-loyalty_is_Ok-Customer-Person "+Temp_ID[0]+"  "+Temp_ID[1]);
            if(test) qDebug()<<"Card-Type-Customer-loyalty-Person"<<Temp_ID[0]<<Temp_ID[1];
        }
        else
        {
            Saved_Customer_ID=Temp_ID[0];
            Saved_Driver_ID.clear();
            Saved_Person_ID.clear();
            settings.setlog("SATIS_DEBUG  Card-Type-loyalty_is_Ok-Customer  "+Temp_ID[0]);
            if(test) qDebug()<<"Card-Type-Customer-loyalty";

        }

        pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP=1;
        pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=9;

    }

    if(Wrong_trans_for_save==true) //usc-yokken hatalı satışı kaydetmek için
    {
        Wrong_trans_for_save=false;
        if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP==1)
        {
            pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP+20;
        }
    }
    save_pump_status();
    Temp_ID[0].clear();
    Temp_ID[1].clear();
    Temp_ID[2].clear();
    ID_count=0;

}

void ScpRoutine::saveID_old()
{
    if(test)qDebug()<<"saveID-ftyp"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP<<Person_is_Ok<<ID_count<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP;
    settings.setlog("SATIS_DEBUG saveID-ftyp "+QString::number(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP)+"  "+QString::number(Person_is_Ok)+"  "+QString::number(ID_count));
    if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==0)      //Pompacı
    {
        Saved_Person_ID=Temp_ID[0];
        if(test)qDebug()<<"Card-Type-Person";
        settings.setlog("SATIS_DEBUG Card-Type-Person "+Temp_ID[0]);
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PATYP=1;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP=0;
        if(USC_is_exist==false){qDebug()<<"pumperf";StoredScreenRFID_fromMaster=2;}
    }
    else if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1) || (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==2)) //authorize de FTYP TTS  veya MTS olarak döndüyse
    {
        if(USC_is_exist==false)
        {
            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1)
            {
                qDebug()<<"burdayken2";
                StoredScreenRFID_fromMaster=4;
            }
            else
            {
                if(StoredScreenRFID_fromMaster!=2 || pumper_flg==false)
                    StoredScreenRFID_fromMaster=3;
            }
        }
        if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==1) //TTS
        {
            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PATYP=2;
            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP=1;
        }
        else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==2) //MTS
        {
            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PATYP=2;
            pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP=2;
        }

        if(ID_count==3)  //CUSTOMER//Driver//PERSON
        {
            if(USC_is_exist==false)
            {
                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==1)
                {
                    Saved_Customer_ID=Temp_ID[0];
                }
                else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==2)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Driver_ID=Temp_ID[1];
                }
                else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==3)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Driver_ID=Temp_ID[1];
                    Saved_Person_ID=Temp_ID[2];
                }
                else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==4)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Person_ID=Temp_ID[1];
                }

            }
            else
            {
                Saved_Customer_ID=Temp_ID[0];
                Saved_Driver_ID=Temp_ID[1];
                Saved_Person_ID=Temp_ID[2];
            }

            if(test)qDebug()<<"Card-Type-Customer-Driver-Person";
            settings.setlog("SATIS_DEBUG  Card-Type-Customer-Driver-Person  "+Temp_ID[0]+"  "+Temp_ID[1]+"  "+Temp_ID[2]);
        }
        else if(ID_count==2) //CUSTOMER//PERSON
        {
            if(USC_is_exist==false)
            {

                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==3)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Person_ID=Temp_ID[1];
                    if(test)qDebug()<<"Card-Type-Customer+Person";
                    settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person "+Temp_ID[0]+"  "+Temp_ID[1]);
                }
                else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==4)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Driver_ID=Temp_ID[1];
                    //qDebug()<<"Card-Type-Customer+Driver";
                    settings.setlog("SATIS_DEBUG  Card-Type-Customer+Driver "+Temp_ID[0]+"  "+Temp_ID[1]);
                }
                else
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Person_ID=Temp_ID[1];
                    if(test)qDebug()<<"Card-Type-Customer+Person";
                    settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person "+Temp_ID[0]+"  "+Temp_ID[1]);
                }
            }
            else  //usc varsa
            {
                if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==2)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Person_ID=Temp_ID[1];
                    if(test)qDebug()<<"Card-Type-Customer+Person";
                    settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person "+Temp_ID[0]+"  "+Temp_ID[1]);
                }
                else if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].ATYP==4)
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Driver_ID=Temp_ID[1];
                    if(test)qDebug()<<"Card-Type-Customer+Driver";
                    settings.setlog("SATIS_DEBUG  Card-Type-Customer+Driver "+Temp_ID[0]+"  "+Temp_ID[1]);
                }
                else
                {
                    Saved_Customer_ID=Temp_ID[0];
                    Saved_Driver_ID.clear();
                    Saved_Person_ID.clear();
                }

            }
        }
        else if(ID_count==1) //CUSTOMER -->>TTS or MTS
        {

            if(Person_is_Ok==true)
            {
                Saved_Person_ID=Temp_ID[0];
                Saved_Customer_ID=Temp_ID[1];
                Person_is_Ok=false;
                settings.setlog("SATIS_DEBUG  Card-Type-Customer+Person-Person_is_Ok "+Temp_ID[0]+"  "+Temp_ID[1]);
                if(test) qDebug()<<"Card-Type-Customer-Person";

            }
            else
            {
                Saved_Customer_ID=Temp_ID[0];
                settings.setlog("SATIS_DEBUG  Card-Type-Person_is_Ok-Customer "+Temp_ID[0]+"  "+Temp_ID[1]);
                if(test) qDebug()<<"Card-Type-Customer-";

            }
            if(test) qDebug()<<"Card-Type-Customer-";
        }
    }
    else if((pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==9) || (pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].FTYP==19))
    {
        if(USC_is_exist==false){qDebug()<<"pumperb";StoredScreenRFID_fromMaster=2;}
        if(Person_is_Ok==true)
        {
            Saved_Person_ID=Temp_ID[0];
            Saved_Customer_ID=Temp_ID[1];
            Person_is_Ok=false;
            settings.setlog("SATIS_DEBUG  Card-Type-loyalty_is_Ok-Customer-Person "+Temp_ID[0]+"   "+Temp_ID[1]);
            if(test) qDebug()<<"Card-Type-Customer-loyalty-Person"<<Temp_ID[0]<<Temp_ID[1];
        }
        else
        {
            Saved_Customer_ID=Temp_ID[0];
            settings.setlog("SATIS_DEBUG  Card-Type-loyalty_is_Ok-Customer "+Temp_ID[0]);
            if(test) qDebug()<<"Card-Type-Customer-loyalty";

        }

        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PATYP=1;
        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].STYP=9;

    }
    Temp_ID[0].clear();
    Temp_ID[1].clear();
    Temp_ID[2].clear();
    ID_count=0;
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("Saved_Customer_ID",Saved_Customer_ID);
    settings.setValue("Saved_Driver_ID",Saved_Driver_ID);
    settings.setValue("Saved_Person_ID",Saved_Person_ID);
    sync();
    system("sync");
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///////////////////////// printinggggggggggggggg/////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void ScpRoutine::routine()
{

    quint8 nn;
    //QSettings settings(SettingsFile, QSettings::IniFormat);
    if(test_modu)
    {
        read_Print_data();send_printer_poll();
    }
    //filling Completed
    if(routinedebug){if(Filling_comp_flg==1) qDebug()<<"printingggg"<< ask_last_screen<<End_total_report<<Paid_Confirm<<Volumetotal_received<<Amounttotal_received;}
    if((Filling_comp_flg==1) && (ask_last_screen==false) && (Paid_Confirm==true)&&((Amounttotal_received==true)&&(Volumetotal_received==true)))
    {
        convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_amount,4,amount_dot_index);
        QByteArray amount_screen=printer_temp_buff;
        convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].filling_volume,4,volume_dot_index);
        QByteArray volume_screen=printer_temp_buff;
        qDebug()<<"kayıt";
        amount_screen=amount_screen.replace(".",",");
        volume_screen=volume_screen.replace(".",",");
        settings.setVolumeAmount(amount_screen,volume_screen,0,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE,pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Product_name);

        saving_the_sales=true;//kayıt yapılırken totol sorup kayıt etmeye engel
        Paid_Confirm=false;
        Filling_comp_flg=0;
        NewPlateExist=0;
        LCD_status=1;
        LCD_refresh_flg=1;
        send_printer_sale_data();
        if(bater_low_flg==true)
        {
            fisdate="01.01.01";
            fisclock="01:01";
        }

        SalesID.clear();

        SalesID.append(QString::number(ABU_address).rightJustified(2,'0'));

        SalesID.append(QString::number(TS_no).rightJustified(6,'0'));


        ReadsavedID();
        Person_is_Ok=false;
        if(test)qDebug()<<"Saved_Customer_ID"<<Saved_Customer_ID;
        if(test)qDebug()<<"Saved_Driver_ID"<<Saved_Driver_ID;
        if(test)qDebug()<<"Saved_Person_ID"<<Saved_Person_ID;

        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-Customer_ID %s",Saved_Customer_ID.toLocal8Bit().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-Driver_ID %s",Saved_Driver_ID.toLocal8Bit().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-Person_ID %s",Saved_Person_ID.toLocal8Bit().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-fisno %d",fisno);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-Nozzle %d",DCR_par.Active_Nozzle);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-SalesID %s",SalesID.toLocal8Bit().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-filling_price %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-filling_volume %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-filling_amount %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_amount.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-BT_Amount %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Amount.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-T_Amount %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Amount.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-BT_Volume %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Volume.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-T_Volume %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Volume.toHex().constData());
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-FTYP %d",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-CTYP %d",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].CTYP);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-ATYP %d",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-STYP %d",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-PATYP %d",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP);
        //        syslog(LOG_DEBUG,"SATIS_DEBUG  SAVED-PLATE %s",pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE.toHex().constData());


        settings.setlog("SATIS_DEBUG  SAVED-Customer_ID : "+Saved_Customer_ID);
        settings.setlog("SATIS_DEBUG  Saved_Person_ID : "+Saved_Person_ID);
        settings.setlog("SATIS_DEBUG  Saved-fisno: "+QString::number(fisno));
        settings.setlog("SATIS_DEBUG  Saved-Nozzle: "+QString::number(DCR_par.Active_Nozzle));
        settings.setlog("SATIS_DEBUG  Saved-Nozzle_id: "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].nozzle_id));
        settings.setlog("SATIS_DEBUG  Saved-SalesID: "+SalesID);
        settings.setlog("SATIS_DEBUG  Saved-filling_price: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-filling_volume: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-filling_amount: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_amount.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-BT_Amount: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Amount.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-T_Amount: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Amount.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-BT_Volume: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Volume.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-T_Volume: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Volume.toHex()));
        settings.setlog("SATIS_DEBUG  Saved-FTYP: "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP));
        settings.setlog("SATIS_DEBUG  Saved-CTYP: "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].CTYP));
        settings.setlog("SATIS_DEBUG  Saved-ATYP: "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP));
        settings.setlog("SATIS_DEBUG  Saved-STYP: "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP));
        settings.setlog("SATIS_DEBUG  Saved-PATYP: "+QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP));
        settings.setlog("SATIS_DEBUG  Saved-PLATE: "+QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE.toHex()));
        if(Payment_Error_code==1)
        {
            settings.setlog("SATIS_DEBUG  SAVED-USC ");
        }
        else
        {
            settings.setlog("SATIS_DEBUG  SAVED-UBK ");
        }
        if(test)qDebug()<<"TOTAL-VOLUME-NOZZZLE:"+QString::number(DCR_par.Active_Nozzle)<<pump[pumpNo].nozzle[0].BT_Volume.toHex()<<pump[pumpNo].nozzle[1].BT_Volume.toHex()<<pump[pumpNo].nozzle[2].BT_Volume.toHex()<<pump[pumpNo].nozzle[3].BT_Volume.toHex()<<pump[pumpNo].nozzle[4].BT_Volume.toHex();
        //InsertSales(int id,int TSNO, int RNO, QString date, QString clock, int PMPN, QString nozzle, QString unitprice, QString liter, QString amount, QString type, QString rfid, QString plate, QString vehiclekm, QString status)

        if(DCR_par.Active_Nozzle==0){DCR_par.Active_Nozzle=DCR_par.DCR_Nozzle_Number;}
        if((pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PrdctNo==0) || (pump[pumpNo].nozzle[DCR_par.Active_Nozzle].TankNo==0))
        {
            database->NozzleValues();
            //qDebug()<<"Nozzlevalues-again";
        }
        int counttable=database->CountId("SALES");
        if(counttable>=50)
        {
            database->delete_first_sales();
        }
        //        QJsonObject sales_object=database->kontrolSalesStatus();
        //        if(sales_object.value("SALES").toArray().size()<=0)
        //        {
        //            if((counttable>=number_of_salessave))
        //            {
        //                if(table_name=="SALES")
        //                {
        //                    table_name="SALES2";
        //                    database->delete_table(table_name);
        //                    sync();
        //                    system("sync");
        //                }
        //                else
        //                {
        //                    table_name="SALES";
        //                    database->delete_table(table_name);
        //                    sync();
        //                    system("sync");
        //                }
        //                saveTableName();

        //            }
        //        }

        Change_status_to_IDLE=true;

        long endtotal=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Volume.toHex().toLong();
        long begintotal=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Volume.toHex().toLong();
        long fillingfortotal=pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex().toLong();

        if(routinedebug)
        {
            qDebug()<<"endtotal-kayıt: "<<QString::number(endtotal);
            qDebug()<<"begintotal-kayıt: "<<QString::number(begintotal);
            qDebug()<<"fillingfortotal-kayıt: "<<QString::number(fillingfortotal);
            qDebug()<<"fark1: "<<QString::number((endtotal-begintotal)-fillingfortotal);
        }

        if(endtotal>=begintotal)
        {
            if(((endtotal-begintotal)-fillingfortotal<=50) && ((endtotal-begintotal)-fillingfortotal>=-50))
            {
                Payment_Error_code=0;
                if(routinedebug)qDebug()<<"satış-doğru-kayıt: ";
            }
            else
            {
                Payment_Error_code=6;
                if(routinedebug)qDebug()<<"satış-yanlış-kayıt: ";
            }

        }
        else
        {
            if((((100000000-begintotal) + endtotal-fillingfortotal)<=50) && (((100000000-begintotal) + endtotal-fillingfortotal)>=-50))
            {
                Payment_Error_code=0;
                if(routinedebug)qDebug()<<"satış-doğru-kayıt: ";
            }
            else
            {
                Payment_Error_code=6;
                if(routinedebug)qDebug()<<"satış-yanlış-kayıt: ";
            }

        }
        ////TS_date,\
        /// //TS_time,\
        if(test)qDebug()<<"Payment_Error_code: "<<QString::number(Payment_Error_code);
        //qDebug()<<"Sales : "<<QString::number(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP);
        if(plateDB==false){
        balanceLimit=database->getBalanceLimit(Saved_Customer_ID);//chn
        } else {
            balanceLimit=database->getBalanceLimit(customerPlate);//chn
        }
        qDebug()<<QString::number(volume_dot_index) <<"nokta ayar"; //chn
        int pointSetting; //chn
        if(QString::number(volume_dot_index)=="3"){pointSetting=5;} //chn
        else if(QString::number(volume_dot_index)=="2"){pointSetting=6;}//chn
        else if(QString::number(volume_dot_index)=="1"){pointSetting=7;}//chn
        qDebug()<<"UPDATE BALANCE LIMIT: "<<customerPlate<<Saved_Customer_ID<<"BalanceLimit"<<balanceLimit<<"filled volume"<<pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex();
        qDebug()<<QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex()).insert(pointSetting,".")<<QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex()).insert(pointSetting,".").toDouble(&ok)<<balanceLimit.replace(",",".").toDouble(&ok)-QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex()).insert(pointSetting,".").toDouble(&ok);
//        if(pumper_flg){
//            balanceLimit=database->getBalanceLimit(Saved_Customer_ID);
//            qDebug()<<"BAANCEEEEEE"<<balanceLimit;
//        }
        balanceLimit=QString::number(balanceLimit.replace(",",".").toDouble(&ok)-QString::fromUtf8(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume.toHex()).insert(pointSetting,".").toDouble(&ok),'f',2);
        qDebug()<<balanceLimit<<"balance limit debug"<<Saved_Customer_ID;
        if(plateDB==false){
        database->updateBalanceLimit(Saved_Customer_ID,balanceLimit);
        } else {
            database->updateBalanceLimit(customerPlate,balanceLimit);
            plateDB=false;
        }
        //fisclock=QDateTime::currentDateTime().toString("HH:mm:ss");
        database->InsertSales(\
                    SalesID,\
                    TS_no,\
                    fisno,\
                    fisdate.remove("."),\
                    fisclock.remove(":"),\
                    pumpNo,\
                    pump[pumpNo].nozzle[DCR_par.Active_Nozzle].nozzle_id,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PrdctNo,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].TankNo,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_amount,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Amount,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Amount,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].BT_Volume,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].T_Volume,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].FTYP,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].CTYP,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].ATYP,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].STYP,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PATYP,\
                Saved_Customer_ID,\
                Saved_Driver_ID,\
                Saved_Person_ID,\
                pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE,\
                pump[pumpNo].KM,\
                pump[pumpNo].runningTime,
                Nozzle_Up_Time,\
                Nozzle_Down_Time,\
                RFID_Read_Time,\
                RFID_Result_Time,\
                Authorize_Time,\
                Filling_Start_Time,\
                Paid_Confrim_Time,\
                Paid_Fiscal_Time,\
                Payment_Error_code\
                );
        //qDebug()<<"save-saless"<<DCR_par.Active_Nozzle;
        save_pump_status();

        pump_sta_2=false; //cihaz fazla açılıp kapanırsa ikinci kat koruma
        //qDebug()<<"pump_sta_2"<<pump_sta_2;
        settings.setValue("pump_sta_2",pump_sta_2);
        //qDebug()<<"pump_sta_2-kayıt"<<pump_sta_2;
        sync();
        system("sync");
        //qDebug()<<"sync"<<pump_sta_2;
        saveFilling(1,DCR_par.Active_Nozzle);
        saving_the_sales=false;
        Payment_Error_code=0;
        Tag_read_count_flg=0;
        Tag_read_flg=0;
        qDebug()<<"cl5";
        vehiclekmtr.clear();
        Saved_Customer_ID.clear();
        Saved_Driver_ID.clear();
        Saved_Person_ID.clear();
        ID_count=0;
        USC_com_lock=false;
        for(nn=0; nn<=Number_of_Nozzle; nn++)
        {
            pump[pumpNo].nozzle[0].Cyrpto_key_from_master.clear();
        }
        save_to_sdcard();
        protect_flg=false;
        End_total_report=false;
        Amounttotal_received=false;
        Volumetotal_received=false;
       reboot_ok=false;
        if(routinedebug)qDebug()<<"satıs-kaydedildi";
        if(immediatelyUpdateBalanceLimit==1){
            immediatelyUpdateBalanceLimit=2;
        }

    }
    if(print_test_mode==true)
    {
        print_test_mode=false;
        send_print_test();
    }
    if(givecopyflg==true)
    {
        givecopyflg=false;
        send_printer_sale_data();
    }


}

void ScpRoutine::saveTableName()
{
    //QSettings settings(settingPath, QSettings::IniFormat);
    settings.setValue("table_name",table_name );
    system("sync");
}

void ScpRoutine::send_print_test()
{
    QString space="         ";
    SendPrintData.clear();
    SendPrintData.append(0x02);
    SendPrintData.append(0x30);
    SendPrintData.append(0x31);
    SendPrintData.append(0x50);


    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    //date -Time
    fisdate=QDate::currentDate().toString("dd.MM.yyyy"); // gün-ay-yıl şeklinde tarih değişkeni
    SendPrintData.append(fisdate.toLatin1());
    SendPrintData.append("  ");
    SendPrintData.append("SAAT:");
    fisclock=QDateTime::currentDateTime().toString("HH:mm:ss");
    dataTime = fisclock.toLatin1();
    int pos= dataTime.lastIndexOf(QChar(':'));
    SendPrintData.append(dataTime.left(pos));
    SendPrintData.append(0x0A);
    // 1- line Space

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    SendPrintData.append("         TEST         ");
    SendPrintData.append(0x0B);

    // 1- line Space

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    SendPrintData.append("DEVICE SERIAL: "+getMacAddress().remove(":").mid(0,6).toLatin1());
    SendPrintData.append(0x0A);

    // 1- line Space

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    if(clrc_check_flg)
    {
        SendPrintData.append("    CARD READER OK    ");
        SendPrintData.append(0x0B);
    }
    else
    {
        SendPrintData.append("  CARD READER ERROR   ");
        SendPrintData.append(0x0B);
    }
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    if(rfid_check_flg)
    {
        SendPrintData.append("   ACCESS POINT OK    ");
        SendPrintData.append(0x0B);
    }
    else
    {
        SendPrintData.append("  ACCESS POINT ERROR  ");
        SendPrintData.append(0x0B);
    }
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    if(printer_check_flg)
    {
        SendPrintData.append("      PRINTER OK      ");
        SendPrintData.append(0x0B);
    }
    else
    {
        SendPrintData.append("     PRINTER ERROR    ");
        SendPrintData.append(0x0B);
    }
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(0xFA);

    SCPArray[1]->writePrintData(SendPrintData);
}

void add_space(unsigned char space_len)
{
    unsigned char i;
    for(i=0;i<=space_len;i++)
    {
        space_buffer[i]=0x20;
    }
    space_len=0;
}
void calculate_KDV(unsigned int  &kdvint,unsigned int  &data,unsigned char dot_ind)//
{
    char i,temp;
    kdv_temp_buff.clear();
    if(data%(100+kdvint)>50)
    {
        data=data+100;
        data=data/(100+kdvint);
    }
    else
    {
        data=data/(100+kdvint);;
    }
    kdv_temp_buff=QString::number(data).toLatin1();


    if(kdv_temp_buff.length()<=dot_ind)
    {   temp=dot_ind-kdv_temp_buff.length()+1;

        for(i=0;i<temp;i++)
        {
            kdv_temp_buff.prepend(0x30);

        }

    }
    kdv_temp_buff.insert((kdv_temp_buff.length()-dot_ind),QByteArray(","));


    if(kdv_temp_buff.length()>dot_ind+4)
    {
        unsigned int temp_len=kdv_temp_buff.length();
        for(i=0;i<(temp_len-dot_ind-2)/3;i++)
        {
            kdv_temp_buff.insert(temp_len-((dot_ind+4)+(3*i)),QByteArray("."));
        }
    }

}
void ScpRoutine::convert_hex_to_ascii(QByteArray &data, unsigned char len,unsigned char dot_ind)//
{ unsigned char i,nonzero_ind,temp;
    QByteArray asc;
    ascii.clear();
    printer_temp_buff.clear();
    printer_temp_buff_for_database.clear();
    nonzero_ind=0;
    asc.clear();

    asc=data.toHex();

    for(i=0;i<2*len;i++)
    {
        if(asc[i]!=0x30)
        {
            nonzero_ind=i;
            break;
        }
    }

    printer_temp_buff=asc.mid(nonzero_ind);

    if(dot_ind!=0)
    {
        if(printer_temp_buff.length()<=dot_ind)
        {   temp=dot_ind-printer_temp_buff.length()+1;
            for(i=0;i<temp;i++)
            {
                printer_temp_buff.prepend(0x30);


            }


        }
        printer_temp_buff_for_database=printer_temp_buff;
        printer_temp_buff_for_database.insert((printer_temp_buff_for_database.length()-dot_ind),QByteArray("."));
        printer_temp_buff.insert((printer_temp_buff.length()-dot_ind),QByteArray(","));
        //printer_temp_buff_for_setting.insert((printer_temp_buff_for_setting.length()-dot_ind),QByteArray("."));
    }

    if(dot_ind==3)if(printer_temp_buff_for_database=="00000.000"){printer_temp_buff_for_database="0.000";}
    if(dot_ind==2)if(printer_temp_buff_for_database=="000000.00"){printer_temp_buff_for_database="0.00";}
    if(printer_temp_buff.length()>dot_ind+4)
    {
        unsigned int temp_len=printer_temp_buff.length();
        for(i=0;i<(temp_len-dot_ind-2)/3;i++)
        {
            printer_temp_buff.insert(temp_len-((dot_ind+4)+(3*i)),QByteArray("."));
        }
    }
    if(dot_ind==3)if(printer_temp_buff.toHex()=="30302e3030302c303030"){printer_temp_buff="0,000";}
    if(dot_ind==2)if(printer_temp_buff.toHex()=="3030302e3030302c3030"){printer_temp_buff="0,00";}
}

void ScpRoutine::send_printer_sale_data()
{

    //read_time();


    if(ZetNO==0)
    {
        ZetNO=1;

    }

    if(vardiyano==0)
    {
        vardiyano=1;

    }

    QString space="         ";
    SendPrintData.clear();
    SendPrintData.append(0x02);
    SendPrintData.append(0x30);
    SendPrintData.append(0x31);
    SendPrintData.append(0x50);


    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);


    //header_1 --> MEPSAN
    if(printing_header_data[0].length()>0){

        if(printing_header_data[0].length()<20)
        {
            add_space(((22-printing_header_data[0].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[0].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[0].toLatin1());
        SendPrintData.append(0x0B);
    }
    else
    {
        SendPrintData.append(space.toLatin1());
        SendPrintData.append(0x0A);
        SendPrintData.append(space.toLatin1());
        SendPrintData.append(0x0A);

    }
    // Header_2 -- > Elektronik
    if(printing_header_data[1].length()>0){

        if(printing_header_data[1].length()<20)
        {
            add_space(((22-printing_header_data[1].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[1].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[1].toLatin1());
        SendPrintData.append(0x0A);
    }

    // Header_3 -- > Elektronik
    if(printing_header_data[2].length()>0){

        if(printing_header_data[2].length()<20)
        {
            add_space(((22-printing_header_data[2].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[2].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[2].toLatin1());
        SendPrintData.append(0x0A);
    }
    // Header_4 -- > Elektronik
    if(printing_header_data[3].length()>0){
        if(printing_header_data[3].length()<20)
        {
            add_space(((22-printing_header_data[3].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[3].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[3].toLatin1());
        SendPrintData.append(0x0A);
    }
    // 1- line Space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    //date -Time
    fisdate=QDate::currentDate().toString("dd.MM.yy"); // gün-ay-yıl şeklinde tarih değişkeni
    SendPrintData.append(fisdate.toLatin1());
    QStringList list=fisdate.split(".");
    fisdate.clear();
    fisdate.append(list.at(2));
    fisdate.append(list.at(1));
    fisdate.append(list.at(0));
    SendPrintData.append("   ");
    SendPrintData.append("SAAT: ");
    fisclock=QDateTime::currentDateTime().toString("HH:mm:ss");
    dataTime = fisclock.toLatin1();
    int pos= dataTime.lastIndexOf(QChar(':'));
    SendPrintData.append(dataTime.left(pos));
    SendPrintData.append(0x0A);


    //FIS NO
    dec_to_bcd(fisno);
    pump[pumpNo].Fiscal_R_Number[1]=bcd[1];
    pump[pumpNo].Fiscal_R_Number[0]=bcd[0];
    SendPrintData.append("FIS NO:");
    SendPrintData.append(QString::number(fisno).toLatin1());
    SendPrintData.append(0x0A);

    // 1- line Space

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    ////////////////////PLATE AND FİLLİNG TYPE
    //satış tipine göre değişiklik gösterir
    //#define Cash             0x00
    //#define TTS              0x01
    //#define MTS              0x02
    //#define Barrel           0x03
    //#define Transfer         0x04
    //#define Test             0x05
    //#define Manual           0x06
    //#define Undefined        0x07
    //#define Credit_Card      0x08
    //#define Loyalty          0x09
    //#define SaleName         0x0A



    //Plate Number

    add_space(((22-pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE.length())/2)-1);
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE);
    SendPrintData.append(0x0B);
    Customer_plate.clear();

    // 1- line Space

    //SendPrintData.append(space.toLatin1());
   // SendPrintData.append(0x0A);

    if(ask_km==true)
    {
        //-----------------------
        SendPrintData.append("----------------------");
        SendPrintData.append(0x0A);
        SendPrintData.append("KM:");
        add_space((17-vehiclekmtr.length()));
        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append("*");
        SendPrintData.append(vehiclekmtr.toLatin1());
        SendPrintData.append(0x0A);
    }


    if(ask_km_after==true)
    {
        //-----------------------
        SendPrintData.append("----------------------");
        SendPrintData.append(0x0A);
        SendPrintData.append("KM:");
        add_space((17-vehiclekmtr.length()));
        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append("*");
        SendPrintData.append(vehiclekmtr.toLatin1());
        SendPrintData.append(0x0A);
    }
    //-----------------------
     SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.length()==0)
    {
        pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name="TANIMSIZ";

    }
    convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume,4,volume_dot_index);
    Volume_for_Database=printer_temp_buff_for_database;
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append("LT X ");

    convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price,3,unit_price_dot_index);
    Unitprice_for_Database=printer_temp_buff_for_database;
    //add_space((20-pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.length()-(printer_temp_buff.length())));
    //SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append(printer_temp_buff);

    SendPrintData.append(0x0A);
    convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_amount,4,amount_dot_index);
    Amount_for_Database=printer_temp_buff_for_database;
    qDebug()<<"offf"<<printer_temp_buff.length();
    SendPrintData.append(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.toLatin1());
    add_space((19-(printer_temp_buff.length())-(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.length()))-1);
    SendPrintData.append(space_buffer);space_buffer.clear();

    SendPrintData.append("*");
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append("TL");
    SendPrintData.append(0x0A);

    //toplam

    SendPrintData.append("TOP");
    qDebug()<<"offf3"<<printer_temp_buff.length();
    add_space((15-printer_temp_buff.length()));
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append("*");
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append("TL");
    SendPrintData.append(0x0B);

    SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    Payload_type=0;
    switch (Payload_type)  //FOR DATABASE
    {
    case 0:
        type="NAKIT";
        break;
    case 1:
        type="TTS";
        break;
    case 2:
        type="MTS";
        break;
    case 3:
        type="NAKIT";
        break;
    case 4:
        type="TRANSFER";
        break;
    case 5:
        type="TEST";
        break;
    case 6:
        type="MANUAL";
        break;
    case 7:
        type="TANIMSIZ";
        break;
    case 8:
        type="KKARTI";
        break;
    case 9:
        type="LOYALTY";
        break;
    case 10:
        type="ISMESATIS";
        break;
    default:
        break;
    }

    //TYPE
    SendPrintData.append(type.toLatin1());
    add_space((19-printer_temp_buff.length()-type.length())-1);
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append("*");
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append("TL");
    SendPrintData.append(0x0B);
    //-----------------------
    SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    //POMPA NO:  && TAB. NO
    SendPrintData.append("POMPA NO:");

    SendPrintData.append(QString::number(cash_register_pump_no).toLatin1());
    add_space((5-QString::number(cash_register_pump_no).toLatin1().length()-
               QString::number(DCR_par.Active_Nozzle).toLatin1().length())-1);
    SendPrintData.append(space_buffer); space_buffer.clear();
    SendPrintData.append("TAB. NO:");
    SendPrintData.append(QString::number(DCR_par.Active_Nozzle).toLatin1());
    SendPrintData.append(0x0A);

    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);




    //end_info-1

    if((End_info[0].length()>0) && (End_info[0].length()<22))
    {

        if(End_info[0].length()<20)
        {
            add_space(((22-End_info[0].length())/2)-1);
        }
        else
        {
            add_space((22-End_info[0].length()-1));
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(End_info[0].toLatin1());
        SendPrintData.append(0x0A);
    }


    //end_info-2
    if((End_info[1].length()>0)&&(End_info[1].length()<22))
    {

        if(End_info[1].length()<20)
        {
            add_space(((22-End_info[1].length())/2)-1);
        }
        else
        {
            add_space((22-End_info[1].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(End_info[1].toLatin1());
        SendPrintData.append(0x0A);

    }

    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    SendPrintData.append("   MALI ISLEM DISI");
    SendPrintData.append(0x0B);
    SendPrintData.append("  SATIS FISI KOPYASI");
    SendPrintData.append(0x0B);

    //2-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);


    SendPrintData.append(0xFA);
    SendPrintDataCOPY=SendPrintData;
    if(test)qDebug()<<SendPrintData;
    SCPArray[1]->writePrintData(SendPrintData);



    /*
    //read_time();


    if(ZetNO==0)
    {
        ZetNO=1;

    }

    if(vardiyano==0)
    {
        vardiyano=1;

    }

    QString space="         ";
    SendPrintData.clear();
    SendPrintData.append(0x02);
    SendPrintData.append(0x30);
    SendPrintData.append(0x31);
    SendPrintData.append(0x50);


    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);


    //header_1 --> MEPSAN
    if(printing_header_data[0].length()>0){

        if(printing_header_data[0].length()<20)
        {
            add_space(((22-printing_header_data[0].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[0].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[0].toLatin1());
        SendPrintData.append(0x0B);
    }
    else
    {
        SendPrintData.append(space.toLatin1());
        SendPrintData.append(0x0A);
        SendPrintData.append(space.toLatin1());
        SendPrintData.append(0x0A);

    }
    // Header_2 -- > Elektronik
    if(printing_header_data[1].length()>0){

        if(printing_header_data[1].length()<20)
        {
            add_space(((22-printing_header_data[1].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[1].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[1].toLatin1());
        SendPrintData.append(0x0A);
    }

    // Header_3 -- > Elektronik
    if(printing_header_data[2].length()>0){

        if(printing_header_data[2].length()<20)
        {
            add_space(((22-printing_header_data[2].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[2].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[2].toLatin1());
        SendPrintData.append(0x0A);
    }
    // Header_4 -- > Elektronik
    if(printing_header_data[3].length()>0){
        if(printing_header_data[3].length()<20)
        {
            add_space(((22-printing_header_data[3].length())/2)-1);
        }
        else
        {
            add_space((22-printing_header_data[3].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(printing_header_data[3].toLatin1());
        SendPrintData.append(0x0A);
    }
    // 1- line Space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    //date -Time
    fisdate=QDate::currentDate().toString("dd.MM.yy"); // gün-ay-yıl şeklinde tarih değişkeni
    SendPrintData.append(fisdate.toLatin1());
    QStringList list=fisdate.split(".");
    fisdate.clear();
    fisdate.append(list.at(2));
    fisdate.append(list.at(1));
    fisdate.append(list.at(0));
    SendPrintData.append("  ");
    SendPrintData.append("SAAT:");
    fisclock=QDateTime::currentDateTime().toString("HH:mm:ss");
    SendPrintData.append(fisclock.toLatin1());
    SendPrintData.append(0x0A);


    //FIS NO
    dec_to_bcd(fisno);
    pump[pumpNo].Fiscal_R_Number[1]=bcd[1];
    pump[pumpNo].Fiscal_R_Number[0]=bcd[0];
    SendPrintData.append("FIS NO:");
    SendPrintData.append(QString::number(fisno).toLatin1());
    SendPrintData.append(0x0A);

    // 1- line Space

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    ////////////////////PLATE AND FİLLİNG TYPE
    //satış tipine göre değişiklik gösterir
    //#define Cash             0x00
    //#define TTS              0x01
    //#define MTS              0x02
    //#define Barrel           0x03
    //#define Transfer         0x04
    //#define Test             0x05
    //#define Manual           0x06
    //#define Undefined        0x07
    //#define Credit_Card      0x08
    //#define Loyalty          0x09
    //#define SaleName         0x0A



    //Plate Number
    add_space(((22-pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE.length())/2)-1);
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].PLATE);
    SendPrintData.append(0x0B);
    Customer_plate.clear();

    // 1- line Space

    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    if(ask_km==true)
    {
        //-----------------------
        SendPrintData.append("----------------------");
        SendPrintData.append(0x0A);
        SendPrintData.append("KM:");
        add_space((17-vehiclekmtr.length()));
        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append("*");
        SendPrintData.append(vehiclekmtr.toLatin1());
        SendPrintData.append(0x0A);
    }


    if(ask_km_after==true)
    {
        //-----------------------
        SendPrintData.append("----------------------");
        SendPrintData.append(0x0A);
        SendPrintData.append("KM:");
        add_space((17-vehiclekmtr.length()));
        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append("*");
        SendPrintData.append(vehiclekmtr.toLatin1());
        SendPrintData.append(0x0A);
    }
    //-----------------------
    SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    if(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.length()==0)
    {
        pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name="TANIMSIZ";

    }
    SendPrintData.append(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.toLatin1());
    convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_price,3,unit_price_dot_index);
    Unitprice_for_Database=printer_temp_buff_for_database;
    add_space((20-pump[pumpNo].nozzle[DCR_par.Active_Nozzle].Product_name.length()-(printer_temp_buff.length())));
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append("*");
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append(0x0B);

    //-----------------------
    SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    //TOPLAM:
    convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_volume,4,volume_dot_index);
    Volume_for_Database=printer_temp_buff_for_database;
    SendPrintData.append("TOP");
    //SendPrintData=SendPrintData.leftJustified(18,' ');
    add_space((14-printer_temp_buff.length()));
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append("*");
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append(" LT");
    SendPrintData.append(0x0B);

    //-----------------------
    SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    //////////////////Payload TYPE:  && amount////////////
    //#define Cash             0x00
    //#define TTS              0x01
    //#define MTS              0x02
    //#define Barrel           0x03
    //#define Transfer         0x04
    //#define Test             0x05
    //#define Manual           0x06
    //#define Undefined        0x07
    //#define Credit_Card      0x08
    //#define Loyalty          0x09
    //#define SaleName             0x0A


    Payload_type=0;
    switch (Payload_type)  //FOR DATABASE
    {
    case 0:
        type="NAKIT";
        break;
    case 1:
        type="TTS";
        break;
    case 2:
        type="MTS";
        break;
    case 3:
        type="NAKIT";
        break;
    case 4:
        type="TRANSFER";
        break;
    case 5:
        type="TEST";
        break;
    case 6:
        type="MANUAL";
        break;
    case 7:
        type="TANIMSIZ";
        break;
    case 8:
        type="KKARTI";
        break;
    case 9:
        type="LOYALTY";
        break;
    case 10:
        type="ISMESATIS";
        break;
    default:
        break;
    }

    convert_hex_to_ascii(pump[pumpNo].nozzle[DCR_par.Active_Nozzle].filling_amount,4,amount_dot_index);
    Amount_for_Database=printer_temp_buff_for_database;
    SendPrintData.append(type.toLatin1());
    add_space((21-printer_temp_buff.length()-type.length())-1);
    SendPrintData.append(space_buffer);space_buffer.clear();
    SendPrintData.append("*");
    SendPrintData.append(printer_temp_buff);
    SendPrintData.append(0x0B);
    //-----------------------
    SendPrintData.append("----------------------");
    SendPrintData.append(0x0A);

    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    //POMPA NO:  && TAB. NO
    SendPrintData.append("POMPA NO:");

    SendPrintData.append(QString::number(cash_register_pump_no).toLatin1());
    add_space((5-QString::number(cash_register_pump_no).toLatin1().length()-
               QString::number(DCR_par.Active_Nozzle).toLatin1().length())-1);
    SendPrintData.append(space_buffer); space_buffer.clear();
    SendPrintData.append("TAB. NO:");
    SendPrintData.append(QString::number(DCR_par.Active_Nozzle).toLatin1());
    SendPrintData.append(0x0A);

    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);




    //end_info-1

    if((End_info[0].length()>0) && (End_info[0].length()<22))
    {

        if(End_info[0].length()<20)
        {
            add_space(((22-End_info[0].length())/2)-1);
        }
        else
        {
            add_space((22-End_info[0].length()-1));
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(End_info[0].toLatin1());
        SendPrintData.append(0x0A);
    }


    //end_info-2
    if((End_info[1].length()>0)&&(End_info[1].length()<22))
    {

        if(End_info[1].length()<20)
        {
            add_space(((22-End_info[1].length())/2)-1);
        }
        else
        {
            add_space((22-End_info[1].length())-1);
        }

        SendPrintData.append(space_buffer);space_buffer.clear();
        SendPrintData.append(End_info[1].toLatin1());
        SendPrintData.append(0x0A);

    }

    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    //1-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);

    SendPrintData.append("   MALI ISLEM DISI");
    SendPrintData.append(0x0B);
    SendPrintData.append("  SATIS FISI KOPYASI");
    SendPrintData.append(0x0B);

    //2-line space
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);
    SendPrintData.append(space.toLatin1());
    SendPrintData.append(0x0A);


    SendPrintData.append(0xFA);
    SendPrintDataCOPY=SendPrintData;
    if(test)qDebug()<<SendPrintData;
    SCPArray[1]->writePrintData(SendPrintData);
    */
}

//////////////////////////////////////////////////////////////////////////
/// \brief ScpRoutine::decode_TagID
///////////////////////////////////////////////////////////////////////////

void ScpRoutine::decode_TagID()
{

    Delta_m.Byte[3]=Decoding_Key[0];
    Delta_m.Byte[2]=Decoding_Key[1];
    Delta_m.Byte[1]=Decoding_Key[2];
    Delta_m.Byte[0]=Decoding_Key[3];
    if(ReadTempData_RFID[last_index_RFID]==0xFA){
        if(ReadTempData_RFID[2]==CardReader_Addr[0] && ReadTempData_RFID[3]==CardReader_Addr[1]){
            union aBffr_t {
                unsigned long ByteArray[4];
                unsigned char Byte[32];
            } aBffr;

            unsigned long vw[4] = {0x32464741, 0x11131519, 0x26232528, 0x42454649};
            unsigned long vw0 = 0, vw1 = 0, vw2 = 0;

            char i = 0, k = 0;
            i = 0;
            k = 0;
            while (i < 32) {

                aBffr.Byte[k] = (((unsigned char) ReadTempData_RFID[i + 5] - 48) * 16) + ((unsigned char) ReadTempData_RFID[i + 6] - 48);
                i += 2;
                k++;
            }
            vw0 = (Delta_m.LongWrd * 2);
            vw1 = aBffr.ByteArray[0];
            vw2 = aBffr.ByteArray[1];
            while (vw0 != 0) {
                vw2 -= ((((vw1 << 4) ^ (vw1 >> 5)) + vw1) ^ (vw0 + vw[((vw0 >> 11) & 3)]));
                vw0 -= Delta_m.LongWrd;
                vw1 -= ((((vw2 << 4) ^ (vw2 >> 5)) + vw2) ^ (vw0 + vw[(vw0 & 3)]));
            }
            aBffr.ByteArray[0] = vw1;
            aBffr.ByteArray[1] = vw2;
            vw0 = (Delta_m.LongWrd * 2);
            vw1 = aBffr.ByteArray[2];
            vw2 = aBffr.ByteArray[3];
            while (vw0 != 0) {
                vw2 -= ((((vw1 << 4) ^ (vw1 >> 5)) + vw1) ^ (vw0 + vw[((vw0 >> 11) & 3)]));
                vw0 -= Delta_m.LongWrd;
                vw1 -= ((((vw2 << 4) ^ (vw2 >> 5)) + vw2) ^ (vw0 + vw[(vw0 & 3)]));
            }
            aBffr.ByteArray[2] = vw1;
            aBffr.ByteArray[3] = vw2;

            for (i = 0; i < 8; i++) {

                ReadTAG[i] = aBffr.Byte[i];

            }
            if(test) qDebug()<<"TAG-VALUE-USC-FALSE00"<<ReadTAG.toHex();

        }

    }

    Decoding_Key.clear();
    //qDebug()<<"read tag : "<<ReadTAG.toHex();
    if(USC_is_exist==false) //USC Yoksa
    {
        if(test)qDebug()<<"TAG-VALUE-USC-FALSE"<<ReadTAG.toHex();
        settings.setlog("SCP_DEBUG  TAG-VALUE-USC-FALSE "+QString::fromUtf8(ReadTAG.toHex()));
        if((ReadTAG[3]!=0))
        {
            suspend_first=false;
            if(ReadTAG[0]==0xEE && ReadTAG[1]==0x01){
                if(test)qDebug()<<"HATA:"<<"Dağıtıcı Firma Hatası.";
                RF_Reader_ErrorCode=0x01;
                Tag_read_flg=0;
            }
            else if(ReadTAG[0]==0xEE && ReadTAG[1]==0x02){
                if(test)qDebug()<<"HATA:"<<"Araç Tag Hatası.";
                RF_Reader_ErrorCode=0x02;
                Tag_read_flg=0;
            }
            else if(ReadTAG[0]==0xEE && ReadTAG[1]==0x03){
                if(test)qDebug()<<"HATA:"<<"ID Uyuşmazlığı Hatası.";
                RF_Reader_ErrorCode=0x03;
                Tag_read_flg=0;
            }
            else if(ReadTAG[0]==0xEE && ReadTAG[1]==0x04){
                if(test)qDebug()<<"HATA:"<<"Nozzle Kit Hatası.";
                RF_Reader_ErrorCode=0x04;
                Activation_Nozzle=ReadTempData_RFID[4]-0x30;
                activation_flg=true;
                Tag_read_flg=0;
            }
            else
            {
                if(test)qDebug()<<"Tag_read_flg"<<Tag_read_flg;
                tag_read_count++;
                if(tag_read_count>=10){tag_read_count=0;}
                if((tag_read_count==4) &&(Tag_read_count_flg!=1))
                {
                    TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                    Temp_TAGID=TAGID[ReadTempData_RFID[4]-0x30];
                    if(test)qDebug()<<"TAG_read_3"<<Temp_TAGID.toHex()<<TAGID[ReadTempData_RFID[4]-0x30].toHex();
                    tag_read_count=0;
                    Tag_read_zero_count=0;
                    if(card_authorize==false)
                    {
                        TAG_read=true;
                        Temp_ID[0]=QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()); ID_count++;
                        settings.setlog("SATIS_DEBUG  TAG-VALUE-USC-FALSE-READ-TAG "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30]));
                    }
                    if(test)qDebug()<<"TAGRFID_card_authorize"<<card_authorize;
                    if(card_authorize==true)
                    {
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Temp_PLATE=pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE;
                        //qDebug()<<"TTS-temp-PLATE"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Temp_PLATE.toHex();
                        CustomerID=TAGID[DCR_par.DCR_Nozzle_Number].toHex();
                        qDebug()<<"offffgr";
                        if(database->kontrolSalesRfid(CustomerID))
                        {
                            if(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].Temp_PLATE==pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE)
                            {
                                Temp_ID[0]=QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()); ID_count++;
                                //qDebug()<<"Wrong_trans_go_on"<<Temp_ID[0];
                                Person_is_Ok=false;
                                saveID();
                            }
                            else
                            {
                                Wrong_trans=true;
                            }
                        }
                        else
                        {
                            Wrong_trans=true;
                        }
                        //qDebug()<<"TTS-temp-PLATE"<<pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE.toHex();
                        if(test)qDebug()<<"TAGRFID_Wrong_trans"<<Wrong_trans<<Temp_TAGID.toHex();
                        settings.setlog("SCP_DEBUG  TAG-VALUE-USC-FALSE-TAGRFID_Wrong_trans "+QString::fromUtf8(ReadTAG.toHex()));
                    }
                    RFID_Read_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                    RFID_Read_Time.prepend("20");
                    RFID_Result_Time=RFID_Read_Time;
                    RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+1,2,"00");
                    //qDebug()<<"RFID_Read_Time"<<RFID_Read_Time;
                    //qDebug()<<"TAGRFID_Read_Time"<<RFID_Read_Time;
                    if(test_modu==false)buzzer_on_flg=1;
                    Tag_read_count_flg=1;//idnin sürekli algılanması engelleyen
                    Tag_read_flg=1; // Tag_read_flg=0 suspend Tag_read_flg=1 satışta
                    odometre=true;
                }
                if((Tag_read_flg==1) && (Pump_Filling_status==1)) //ilk ID okunduysa ve pompa satıştaysa
                {
                    if(test)qDebug()<<"TAG_read_5"<<Temp_TAGID.toHex()<<ReadTAG.toHex();
                    if(Temp_TAGID!=ReadTAG)  //ilk okunan ıd ile sonraki idler farklı ise
                    {
                        if(ttspause==true)
                        {
                            Sale_suspend_flg=true;
                            settings.setlog("SCP_DEBUG  TAG-VALUE-USC-FALSE-Sale_suspend "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()));
                            Tag_read_flg=0; //sale suspend
                        }
                    }

                }

                if((Pump_Filling_status==1) && (Tag_read_flg==0)) //ssuspend gönderilmiş satış
                {

                    if(Temp_TAGID!=ReadTAG)
                    {
                        if(test)qDebug()<<"TAG_read_6"<<Tag_read_flg;
                        Tag_read_flg=0; //sale suspend

                    }
                    else
                    {
                        TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                        tag_resume_count++;   //satış esnsasında aynı ıd geri geldiyse
                        if(tag_resume_count>=4)
                        {
                            if(Not_resume_TAG!=true)
                            {
                                Sale_resume_flg=true;  //sale resume
                                settings.setlog("SCP_DEBUG  TAG-VALUE-USC-FALSE-Sale_Resume "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()));
                                if(test)qDebug()<<"TAG_read_7";
                                Tag_read_flg=1;
                                tag_resume_count=0;
                            }
                        }

                    }

                }

                if((Tag_read_count_flg!=0) &&((LCD_status==4)||(LCD_status==11)))
                {
                    if(Temp_TAGID!=ReadTAG)  //ilk okunan ıd ile sonraki idler farklı ise
                    {
                        if(test)qDebug()<<"TAG_read_8"<<Tag_read_flg;
                        settings.setlog("SCP_DEBUG  TAG-VALUE-USC-FALSE-ID-FARKLI "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()));
                        Tag_read_flg=0; //sale suspend
                        Customer_plate.clear();
                        Keypad_data_for_LCD.clear();
                        for(int jj=0; jj<16;jj++)
                        {
                            TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                            databuf[jj]=0;
                        }

                        Temp_TAGID.clear();
                        qDebug()<<"cl6";
                        vehiclekmtr.clear();
                        Cancel_flg=1;
                        Enter_flg=0;
                        usleep(300);
                        LCD_refresh_flg=1;
                        tag_read_count=4;
                        Tag_read_count_flg=0;
                        delete_last_sale_flg=1;
                        LCD_status=1;
                        LCD_refresh_flg=1;
                    }
                }

            }
        }
        else
        {

            if(test) qDebug()<<"Tag_read_flg"<<Tag_read_flg<<Tag_read_zero_count;
            if(test) qDebug()<<"TAG_read_10"<<Temp_TAGID.toHex()<<TAGID[ReadTempData_RFID[4]-0x30].toHex();
            Tag_read_zero_count++;
            if(Tag_read_zero_count>=2)
            {
                Tag_read_zero_count=0;
                for(int jj=0; jj<16;jj++)
                {
                    TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                    databuf[jj]=0;
                }

                tag_read_count=0;
                if((Pump_Filling_status==1) && (Tag_read_flg==1))  //satış esnsaında ıd 0 okunduysa
                {
                    if(ttspause==true)
                    {
                        tag_suspend_count++;   //belli bir süre sonra
                        if(tag_suspend_count>=tts_pause_count)
                        {
                            if(test)qDebug()<<"TAG_read_9"<<Tag_read_flg;
                            Tag_read_flg=0;  //sale suspend
                            settings.setlog("SCP_DEBUG  TAG-VALUE-USC-FALSE-Sale_suspend-ID-SIFIR "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()));
                            Sale_suspend_flg=true;
                            suspend_first=true;
                            tag_suspend_count=0;
                        }
                    }
                }
                if((Tag_read_count_flg!=0) &&((LCD_status==4)||(LCD_status==11)))
                {
                    if(ttspause==true)
                    {
                        if(Temp_TAGID!=ReadTAG)  //ilk okunan ıd ile sonraki idler farklı ise
                        {
                            if(test)qDebug()<<"TAG_read_11"<<Tag_read_flg;
                            Tag_read_flg=0; //sale suspend
                            Customer_plate.clear();
                            Keypad_data_for_LCD.clear();
                            for(int jj=0; jj<16;jj++)
                            {
                                TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                                databuf[jj]=0;
                            }

                            Temp_TAGID.clear();
                            qDebug()<<"cl7";
                            vehiclekmtr.clear();
                            Cancel_flg=1;
                            Enter_flg=0;
                            usleep(300);
                            LCD_refresh_flg=1;
                            tag_read_count=4;
                            Tag_read_count_flg=0;
                            delete_last_sale_flg=1;
                            LCD_status=1;
                            LCD_refresh_flg=1;
                        }
                    }
                }

            }

        }
    }
    else //USC bağlantısı varsa
    {
        if(test) qDebug()<<"TAGgggggggggg-USC-ok"<<ReadTAG.toHex()<<TAGID[1].toHex();

        if((ReadTAG[3]!=0))
        {
            suspend_first=false;
            if(ReadTAG[0]==0xEE && ReadTAG[1]==0x01){
                if(test)qDebug()<<"HATA:"<<"Dağıtıcı Firma Hatası.";
                RF_Reader_ErrorCode=0x01;
                TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                Tag_read_flg=0;
            }
            else if(ReadTAG[0]==0xEE && ReadTAG[1]==0x02){
                if(test)qDebug()<<"HATA:"<<"Araç Tag Hatası.";
                RF_Reader_ErrorCode=0x02;
                TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                Tag_read_flg=0;
            }
            else if(ReadTAG[0]==0xEE && ReadTAG[1]==0x03){
                if(test)qDebug()<<"HATA:"<<"ID Uyuşmazlığı Hatası.";
                RF_Reader_ErrorCode=0x03;
                TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                Tag_read_flg=0;
            }
            else if(ReadTAG[0]==0xEE && ReadTAG[1]==0x04){
                if(test)qDebug()<<"HATA:"<<"Nozzle Kit Hatası.";
                RF_Reader_ErrorCode=0x04;
                TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                Tag_read_flg=0;
            }
            else
            {
                //qDebug()<<"Tag_read_flg"<<Tag_read_flg;
                tag_read_count++;
                if(tag_read_count>=10){tag_read_count=0;}
                if((tag_read_count==2) &&(Tag_read_count_flg!=1))
                {
                    //                        TEXT_fromMaster=idTaken_ByteArray;
                    //                        LCD_refresh_flg=1;
                    TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                    //qDebug()<<"TAG_read_3-USC-ok"<<Temp_TAGID.toHex()<<TAGID[ReadTempData_RFID[4]-0x30].toHex();
                    settings.setlog("SCP_DEBUG  TAG-VALUE-USC-OK-ReadTAG "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()));
                    //qDebug()<<"ID_countID_countID_countID_countID_count"<<ID_count;
                    if(ID_count<3)
                    {
                        ID_count=0;
                        Temp_ID[ID_count]=QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex());
                        ID_count++;
                    }
                    Temp_TAGID=TAGID[ReadTempData_RFID[4]-0x30]; ///sonradan
                    tag_read_count=0;
                    TAG_read=true;
                    RFID_Read_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                    RFID_Read_Time.prepend("20");
                    RFID_Result_Time=RFID_Read_Time;
                    RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+1,2,"00");
                    if(test)qDebug()<<"TAGRFID_Read_Time"<<RFID_Read_Time;
                    StoredScreenRFID_fromMaster=0;
                    if(test_modu==false)buzzer_on_flg=1;
                    Tag_read_count_flg=1;//idnin sürekli algılanması engelleyen
                    Tag_read_flg=1; // Tag_read_flg=0 suspend Tag_read_flg=1 satışta
                }

                //  if(Tag_read_flg==1) //ilk ID okunduysa ve pompa satıştaysa
                //  {
                USC_ok_read_count++;
                if(USC_ok_read_count>=3)
                {
                    USC_ok_read_count=0;
                    TAGID[ReadTempData_RFID[4]-0x30]=ReadTAG;
                    if(test) qDebug()<<"TAGgggggggggg-USC-ok-2"<<ReadTAG.toHex()<<TAGID[1].toHex();
                }
                //  }

            }

        }
        else
        {
            settings.setlog("SCP_DEBUG  TAG-VALUE-USC-OK-ID-SIFIR  "+QString::fromUtf8(TAGID[ReadTempData_RFID[4]-0x30].toHex()));
            if(test)qDebug()<<"SCP_DEBUG  TAG-VALUE-USC-OK-ID-SIFIR";
            for(int jj=0; jj<16;jj++)
            {
                TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                databuf[jj]=0;
            }
            ZeroID_count++;
            if(ZeroID_count>4)
            {
                ZeroID_count=0;
                if(((LCD_status==11) || (LCD_status==3)) && (Tag_read_flg==1))//km giriniz ekranındaysa tag 0 okunursa
                {
                    LCD_status=9;
                    StoredScreenRFID_fromMaster=8;
                    TEXT_fromMaster=idError_ByteArray;

                }
            }
        }

    }

}

void ScpRoutine::read_RFID(){
    ReadData_RFID.clear();
    ReadTempData_RFID.clear();
    SCPArray[3]->readData(ReadData_RFID);
    if(ReadData_RFID.length()>0)
    {
        ReadTempData_RFID=ReadData_RFID;
        last_index_RFID=ReadTempData_RFID.lastIndexOf(0xFA);
        if(ReadTempData_RFID[0]==0x02 && ReadTempData_RFID[1]==0x52 && last_index_RFID>0){
            rfid_check_flg=true;
            tag_Random_flg=1;
            decode_TagID();
        }
        else if(ReadTempData_RFID[0]==0x02 && ReadTempData_RFID[1]==0xFF && ReadTempData_RFID[4]==0x32)
        {
            rfid_check_flg=true;
            read_configuration=false;
            read_acc_auto_adr=QString::fromUtf8(ReadTempData_RFID.mid(5,2));
            read_acc_nozzle_adr=QString::fromUtf8(ReadTempData_RFID.mid(7,2));
            read_acc_channel=QString::fromUtf8(ReadTempData_RFID.mid(9,2));
            read_acc_wait_time=QString::fromUtf8(ReadTempData_RFID.mid(11,1).toHex());
            read_configuration_success=1;
            try_read_count=0;
        }
        else if(ReadTempData_RFID[0]==0x02 && ReadTempData_RFID[1]==0xFF && ReadTempData_RFID[4]==0x31)
        {
            rfid_check_flg=true;
            write_configuration=false;
            write_configuration_success=1;
            try_write_count=0;
        }
        else if(ReadTempData_RFID[0]==0x02 && ReadTempData_RFID[1]==0x87)
        {
            odometre=false;
            odometre_KM=QString::fromUtf8(ReadTempData_RFID.mid(5,8));
            odometre_time=QString::fromUtf8(ReadTempData_RFID.mid(13,8));

            odometre_time=QString::number(odometre_time.toDouble()/60);
            if(odometre_time.indexOf(".")>=0)
            {
                QStringList odometre_split=odometre_time.split(".");
                odometre_time=odometre_split.at(0)+".";
                odometre_time+=odometre_split.at(1).mid(0,2);
            }
            settings.setlog("RFID_DEBUG ODOMETRE KM : "+odometre_KM+"  SAAT : "+odometre_time);

        }
        else rfid_check_flg=false;
    }
    else
    {
        rfid_check_flg=false;
        tag_Random_flg=0;
    }

}
void ScpRoutine::send_odometre(int NZL)
{
    SendData3[0]=STX;
    SendData3[1]=0x87;
    SendData3[2]=CardReader_Addr[0];
    SendData3[3]=CardReader_Addr[1];
    SendData3[4]=0x30+NZL;
    SendData3[5]=pump[pumpNo].nozzle[NZL].Coding_Key[0];
    SendData3[6]=pump[pumpNo].nozzle[NZL].Coding_Key[1];
    SendData3[7]=pump[pumpNo].nozzle[NZL].Coding_Key[2];
    SendData3[8]=pump[pumpNo].nozzle[NZL].Coding_Key[3];
    for(int i=0;i<4;i++){
        Decoding_Key[i]=pump[pumpNo].nozzle[NZL].Coding_Key[i];
    }
    SCPArray[3]->writeRFIDData(SendData3,9);
}

void ScpRoutine::routine_3()//RFID
{
    if(Allow_card_read==1)
    {
        if((only_card_read_flg==false) && (meter_com_err==false))
        {
            currentTime(DCR_par.DCR_Nozzle_Number); //USC mevcut değilse Random key üret
            read_RFID();
            if(Tag_read_flg==1 && odometre_active)
            {
                if(odometre)
                {
                    send_odometre(DCR_par.DCR_Nozzle_Number);
                }
                else
                {
                    send_CMD1(DCR_par.DCR_Nozzle_Number); // tag okuma mesajı
                }
            }
            else
            {
                send_CMD1(DCR_par.DCR_Nozzle_Number);// tag okuma mesajı
            }
            send_CMD1(DCR_par.DCR_Nozzle_Number);
        }

    }
    else if(test_modu)
    {
        currentTime(DCR_par.DCR_Nozzle_Number);
        read_RFID();
        send_CMD1(DCR_par.DCR_Nozzle_Number);
    }
    else
    {
        if(read_configuration==true)
        {
            if(try_read_count<=5)
            {
                try_read_count++;
                send_read_acc_config();
            }
            else
            {
                read_configuration=false;
                read_configuration_success=2;
                try_read_count=0;
            }
        }
        if(write_configuration==true)
        {
            if(try_write_count<=5)
            {
                try_write_count++;
                send_write_acc_config();
            }
            else
            {
                write_configuration=false;
                write_configuration_success=2;
                try_write_count=0;
            }
        }
        if(activation_flg==true)
        {
            activation_flg=false;
            send_CMDActivation(Activation_Nozzle);
        }
        read_RFID();
    }

}

void ScpRoutine::send_read_acc_config()
{
    QByteArray AccSettingPacket;
    AccSettingPacket[0] = 0x02;
    AccSettingPacket[1] = 0xFF;
    AccSettingPacket[2] = 0x39;
    AccSettingPacket[3] = 0x39;
    AccSettingPacket[4] = 0x32;
    SCPArray[3]->writeRFIDData(AccSettingPacket,5);
}

void ScpRoutine::send_write_acc_config()
{
    if(acc_otomasyon_adresi.length()<2)
    {
        acc_otomasyon_adresi=acc_otomasyon_adresi.prepend("0");
    }
    if(acc_channel_no_adresi.length()<2)
    {
        acc_channel_no_adresi=acc_channel_no_adresi.prepend("0");
    }
    if(acc_nozzle_adresi.length()<2)
    {
        acc_nozzle_adresi=acc_nozzle_adresi.prepend("0");
    }
    if(acc_pause_time.length()>1)
    {
        acc_pause_time=acc_pause_time.mid(1,1);
    }
    QByteArray AccSettingPacket;
    AccSettingPacket[0]=0x02;
    AccSettingPacket[1]=0xFF;
    AccSettingPacket[2]=0x39;
    AccSettingPacket[3]=0x39;
    AccSettingPacket[4]=0x31;
    AccSettingPacket[5]=acc_otomasyon_adresi[0];//otomasyon adresi
    AccSettingPacket[6]=acc_otomasyon_adresi[1];

    AccSettingPacket[7]=acc_nozzle_adresi[0];//nozzle adresi
    AccSettingPacket[8]=acc_nozzle_adresi[1];

    AccSettingPacket[9]=acc_channel_no_adresi[0];//kanal numarası
    AccSettingPacket[10]=acc_channel_no_adresi[1];

    AccSettingPacket[11]=acc_pause_time[0]-0x30;//pause time
    SCPArray[3]->writeRFIDData(AccSettingPacket,12);
}

void ScpRoutine::save_to_sdcard()
{
    QDir dir("/media/root/");
    QFileInfoList list_usb=dir.entryInfoList();
    for(int i=0;i<list_usb.size();i++)
    {
        QString file_name=list_usb.at(i).fileName();
        if(file_name=="MEPSAN")
        {
            system("cp -f /var/www/html/Upc_Database /media/root/MEPSAN/Upc_Database");
            system("cp -f /var/www/html/setting_save.txt /media/root/MEPSAN/setting_save.txt");
            break;
        }
    }
    sync();
    system("sync");
}
