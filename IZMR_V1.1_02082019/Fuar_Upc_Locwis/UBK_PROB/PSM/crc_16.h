#ifndef CRC_16_H
#define CRC_16_H
unsigned int calcrc(char *ptr,unsigned int lenght,unsigned int crc_ini);
unsigned int USCcalcrc(char *ptr,unsigned int lenght,unsigned int crc_ini);
unsigned int RFIDcalcrc(char *ptr,unsigned int lenght,unsigned int crc_ini);
#endif // CRC_16_H
