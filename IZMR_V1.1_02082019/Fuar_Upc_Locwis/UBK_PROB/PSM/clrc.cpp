#include "clrc.h"
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <QCoreApplication>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <QDebug>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <QTimer>
#include <linux/i2c.h>
#include <QThread>
#include"global.h"
#include"syslog.h"

unsigned int ii;
int clrc_count=0;
int clrc_count2=0;
CLRC::CLRC(QObject *parent) :
    QObject(parent)
{
    DBuffer[30];
    I2CStartUp();
    //keybuzzer=new keypad(this);
    CLRCtimer1=new QTimer(this);
    Card_read_flg=0;
    Allow_card_read=0;
    
}

void CLRC::clrctoThread(QThread &clrcThread)
{
    connect(CLRCtimer1,SIGNAL(timeout()),this,SLOT(action()));
    connect(&clrcThread,SIGNAL(started()),this,SLOT(action_start()));
}

void CLRC::mysleep(unsigned int delay)
{
    unsigned long dd;
    
    for(dd=0;dd<=30*delay;dd++)
    {
        dd++;
    }

}



void CLRC::action_start()
{
    CLRCtimer1->start(1000);
}



void CLRC::I2CStartUp()
{
    if((i2c=open("/dev/i2c-2",O_RDWR))<0)
    {
        if(test)qDebug()<<"Number -"<<QString::number(i2c);
        if(test)qDebug()<<"Error: ";
    }
    else
        if(test)qDebug()<<"Number +"<<QString::number(i2c);
    if((ioc=ioctl(i2c,I2C_SLAVE,CSlave_Address))<0)
        if(test)qDebug()<<"Number -"<<QString::number(ioc);
        else
            if(test)qDebug()<<"Number +"<<QString::number(ioc);

    mysleep(10000);
}


void CLRC::CLRC_write_byte(uint8_t reg, uint8_t val)
{
    if(clrc_check_flg)
    {
        DBuffer[0] = reg;
        DBuffer[1] = val;
        write(i2c,DBuffer,2);
    }
}

unsigned char CLRC::CLRC_read_byte(uint8_t reg)
{
    if(clrc_check_flg)
    {
        unsigned char DBuffer[30];
        DBuffer[0] = reg;
        write(i2c,DBuffer,1);
        DBuffer[0]=0x00;
        err=read(i2c,DBuffer,1);
        readValue[0]=DBuffer[0];
        if(err==1)
        {
            clrc_check_flg=true;
            result=true;
        }
        else{
            clrc_check_flg=false;

        }
    }
    return readValue[0];

}

unsigned char CLRC::CLRC_CONTROL_read_byte(uint8_t reg)
{
    unsigned char DBuffer[30];
    DBuffer[0] = reg;
    write(i2c,DBuffer,1);
    //qDebug()<<"i2cwrite";
    DBuffer[0]=0x00;
    err=read(i2c,DBuffer,1);
    //qDebug()<<"i2cread";
    readValue[0]=DBuffer[0];
    if(err==1)
    {
        clrc_check_flg=true;
        result=true;
    }
    else{
        clrc_check_flg=false;

    }
    return readValue[0];

}

//> Terminate any running command. Flush_FiFo
void CLRC::terminate_and_flush_FiFo()
{
    CLRC_write_byte( 0x00, 0x00 );
    CLRC_write_byte( 0x02, 0xb0 );
}


// Clear all IRQ 0,1 flags
void CLRC::clear_IRQ()
{
    CLRC_write_byte( 0x06, 0x7f );
    CLRC_write_byte( 0x07, 0x7f );
}


// Disable Irq 0,1 sources
void CLRC::disable_IRQ()
{
    CLRC_write_byte( 0x08, 0x00 );
    CLRC_write_byte( 0x09, 0x00 );
}

//> Wait until the command is finished. Enable IRQ sources.
void CLRC::wait_command_and_enable_IRQ()
{
    unsigned int ii;
    CLRC_write_byte( 0x08, 0x18 );         // Enable Irqs 0,1
    CLRC_write_byte( 0x09, 0x42 );         // Enable the global IRQ to be propagated to the IRQ pin
    
    while( (CLRC_read_byte( 0x07 ) & 0x40)==0){ii++;if(ii>=10000){ii=0; break;}};
}


