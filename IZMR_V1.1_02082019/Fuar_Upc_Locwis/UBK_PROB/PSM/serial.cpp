#include"config.h"
#include "serial.h"
#include <QtDebug>
#include"crc_16.h"
#include"global.h"
#include"syslog.h"
long key_meter[4] = { 0x32464741, 0x11131519, 0x26232528, 0x42454649 };

serialport *SCPArray[MaxSCPCount+1];
serialport::serialport(int portnumber)
{
    serial = new QSerialPort(this);
    this->PortNumber = portnumber;
    serial_init();

}
void serialport::serial_init()
{
    if(PortNumber==1) //printer
    {
        //serial->setBaudRate(19200);
        
        serial->setPortName("ttyAMA3");
        serial->open(QIODevice::ReadWrite);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setBaudRate(9600);
    }
    if(PortNumber==2) //metre
    {
        serial->setPortName("ttyAMA0");
        serial->open(QIODevice::ReadWrite);
        serial->setBaudRate(9600);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setDataBits(QSerialPort::Data8);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setParity(QSerialPort::OddParity);
    }
    if(PortNumber==3) //868
    {
        serial->setPortName("ttyAMA1");
        serial->open(QIODevice::ReadWrite);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setDataBits(QSerialPort::Data8);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setBaudRate(9600);
        serial->setParity(QSerialPort::OddParity);
        
    }
    if(PortNumber==4) //USC OR PROBE
    {
        if(upc_locvis==true)
        {
            serial->setPortName("ttyAMA2");
            serial->open(QIODevice::ReadWrite);
            serial->setBaudRate(9600);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setDataBits(QSerialPort::Data8);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setParity(QSerialPort::EvenParity);
        }
        else
        {
            serial->setPortName("ttyAMA2");
            serial->open(QIODevice::ReadWrite);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setDataBits(QSerialPort::Data8);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setBaudRate(9600);
            serial->setParity(QSerialPort::OddParity);
        }
    }

}


void serialport::close_port()
{
    if (serial->isOpen()){serial->close();}
}
void serialport::writeData(QByteArray &data,unsigned char crc_ind)
{
    //    Crypto  Crypt2;
    //    USCcalcrc(data.data(),crc_ind,0);
    //    if(USC_crc_msb==0xfa)
    //    {
    //     data[crc_ind]=USC_crc_lsb;
    //     data[crc_ind+1]=0x10;
    //     data[crc_ind+2]=USC_crc_msb;
    //     data[crc_ind+3]=ETX;
    //     data[crc_ind+4]=SF;
    //    }
    //    else
    //    {
    //    data[crc_ind]=USC_crc_lsb;
    //    data[crc_ind+1]=USC_crc_msb;
    //    data[crc_ind+2]=ETX;
    //    data[crc_ind+3]=SF;
    //    }
    //    data=QString::fromStdString(Crypt2.encryptByteArray(data.toHex())).toLatin1();
    //    data.append("mlb");
    //    serial->write(data);
    //    qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<>>>SEND"<<data;
    //    if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_USC_DEBUG  >>>SEND: %s ",data.toHex().constData());

    USCcalcrc(data.data(),crc_ind,0);
    if(USC_crc_msb==0xfa)
    {
        data[crc_ind]=USC_crc_lsb;
        data[crc_ind+1]=0x10;
        data[crc_ind+2]=USC_crc_msb;
        data[crc_ind+3]=ETX;
        data[crc_ind+4]=SF;
    }
    else
    {
        data[crc_ind]=USC_crc_lsb;
        data[crc_ind+1]=USC_crc_msb;
        data[crc_ind+2]=ETX;
        data[crc_ind+3]=SF;
    }
    serial->write(data);
    //if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_USC_DEBUG  >>>SEND: %s ",data.toHex().constData());
}


void serialport::make_for_Crypto_buf_meter(uint *ptr, int length)
{
    char j;
    long tmp1,tmp11,tmp12,tmp13;
    if(length/4<32)
    {
        for (j=0;j<length/4;j++)
        {
            Crypto_BUF_meter[j]=0;
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
            Crypto_BUF_meter[j]=tmp1+tmp11+tmp12+tmp13;
        }
    }

}

