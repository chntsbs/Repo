#include "crc_16.h"
#include "global.h"

bool bit_test(volatile int data,char bit){
    if(bit)
        data>>=bit;
    if((data & 1)==1)
        return true;
    else return false;
}


unsigned int calcrc(char *ptr, unsigned int lenght, unsigned int crc_ini){
    unsigned int crcresult;
    unsigned char k,i,temp;
    bool carry_in=0,carry_out=0;
    
    crcresult=crc_ini;
    
    for(i=0;i<lenght;i++)
    {
        temp=ptr[i];//get next byte from string to temp
        for(k=0;k<8;++k)
        {
            if(bit_test(crcresult,0)!=bit_test(temp,0))
            {carry_in=1;crcresult^=0x4002;}
            else {carry_in=0;}
            if(bit_test(crcresult,0))carry_out=1;
            else carry_out=0;
            crcresult>>=1;temp>>=1;
            if(carry_in)crcresult|=0x8000;
            if(carry_out)temp|=0x80;
        }
    }
    crc_msb=crcresult/256;
    crc_lsb=crcresult%256;
    return (crcresult);
}
unsigned int USCcalcrc(char *ptr, unsigned int lenght, unsigned int crc_ini){
    unsigned int crcresult;
    unsigned char k,i,temp;
    bool carry_in=0,carry_out=0;

    crcresult=crc_ini;

    for(i=0;i<lenght;i++)
    {
        temp=ptr[i];//get next byte from string to temp
        for(k=0;k<8;++k)
        {
            if(bit_test(crcresult,0)!=bit_test(temp,0))
            {carry_in=1;crcresult^=0x4002;}
            else {carry_in=0;}
            if(bit_test(crcresult,0))carry_out=1;
            else carry_out=0;
            crcresult>>=1;temp>>=1;
            if(carry_in)crcresult|=0x8000;
            if(carry_out)temp|=0x80;
        }
    }
    USC_crc_msb=crcresult/256;
    USC_crc_lsb=crcresult%256;
    return (crcresult);
}
unsigned int RFIDcalcrc(char *ptr, unsigned int lenght, unsigned int crc_ini){
    unsigned int crcresult;
    unsigned char k,i,temp;
    bool carry_in=0,carry_out=0;

    crcresult=crc_ini;

    for(i=0;i<lenght;i++)
    {
        temp=ptr[i];//get next byte from string to temp
        for(k=0;k<8;++k)
        {
            if(bit_test(crcresult,0)!=bit_test(temp,0))
            {carry_in=1;crcresult^=0x4002;}
            else {carry_in=0;}
            if(bit_test(crcresult,0))carry_out=1;
            else carry_out=0;
            crcresult>>=1;temp>>=1;
            if(carry_in)crcresult|=0x8000;
            if(carry_out)temp|=0x80;
        }
    }
    RFIDcrc_msb=crcresult/256;
    RFIDcrc_lsb=crcresult%256;
    return (crcresult);
}
