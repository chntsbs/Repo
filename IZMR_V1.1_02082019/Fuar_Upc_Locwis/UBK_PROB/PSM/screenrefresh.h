#ifndef SCREENREFRESH_H
#define SCREENREFRESH_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QThread>
#include"customerscreen.h"

class screenrefresh : public QObject
{
    Q_OBJECT
public:
    explicit screenrefresh(QObject *parent = 0);

    QTimer *LCDtimer1;
    QTimer *LCDtimer2;
    void screenrefresh::LCDtoThread(QThread &LCDThread);


signals:

public slots:

  void LCD_refresh();
  void LCD_scan_timer();

};

#endif // SCREENREFRESH_H