void serialport::readData_printer(QByteArray &RxData)
{

    RxData = serial->readAll();
    while (serial->bytesAvailable())
        RxData.append(serial->readAll());
    //  QByteArray data = serial->readAll();
    //qDebug()<<"*****************************************RxData"<<RxData;

}

void serialport::readData_serial_probe(QByteArray &RxData)
{

    RxData = serial->readAll();
    while (serial->bytesAvailable())
        RxData.append(serial->readAll());
    //  QByteArray data = serial->readAll();
    //qDebug()<<"*****************************************RxData"<<RxData;

}


void serialport:: Encode_meter(long* data, char dataLength, quint32 delta,long *keyptr)
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


void serialport:: make_for_buf_meter(uint *ptr, int length)
{
    char j;
    long tmp1,tmp11,tmp12,tmp13;
    for (j=1;j<length;j++)
    {
        ptr[j]=0;
    }
    for (j=0;j<(length/4);j++)
    {
        tmp1=Crypto_BUF_meter[j];
        tmp11=Crypto_BUF_meter[j];
        tmp12=Crypto_BUF_meter[j];
        tmp13=Crypto_BUF_meter[j];

        ptr[(j*4)+0]=(char)0x000000ff & tmp1;
        tmp11>>=8;
        ptr[(j*4)+1]=(char)0x000000ff & tmp11;
        tmp12>>=16;
        ptr[(j*4)+2]=(char)0x000000ff & tmp12;
        tmp13>>=24;
        ptr[(j*4)+3]=(char)0x000000ff & tmp13;

    }

}

QByteArray serialport::crypmeterdata(QByteArray data)
{   uint data_databuf[100];
    if(test) qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>original_meter_data"<<data.toHex();
    int append_len=data.length();
    int zero=0;
    if(append_len%8!=0)
    {
        for(int jj=0;jj<((append_len/8)+1)*8-append_len;jj++)
        {
            data.append(zero);
        }
    }
    append_len=data.length();
    for(int jj=0; jj<data.length();jj++)
    {
        data_databuf[jj]=data[jj];
    }
    quint32 delta_meter=Coding_key.toHex().toUInt(&ok,16);
    make_for_Crypto_buf_meter(data_databuf, data.length());
    Encode_meter(Crypto_BUF_meter,(append_len/4), delta_meter, key_meter);
    make_for_buf_meter(data_databuf, data.length());
    int mm=0;
    int datalen=data.length();

    for (int nn = 0; nn < datalen; nn++)
    {
        data[mm] = (((data_databuf[nn] >> 4) & 0x0f) + 0x30); mm++;
        data[mm] = (((data_databuf[nn]) & 0x0f) + 0x30);      mm++;
    }
    int tempend=0xFA;
    data.append(tempend);
    //qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>crypted_meter_data"<<data.toHex();
    return data;

}
void serialport::writeMeterData(QByteArray data,unsigned char crc_ind)
{
    switch (select_version) {
    case 1:calcrc(data.data(),crc_ind,0);
        break;
    case 2:calcrc(data.data(),crc_ind,0x6204); // Different CRC
        break;
    case 3:calcrc(data.data(),crc_ind,0); //crypted
        break;
    default:
        break;
    }

    if((crc_msb==0xfa) && (crc_lsb!=0x10))
    {
        data[crc_ind]=crc_lsb;
        data[crc_ind+1]=0x10;
        data[crc_ind+2]=crc_msb;
        data[crc_ind+3]=ETX;
        data[crc_ind+4]=SF;
    }
    else if(crc_lsb==0xfa)
    {
        data[crc_ind]=0x10;
        data[crc_ind+1]=crc_lsb;
        data[crc_ind+2]=crc_msb;
        data[crc_ind+3]=ETX;
        data[crc_ind+4]=SF;
    }
    else if((crc_lsb==0x10) && (crc_msb==0xfa))
    {
        data[crc_ind]=0x10;
        data[crc_ind+1]=crc_lsb;
        data[crc_ind+2]=0x10;
        data[crc_ind+3]=crc_msb;
        data[crc_ind+4]=ETX;
        data[crc_ind+5]=SF;
    }
    else
    {
        data[crc_ind]=crc_lsb;
        data[crc_ind+1]=crc_msb;
        data[crc_ind+2]=ETX;
        data[crc_ind+3]=SF;
    }
    //qDebug()<<">>write_meter_data"<<data.toHex();
    ////////////////////////////////////////////////////////////////////////////////
    //serial->write(crypmeterdata(data));
    switch (select_version) {
    case 1:serial->write(data);
        break;
    case 2:serial->write(data); // Different CRC
        break;
    case 3:serial->write(crypmeterdata(data)); //crypted
        break;
    default:
        break;
    }
    //if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_METER_DEBUG  >>>SEND: %s ",data.toHex().constData());
    data.clear();
    token++;
    if(token>=0x0F){
        token=0;
    }
}

