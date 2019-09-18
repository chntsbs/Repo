
#ifndef CUSTOMERSCREEN_H
#define CUSTOMERSCREEN_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <QCoreApplication>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QDebug>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <QTimer>
#include <QVector>
#include <QThread>

class CustomerScreen : public QThread
{
    Q_OBJECT
public:
    CustomerScreen();

signals:
    void old_screen_lines(QString,QString);
    void changeFrame(int);
    void test_signal();

public slots:
    void LCD_refresh();
    void LCD_refresh_2();
    void LCD_scan_timer();

    void test_slot();

};
extern CustomerScreen *cusscr;
#endif // CUSTOMERSCREEN_H
