#ifndef MAIN_ROUTINE_H
#define MAIN_ROUTINE_H

#include <QObject>
#include <QObject>
#include <QDebug>
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
#include"scproutine.h"


class main_routine : public QObject
{
    Q_OBJECT
public:
    explicit main_routine(QObject *parent = 0);
    QTimer *maintimer1;
    QTimer *maintimer2;
    void maintoThread(QThread &mainThread);
    Database *database;
    void convert_check_RFID();
    void mainbuzzerOn();

signals:

public slots:

    void main_func();
    void scan_timer();
    void clock();
    void mainbuzzerOff();


};

#endif // MAIN_ROUTINE_H