void serialport::writeRFIDData(QByteArray &data,unsigned char crc_ind)
{
    RFIDcalcrc(data.data(),crc_ind,0);
    if(RFIDcrc_msb==0xfa)
    {
        data[crc_ind]=RFIDcrc_lsb;
        data[crc_ind+1]=0x10;
        data[crc_ind+2]=RFIDcrc_msb;
        data[crc_ind+3]=ETX;
        data[crc_ind+4]=SF;
    }
    else
    {
        data[crc_ind]=RFIDcrc_lsb;
        data[crc_ind+1]=RFIDcrc_msb;
        data[crc_ind+2]=ETX;
        data[crc_ind+3]=SF;
    }
    serial->write(data);
    token++;
    if(token>=0x0F){
        token=0;
    }
}
void serialport::writePrintData(QByteArray &data )
{
    serial->write(data);
}
void serialport::writeRepeatData(QByteArray &data )
{
    
    serial->write(data);
}

void serialport::write_probe_data(QByteArray &data)
{
    serial->write(data);
    //qDebug()<<"write data : "<<data.toHex();
}
void serialport::send_ack(QByteArray &ackdata)
{
    //qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>send_ack"<<ackdata.toHex();
    switch (select_version) {
    case 1:serial->write(ackdata.data());
        break;
    case 2:serial->write(ackdata.data()); // Different CRC
        break;
    case 3:serial->write(crypmeterdata(ackdata).data()); //crypted
        break;
    default:
        break;
    }
    ackdata.clear();
}

void serialport::send_poll(QByteArray &polldata)
{      
    polldata.clear();
    polldata[0]=0x50;
    polldata[1]=0x20;
    polldata[2]=0xFA;
    //qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>send_poll"<<polldata.toHex();
    switch (select_version) {
    case 1:serial->write(polldata.data());
        break;
    case 2:serial->write(polldata.data()); // Different CRC
        break;
    case 3:serial->write(crypmeterdata(polldata).data()); //crypted
        break;
    default:
        break;
    }
    polldata.clear();
}
void serialport::send_withoutCRC(QByteArray &data)
{
    //     Crypto  Crypt3;
    //    data=QString::fromStdString(Crypt3.encryptByteArray(data.toHex())).toLatin1();
    //    data.append("mlb");
    //    serial->write(data);
    //    qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><>>>SEND"<<data;
    //    if(serial_debug_flg==1)syslog(LOG_DEBUG,"SERIAL_USC_DEBUG-send_withoutCRC  >>>SEND: %s ",data.toHex().constData());

    serial->write(data);
}


void serialport::send_ini(QByteArray &inidata)
{
    inidata.clear();
    inidata[0]=0x50;
    inidata[1]=0xC0;
    inidata[2]=0xFA;
    switch (select_version) {
    case 1:serial->write(inidata.data());
        break;
    case 2:serial->write(inidata.data()); // Different CRC
        break;
    case 3:serial->write(crypmeterdata(inidata).data()); //crypted
        break;
    default:
        break;
    }
    inidata.clear();
}

void serialport::readData(QByteArray &RxData)
{
    
    RxData = serial->readAll();
    while (serial->bytesAvailable())
        RxData.append(serial->readAll());
    //  QByteArray data = serial->readAll();
    //qDebug()<<"*****************************************RxData"<<RxData;
    
}



void serialport::readMeterData(QByteArray &RxData)
{
    
    
    RxData = serial->readAll();
    while (serial->bytesAvailable())
        RxData.append(serial->readAll());
    // QByteArray data = serial->readAll();
    // qDebug()<<" <<<Meter : "<< RxData.toHex();

}


void serialport::readSCUData(QByteArray &RxData)
{

    RxData = serial->read(1);
    while (serial->bytesAvailable())
        RxData.append(serial->read(1));

}

void serialport::send_nak(QByteArray &data)
{
    serial->write(data.data());
}
