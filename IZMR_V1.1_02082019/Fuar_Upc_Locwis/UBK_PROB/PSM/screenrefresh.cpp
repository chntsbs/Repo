#include "screenrefresh.h"
#include <QThread>
#include <QTimer>
#include"customerscreen.h"
screenrefresh::screenrefresh(QObject *parent) :
    QObject(parent)
{

    LCDtimer1=new QTimer(this);
    LCDtimer2=new QTimer(this);

}

void screenrefresh::LCDtoThread(QThread &LCDThread)
{
      connect(LCDtimer1,SIGNAL(timeout()),this,SLOT(LCD_refresh()));
      connect(&LCDThread,SIGNAL(started()),this,SLOT(LCD_scan_timer()));

}


void screenrefresh::LCD_scan_timer()
{
    LCDtimer1->start(100);
}

void screenrefresh::LCD_refresh()
{
   qDebug()<<"LCD refresh"


}

