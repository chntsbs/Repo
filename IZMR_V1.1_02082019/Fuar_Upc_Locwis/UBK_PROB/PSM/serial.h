#ifndef SERIAL_H
#define SERIAL_H

#include <QCoreApplication>
#include <QtSerialPort/QtSerialPort>

#define MaxSCPCount 4

class serialport : public QObject
{
   Q_OBJECT
public:
     serialport (int portnumber);
     int PortNumber;
     QSerialPort *serial;


signals:
//finished();
   void  serialsignal();

public slots:
    void serial_init();
    void close_port();
    void writeData(QByteArray &data, unsigned char crc_ind);
    void writePrintData(QByteArray &data);    
    void writeMeterData(QByteArray data, unsigned char crc_ind);
    QByteArray crypmeterdata(QByteArray data);
    void writeRFIDData(QByteArray &data,unsigned char crc_ind);
    void writeRepeatData(QByteArray &data);
    void send_poll(QByteArray &polldata);
    void send_withoutCRC(QByteArray &data);
    void send_ini(QByteArray &inidata);
    void send_ack(QByteArray &ackdata);
    void readData(QByteArray &RxData);
    void readMeterData(QByteArray &RxData);
    void readSCUData(QByteArray &RxData);
    void send_nak(QByteArray &data);
    void make_for_buf_meter(uint *ptr, int length);
    void Encode_meter(long* data, char dataLength, quint32 delta,long *keyptr);
    void make_for_Crypto_buf_meter(uint *ptr, int length);
    void readData_printer(QByteArray &RxData);
    void readData_serial_probe(QByteArray &RxData);
    void write_probe_data(QByteArray &data);

private:



};
extern serialport *SCPArray[MaxSCPCount+1];
#endif // SERIAL_H