// Read IRQ 0,1 Status register
void CLRC::read_IRQ_status()
{
    r = CLRC_read_byte( 0x06 );
    r = CLRC_read_byte( 0x07 );
}


// Start tranceive command
void CLRC::start_tranceive()
{
    CLRC_write_byte( 0x00, 0x07 );
    mysleep(100);//??????
}



void CLRC::action()
{
    if((test_modu) && (Nozzle_up_flg==0))
    {
        if(clrc_count2>=8)
        {
            CLRC_CONTROL_read_byte(0x06);
            clrc_count2=0;
        }
        clrc_count2++;
    }

    if(clrc_up_flg==true)
    {
        if(clrc_check_flg==false)
        {
            mysleep(800);
            clrc_check_flg=true;
        }
    }
    else
    {
        if(test_modu==false)
        {
            mysleep(50);
            CLRC_write_byte( 0x00, 0xFF );
        }
    }

    if(test) qDebug()<<"CLRC FLAGS"<<clrc_check_flg<<"----------"<<clrc_up_flg;
    if(clrc_check_flg && clrc_up_flg)
    {
        unsigned long ii;
        unsigned long clrc_count;
        clrc_count=5000;

        do {
            if(clrc_check_flg && clrc_up_flg)
            {
                w = 0;
                do {
                    if((test_modu) && (Nozzle_up_flg==0))
                    {
                        if(clrc_count2>=8)
                        {
                            CLRC_CONTROL_read_byte(0x06);
                            qDebug()<<"test-2"<<clrc_check_flg;
                            clrc_count2=0;
                        }
                        clrc_count2++;
                    }
                    if(clrc_check_flg && clrc_up_flg)
                    {
                        //qDebug()<<"Second Do";
                        if(Card_reader_chose==1)
                        {
                            Allow_card_read=1;
                        }

                        if((Allow_card_read==1) && (meter_com_err==false))
                        {
                            mysleep(clrc_count);
                            //RESET the device
                            CLRC_write_byte( 0x00, 0x1F );
                            mysleep(50);
                            CLRC_write_byte( 0x00, 0x00 );

                            //> =============================================
                            //> RC663 ApplyProtocolSettings:  ISO14443A=01
                            //> =============================================
                            //
                            //> Configure Timers
                            //
                            // Set Timer-0, T0Control_Reg:

                            //  qDebug()<<"Start";
                            // Starts at the end of Tx. Stops after Rx of first data. Auto-reloaded. 13.56 MHz input clock.
                            CLRC_write_byte( 0x0F, 0x98 );

                            // Set Timer-1, T1Control_Reg:
                            // Starts at the end of Tx. Stops after Rx of first data. Input clock - cascaded with Timer-0.
                            CLRC_write_byte( 0x14, 0x92 );

                            // Set Timer-2, T2Control_Reg:  Timer used for LFO trimming
                            CLRC_write_byte( 0x19, 0x20 );

                            // Set Timer-2 reload value (T2ReloadHi_Reg and T2ReloadLo_Reg)
                            CLRC_write_byte( 0x1A, 0x03 );
                            CLRC_write_byte( 0x1B, 0xFF );

                            // Set Timer-3, T3Control_Reg:
                            // Not started automatically. Not reloaded. Input clock 13.56 MHz
                            CLRC_write_byte( 0x1E, 0x00 );

                            //> Configure FIFO Size=255 and Water-level
                            // Set FifoControl_Reg, Fifo size=255 bytes. Flush FIFO
                            CLRC_write_byte( 0x02, 0x90 );

                            // Set WaterLevel =(FIFO length -1)
                            CLRC_write_byte( 0x03, 0xFE );

                            // RxBitCtrl_Reg(0x0c)    Received bit after collision are replaced with 1.
                            CLRC_write_byte( 0x0C, 0x80 );

                            // DrvMod reg(0x28), Tx2Inv=1
                            r = CLRC_read_byte( 0x28  );
                            CLRC_write_byte( 0x28, 0x80 );

                            // TxAmp_Reg(0x29)
                            CLRC_write_byte( 0x29, 0x00 );

                            // DrvCon_Reg(0x2A)
                            CLRC_write_byte( 0x2A, 0x01 );

                            // TxI_Reg(0x05),(0x05)
                            CLRC_write_byte( 0x2B, 0x05 );

                            // RxSOFD_Reg(0x34),(0x00),
                            CLRC_write_byte( 0x34, 0x00 );

                            // Rcv_Reg(0x38),(0x12)
                            CLRC_write_byte( 0x38, 0x12 );
                            //
                            //> =============================================
                            //> 2. LoadProtocol( bTxProtocol=0, bRxProtocol=0)
                            //> =============================================

                            //> Terminate any running command. Flush_FiFo
                            terminate_and_flush_FiFo();

                            // Clear all IRQ 0,1 flags
                            clear_IRQ();

                            //> Write in Fifo: Tx and Rx protocol numbers(0,0)
                            r = CLRC_read_byte( 0x04 );
                            CLRC_write_byte( 0x05, 0x00 );    // Rx protocol=0
                            CLRC_write_byte( 0x05, 0x00 );    // Tx prot=0

                            // Enable IRQ0 interrupt sources
                            //
                            // Idle interrupt(Command terminated), RC663_BIT_IDLEIRQ=0x10
                            r = CLRC_read_byte( 0x08 );
                            CLRC_write_byte( 0x08, 0x10 );

                            // Enable Global IRQ propagation.
                            r = CLRC_read_byte( 0x09 );
                            CLRC_write_byte( 0x09, 0x40 );
                            mysleep(1);
                            r = CLRC_read_byte( 0x09 );

                            //> Start RC663 command "Load Protocol"=0x0d
                            CLRC_write_byte( 0x00, 0x0D );

                            //L_LoadProtocol

                            while( (CLRC_read_byte( 0x07 ) & 0x40)==0){ii++;if(ii>=10000){ii=0; break;}};

                            // Disable Irq 0,1 sources
                            disable_IRQ();

                            //> Flush Fifo. Read Error Reg
                            CLRC_write_byte( 0x02, 0xB0 );
                            r = CLRC_read_byte( 0x0A );

                            // Apply RegisterSet
                            //
                            //> Configure CRC-16 calculation, preset value(0x6363) for Tx&Rx
                            CLRC_write_byte( 0x2C, 0x18 );
                            CLRC_write_byte( 0x2D, 0x18 );
                            CLRC_write_byte( 0x2E, 0x08  );

                            // Length of the pulse modulation in carrier clks+1
                            CLRC_write_byte( 0x2F, 0x20 );

                            // Symbol 1 and 0 burst lengths = 8 bits.
                            CLRC_write_byte( 0x30, 0x00 );

                            // Start symbol=Symbol2, Stop symbol=Symbol3
                            CLRC_write_byte( 0x33, 0xCF );

                            // Set Rx Baudrate 106 kBaud
                            CLRC_write_byte( 0x35, 0x04 );

                            // Set min-levels for Rx and phase shift
                            CLRC_write_byte( 0x37, 0x32 );
                            CLRC_write_byte( 0x39, 0x00 );

                            // Set Rx waiting time
                            CLRC_write_byte( 0x36, 0x90 );
                            CLRC_write_byte( 0x31, 0xC0 );
                            CLRC_write_byte( 0x32, 0x0B );

                            // Set Timeout. Write T0,T1 reload values(hi,Low)
                            CLRC_write_byte( 0x10, 0x08 );
                            CLRC_write_byte( 0x11, 0xD8 );
                            CLRC_write_byte( 0x15, 0x00 );
                            CLRC_write_byte( 0x16, 0x00 );

                            // Write DrvMod register
                            CLRC_write_byte( 0x28, 0x81 );

                            //> MIFARE Crypto1 state is further disabled.
                            CLRC_write_byte( 0x0B, 0x00 );

                            //>  FieldOn
                            CLRC_write_byte( 0x28, 0x89 );

                            mysleep(5);//??????

                            //> =============================================
                            //>  I14443p3a_Sw_RequestA
                            //> =============================================

                            //  TxWaitStart at the end of Rx data
                            CLRC_write_byte( 0x31, 0xC0 );

                            // Set min.time between Rx and Tx or between two Tx
                            CLRC_write_byte( 0x32, 0x0B );

                            //> Set timeout for this command cmd. Init reload values for timers-0,1
                            CLRC_write_byte( 0x10, 0x08 );
                            CLRC_write_byte( 0x11, 0x94 );
                            CLRC_write_byte( 0x15, 0x00 );
                            CLRC_write_byte( 0x16, 0x00 );
                            CLRC_write_byte( 0x06, 0x08 );
                            CLRC_write_byte( 0x36, 0x90 );
                            CLRC_write_byte( 0x2E, 0x0F );

                            //> ---------------------
                            //> Send the ReqA command
                            //> ---------------------

                            //> Terminate any running command. FlushFifo
                            terminate_and_flush_FiFo();

                            // Clear all IRQ 0,1 flags
                            clear_IRQ();

                            //> Write ReqA=26 into FIFO
                            CLRC_write_byte( 0x05, 0x26 );

                            //> Start RC663 command "Transcieve"=0x07. Activate Rx after Tx finishes.
                            CLRC_write_byte( 0x00, 0x07 );

                            //> Wait until the command is finished. Enable IRQ sources.
                            wait_command_and_enable_IRQ();

                            // Disable Irq 0,1 sources
                            disable_IRQ();

                            // Return Irq0 status
                            r = CLRC_read_byte( 0x06 );

                            //> Wait until reception
                            CLRC_write_byte( 0x08, 0x54 );
                            CLRC_write_byte( 0x09, 0x42 );
                            mysleep(10);

                            while( (CLRC_read_byte( 0x07 ) & 0x40)==0){ii++;if(ii>=10000){ii=0; break;}};

                            while (w == 0){
                                if(clrc_debug_flg==1){ if(test)qDebug()<<"\nNO CARD DETECTED...\n";}
                                if(clrc_debug_flg==1){ if(test)qDebug()<<"WAITING FOR A CARD...\n";}
                                w++;
                            }

                        }
                    }
                    else
                    {
                        if(test_modu==false)
                        {
                            mysleep(50);
                            CLRC_write_byte( 0x00, 0xFF );
                        }
                    }
                  if(clrc_check_flg==false) break;
                } while ( CLRC_read_byte( 0x40 == 0 ) );

                CLRC_write_byte( 0x08, 0x00 );
                CLRC_write_byte( 0x09, 0x00 );

                r = CLRC_read_byte( 0x05 );
                r = CLRC_read_byte( 0x05 );

                // Read the error register
                r = CLRC_read_byte( 0x0a );

                // Reset TxLastBits_Reg
                CLRC_write_byte( 0x2E, 0x08 );

                //> ------------------------------
                //> Get UID cascade level-1
                //> ------------------------------
                CLRC_write_byte( 0x2E, 0x08 );
                CLRC_write_byte( 0x0C, 0x00 );

                // Terminate any running command, FlushFifo
                terminate_and_flush_FiFo();

                // Clear all IRQ 0,1 flags
                clear_IRQ();

                //> Write "Antcollision CL 1" cmd into FIFO (SEL=93, NVB=20)
                CLRC_write_byte( 0x05, 0x93 );
                CLRC_write_byte( 0x05, 0x20 );

                // Start tranceive command
                start_tranceive();

                // Wait until the command is finished
                wait_command_and_enable_IRQ();

                // Disable IRQ0 interrupt sources
                disable_IRQ();

                // Read IRQ 0,1 Status register
                read_IRQ_status();

                //> Read FIFO, Expected - Complete UID (one PICC in HF)
                r = CLRC_read_byte( 0x04 );            // FIFO length

                // Cascade Tag (UID0)
                UID0 = CLRC_read_byte( 0x05 );

                UID1 = CLRC_read_byte( 0x05 );

                UID2 = CLRC_read_byte( 0x05 );

                UID3 = CLRC_read_byte( 0x05 );

                UID4 = CLRC_read_byte( 0x05 );


                temp.append(UID0);
                temp.append(UID1);
                temp.append(UID2);
                temp.append(UID3);
                temp.append(UID4);


                if(LCD_status==1)
                {
                    if((temp.toHex()[0]=='2') && (temp.toHex()[1]=='0')&& (temp.toHex()[2]=='2') && (temp.toHex()[3]=='0')&& (temp.toHex()[4]=='2'))
                    {
                        CLRC_ID_Count=0;
                    }
                    else
                    {

                        if(Temp_CustomerID!=temp.toHex())
                        {
                            if(previous_CustomerID==temp.toHex())
                            {
                                CLRC_ID_Count++;
                            }
                            else
                            {
                                CLRC_ID_Count=0;
                            }
                            QString control=temp.toHex();
                            control=control.left(8);
                            control=control.toUpper();
                            if((CLRC_ID_Count>=2) && (ID_count<3) && (control!="00000000"))
                            {
                                buzzer_on_flg=1;
                                TEXT_fromMaster=idTaken_ByteArray;
                                LCD_refresh_flg=1;
                                CustomerID=temp.toHex();
                                CustomerID=CustomerID.left(8);
                                CustomerID=CustomerID.toUpper();
                                Temp_CustomerID=temp.toHex();
                                previous_CustomerID.clear();
                                CLRC_ID_Count=0;
                                //only_card_read_flg=true;
                                StoredScreenRFID_fromMaster=0;
                                Temp_ID[ID_count]=CustomerID; ID_count++;
                                Card_read_flg=1;
                                RFID_Read_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                                RFID_Read_Time.prepend("20");
                                RFID_Result_Time=RFID_Read_Time;
                                RFID_Read_Time=RFID_Read_Time.replace(RFID_Read_Time.lastIndexOf(":")+1,2,"00");
                                if(test)qDebug()<<"SATIS_DEBUG CARDID -->"<<CustomerID<<ID_count;
                                if(test)qDebug()<<"SATIS_DEBUG CARDID"<<CustomerID<<ID_count;
                                settings.setlog("SATIS_DEBUG CARDID : "+CustomerID);
                             }
                        }
                        previous_CustomerID=temp.toHex();
                    }
                }


                temp.clear();
                w = 1;

                //> Read Error register
                r = CLRC_read_byte( 0x0A );
                //> ------------------------------
                //> Select cascade level-1
                //> ------------------------------
                // Terminate any running command, FlushFifo
                terminate_and_flush_FiFo();

                CLRC_write_byte( 0x2C, 0x19 );          // Enable RX and TX CRC
                CLRC_write_byte( 0x2D, 0x19 );
                CLRC_write_byte( 0x0C, 0x00 );


                // Clear all IRQ 0,1 flags
                clear_IRQ();

                //> Write "Select CL 1" cmd into FIFO (SEL=93, NVB=70)
                CLRC_write_byte( 0x05, 0x93 );
                CLRC_write_byte( 0x05, 0x70 );
                CLRC_write_byte( 0x05, UID0 );          // UID bytes ...
                CLRC_write_byte( 0x05, UID1 );
                CLRC_write_byte( 0x05, UID2 );
                CLRC_write_byte( 0x05, UID3 );
                CLRC_write_byte( 0x05, UID4 );

                // Start tranceive command
                start_tranceive();

                // Wait until the command is finished
                wait_command_and_enable_IRQ();

                // Disable IRQ0 interrupt sources
                disable_IRQ();

                // Read IRQ 0,1 Status register
                read_IRQ_status();

                //> Read FIFO, Expected - Complete UID (one PICC in HF)
                r = CLRC_read_byte( 0x04 );             // FIFO length

            }
            else
            {
                if(test_modu==false)
                {
                    mysleep(50);
                    CLRC_write_byte( 0x00, 0xFF );
                }
            }
            if(clrc_check_flg==false) break;
        } while( (CLRC_read_byte( 0x05 ) & 0x04) == 0 );

        //qDebug()<<"CARD DETECTION FINISHED\n";

        //    //----------------------------------------------------------------------------------------
        //    //----------------------------------------------------------------------------------------
        //    //> ------------------------------
        //    //> Get UID cascade level-2
        //    //> ------------------------------
        //       CLRC_write_byte( 0x2E, 0x08 );
        //       CLRC_write_byte( 0x0C, 0x00 );
        //       CLRC_write_byte( 0x2C, 0x18 );       // TX, RX CRC off
        //       CLRC_write_byte( 0x2D, 0x18 );

        //    // Terminate any running command, FlushFifo
        //       terminate_and_flush_FiFo();

        //    // Clear all IRQ 0,1 flags
        //       clear_IRQ();

        //    //> Write "Antcollision CL 2" cmd into FIFO (SEL=95, NVB=20)
        //       CLRC_write_byte( 0x05, 0x95 );
        //       CLRC_write_byte( 0x05, 0x20 );

        //    // Start tranceive command
        //       start_tranceive();

        //    // Wait until the command is finished
        //       wait_command_and_enable_IRQ();

        //    // Disable IRQ0 interrupt sources
        //       disable_IRQ();

        //    // Read IRQ 0,1 Status register
        //       read_IRQ_status();

        //    //> Read FIFO
        //       r = CLRC_read_byte( 0x04 );             // FIFO length

        //       UID0 = CLRC_read_byte( 0x05 );
        //       mysleep(0.1);
        //       UID1 = CLRC_read_byte( 0x05 );
        //       mysleep(0.1);
        //       UID2 =CLRC_read_byte( 0x05 );
        //       mysleep(0.1);
        //       UID3 =CLRC_read_byte( 0x05 );
        //       mysleep(0.1);
        //       UID4 =CLRC_read_byte( 0x05 );

        //       temp.append(UID0);
        //       temp.append(UID1);
        //       temp.append(UID2);
        //       temp.append(UID3);
        //       temp.append(UID4);


        ////       if((CustomerID[0]=='2') && (CustomerID[1]=='0')&& (CustomerID[2]=='2') && (CustomerID[3]=='0')&& (CustomerID[4]=='2'))
        ////       {
        ////           qDebug()<<"card_read_1";
        ////           Card_read_flg=0;

        ////       }
        //    //> Read Error register
        //       r = CLRC_read_byte( 0x0A );

        //    //> ------------------------------
        //    //> Select cascade level-2
        //    //> ------------------------------
        //    // Terminate any running command, FlushFifo
        //       terminate_and_flush_FiFo();

        //       CLRC_write_byte( 0x2C, 0x19 );          // Enable RX and TX CRC
        //       CLRC_write_byte( 0x2D, 0x19 );
        //       CLRC_write_byte( 0x0C, 0x00 );

        //    // Clear all IRQ 0,1 flags
        //       clear_IRQ();

        //    //> Write "Select CL 2" cmd into FIFO (SEL=95, NVB=70)
        //       CLRC_write_byte( 0x05, 0x95 );
        //       CLRC_write_byte( 0x05, 0x70 );
        //       CLRC_write_byte( 0x05, UID0 );          // UID bytes ...
        //       CLRC_write_byte( 0x05, UID1 );
        //       CLRC_write_byte( 0x05, UID2 );
        //       CLRC_write_byte( 0x05, UID3 );
        //       CLRC_write_byte( 0x05, UID4 );

        //    // Start tranceive command
        //       start_tranceive();

        //    // Wait until the command is finished
        //       wait_command_and_enable_IRQ();

        //    // Disable IRQ0 interrupt sources
        //       disable_IRQ();

        //    // Read IRQ 0,1 Status register
        //       read_IRQ_status();

        //    //> Read FIFO
        //       r = CLRC_read_byte( 0x04 );              // FIFO length

        //       r = CLRC_read_byte( 0x05 );              // Response

        //       w=0;

    }
}

