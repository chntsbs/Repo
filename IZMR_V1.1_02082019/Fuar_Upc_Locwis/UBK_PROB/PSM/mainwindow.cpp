#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "keyboard.h"
#include "ui_keyboard.h"
#include <QDesktopWidget>
#include "customerscreen.h"
#include "QSignalMapper"
#include <global.h>
#include <QSize>
#include <QSizePolicy>
#include"syslog.h"
#include "global.h"
#include"database.h"
#include"QToolButton"
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QWindow>
#include <QMessageBox>
#include "config.h"
#include <QTouchEvent>
#include <QMouseEvent>
#include <QFileSystemWatcher>
#include "savetofile.h"
#include "scproutine.h"


QString Numpad_Output;
void saveUnitPrice(quint8 NZL);
QByteArray convert_format(QByteArray data,unsigned char dot_ind);
int CountId(QString tablo);
int dec_to_bcd(int dec);
QString getMacAddress();
bool menu_passwrd=false;
QIcon iconcancel;
QIcon icon9;
//SaveFile settings;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    system("echo 141 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio141/direction");
    locktimer=175 ;
    MWdatabase=new Database();
    Mwindowtimer=new QTimer(this);
    connect(Mwindowtimer,SIGNAL(timeout()),this,SLOT(Mwindowaction()));
    screen=QApplication::desktop()->screenGeometry();
    screen_width=800;
    screen_heigth=490;
    this->setGeometry(0,0,screen_width,screen_heigth);
    connect(cusscr,SIGNAL(old_screen_lines(QString,QString)),this,SLOT(screen_lines(QString,QString)));
    connect(cusscr,SIGNAL(changeFrame(int)),this,SLOT(frameChange(int)));

    QTimer *tmr=new QTimer(this);
    connect(tmr, SIGNAL(timeout()), this, SLOT(show_time()));
    tmr->start(1000);

    QTimer *CLRCtimer2=new QTimer(this);
    connect(CLRCtimer2,SIGNAL(timeout()),this,SLOT(clrcBuzzerON()));
    CLRCtimer2->start(1000);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    top_frame = new QFrame(this);
    top_frame->setObjectName(QStringLiteral("top_frame"));
    top_frame->setGeometry(QRect(0, 0, 800, 80));
    top_frame->setStyleSheet(QLatin1String("background-color: rgb(0, 0, 0);\n"
                                           "border:1px solid;\n"
                                           "border-color: rgb(255, 255, 255);\n"
                                           ""));
    top_frame->setFrameShape(QFrame::StyledPanel);
    top_frame->setFrameShadow(QFrame::Raised);

    saat_lineEdit = new QLineEdit(top_frame);
    saat_lineEdit->setObjectName(QStringLiteral("saat_lineEdit"));
    saat_lineEdit->setGeometry(QRect(655, 0, 143, 50));
    QFont font;
    font.setPointSize(23);
    saat_lineEdit->setFont(font);
    saat_lineEdit->setStyleSheet(QLatin1String("border-bottom:0px solid;\n"
                                               "color: rgb(255, 255, 255);\n"
                                               "border-right:0px solid;\n"
                                               ""));
    saat_lineEdit->setAlignment(Qt::AlignCenter);
    saat_lineEdit->setReadOnly(true);
    tarih_lineEdit = new QLineEdit(top_frame);
    tarih_lineEdit->setObjectName(QStringLiteral("tarih_lineEdit"));
    tarih_lineEdit->setGeometry(QRect(655, 50, 143, 30));
    QFont font1;
    font1.setPointSize(15);
    tarih_lineEdit->setFont(font1);
    tarih_lineEdit->setStyleSheet(QLatin1String("border-top:0px solid;\n"
                                                "border-right:0px solid;\n"
                                                "color: rgb(255, 255, 255);"));
    tarih_lineEdit->setAlignment(Qt::AlignCenter);
    tarih_lineEdit->setReadOnly(true);
    info_top_frame = new QFrame(top_frame);
    info_top_frame->setObjectName(QStringLiteral("info_top_frame"));
    info_top_frame->setGeometry(QRect(10, 10, 641, 61));
    info_top_frame->setStyleSheet(QStringLiteral("border:0px solid;"));
    info_top_frame->setFrameShape(QFrame::StyledPanel);
    info_top_frame->setFrameShadow(QFrame::Raised);


    //    mepsan_logo = new QPushButton(info_top_frame);
    //    mepsan_logo->setObjectName(QStringLiteral("mepsan_logo"));
    //    mepsan_logo->setGeometry(QRect(10, 0, 200, 61));
    //    mepsan_logo->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
    //                                             "background-color: rgb(0, 0, 0);\n"
    //                                             "border:0px solid #FFFFFF;\n"
    //                                             "border-radius:10px;"));
    //    QIcon icon15;
    //    icon15.addFile(QStringLiteral(":/new/prefix1/icons/logo6.png"), QSize(), QIcon::Normal, QIcon::Off);
    //    mepsan_logo->setIcon(icon15);
    //    mepsan_logo->setIconSize(QSize(200, 61));
    //    mepsan_logo->setFocusPolicy(Qt::NoFocus);

    net_logo = new QPushButton(info_top_frame);
    net_logo->setObjectName(QStringLiteral("net_logo"));
    net_logo->setGeometry(QRect(50, 0, 150, 70));
    net_logo->setStyleSheet(QLatin1String("background-color: rgb(0, 0, 0,0);\n"
                                          ));
    net_logo->setVisible(true);
    net_logo->setFocusPolicy(Qt::NoFocus);


    connect(net_logo,SIGNAL(released()),this,SLOT(locwiz_Clicked()));


    locwiz_logo = new QLabel(info_top_frame);

    locwiz_logo->setObjectName(QStringLiteral("locwiz_logo"));
    locwiz_logo->setGeometry(QRect(50, 0, 150, 70));
    locwiz_logo->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:0px solid #FFFFFF;\n"
                                             "border-radius:10px;"));
    locwiz_logo->setVisible(true);
    locwiz_logo->setFocusPolicy(Qt::NoFocus);


    locwiz_logo_movie=new QMovie(":/new/prefix1/icons/Locwiz-02.gif");
    size1=new QSize();
    size1->setHeight(70);
    size1->setWidth(150);
    locwiz_logo_movie->setScaledSize(*size1);
    locwiz_logo->setMovie(locwiz_logo_movie);
    locwiz_logo_movie->start();

    net_logo->raise();

    top_button = new QPushButton(info_top_frame);
    top_button->setObjectName(QStringLiteral("top_button"));
    top_button->setGeometry(QRect(300, 10, 200, 40));
    QFont font2;
    font2.setPointSize(20);
    font2.setItalic(true);
    top_button->setFont(font2);
    top_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                           "background-color: rgb(142, 142, 142);\n"
                                           "border:1px solid #FFFFFF;\n"
                                           "border-radius:10px;"));
    top_button->setVisible(true);
    top_button->setFocusPolicy(Qt::NoFocus);
    //top_button->setAlignment(Qt::AlignCenter);

    connect(top_button,SIGNAL(released()),this,SLOT(net_logo_clicked()));

    main_frame = new QFrame(this);
    main_frame->setObjectName(QStringLiteral("main_frame"));
    main_frame->setGeometry(QRect(0, 80, 800, 400));
    main_frame->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                            "border:1px solid;\n"
                                            "border-top:2px solid;\n"
                                            "border-color: rgb(255, 255, 255);"));
    main_frame->setFrameShape(QFrame::StyledPanel);
    main_frame->setFrameShadow(QFrame::Raised);
    info_frame = new QFrame(main_frame);
    info_frame->setObjectName(QStringLiteral("info_frame"));
    info_frame->setGeometry(QRect(5, 10, 785, 301));
    info_frame->setStyleSheet(QLatin1String("border-radius:10px;\n"
                                            "background-color: rgb(0, 0, 0);"));
    info_frame->setFrameShape(QFrame::StyledPanel);
    info_frame->setFrameShadow(QFrame::Raised);
    device_icon_button = new QPushButton(info_frame);
    device_icon_button->setObjectName(QStringLiteral("device_icon_button"));
    device_icon_button->setGeometry(QRect(5, 5, 260, 220));
    device_icon_button->setStyleSheet(QStringLiteral("border:0px solid;"));
    device_icon_button->setIcon(QIcon(":/new/prefix1/icons/logo.png"));
    device_icon_button->setIconSize(QSize(250, 150));
    device_icon_button->setFocusPolicy(Qt::NoFocus);
    status_label = new QLabel(info_frame);
    status_label->setObjectName(QStringLiteral("status_label"));
    status_label->setGeometry(QRect(280, 20, 491, 185));
    QFont font3;
    font3.setPointSize(30);
    font3.setItalic(true);
    status_label->setFont(font3);
    status_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                              "border-color: rgb(255, 255, 255);\n"
                                              "color: rgb(255, 255, 255);"));
    status_label->setAlignment(Qt::AlignCenter);

    warning_label = new QLabel(info_frame);
    warning_label->setObjectName(QStringLiteral("warning label"));
    warning_label->setGeometry(QRect(90, 215, 681, 75));
    QFont font31;
    font31.setPointSize(22);
    font31.setBold(true);
    font31.setItalic(true);
    warning_label->setFont(font31);
    warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                               "border-color: rgb(255, 255, 255);\n"
                                               "color: rgb(0, 0, 0); background-color: rgb(142, 142, 142)"));
    warning_label->setAlignment(Qt::AlignCenter);




    records_button = new QPushButton(main_frame);
    records_button->setObjectName(QStringLiteral("records_button"));
    records_button->setGeometry(QRect(5, 320, 251, 71));
    QFont font4;
    font4.setPointSize(25);
    font4.setItalic(true);
    records_button->setFont(font4);
    records_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                "background-color: rgb(0, 0, 0);\n"
                                                "border:1px solid #FFFFFF;\n"
                                                "border-radius:10px;"));
    QIcon icon3;
    icon3.addFile(QStringLiteral(":/new/prefix1/icons/search"), QSize(), QIcon::Normal, QIcon::Off);
    records_button->setIcon(icon3);
    records_button->setIconSize(QSize(50, 50));
    connect (records_button, SIGNAL( released() ), this, SLOT( Enter_Record_Clicked() ) );


    enter_plate_button = new QPushButton(main_frame);
    enter_plate_button->setObjectName(QStringLiteral("enter_plate_button"));
    enter_plate_button->setGeometry(QRect(268, 320, 261, 71));
    QFont font5;
    font5.setPointSize(18);
    font5.setItalic(true);
    enter_plate_button->setFont(font5);
    enter_plate_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                    "background-color: rgb(0, 0, 0);\n"
                                                    "border:1px solid #FFFFFF;\n"
                                                    "border-radius:10px;"));
    QIcon icon4;
    icon4.addFile(QStringLiteral(":/new/prefix1/icons/arrow"), QSize(), QIcon::Normal, QIcon::Off);
    enter_plate_button->setIcon(icon4);
    enter_plate_button->setIconSize(QSize(50, 50));
    connect (enter_plate_button, SIGNAL( released() ), this, SLOT( Enter_Plate_Clicked() ) );

    menu_button = new QPushButton(main_frame);
    menu_button->setObjectName(QStringLiteral("menu_button"));
    menu_button->setGeometry(QRect(540, 320, 251, 71));
    menu_button->setFont(font4);
    menu_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:1px solid #FFFFFF;\n"
                                             "border-radius:10px;"));
    QIcon icon5;
    icon5.addFile(QStringLiteral(":/new/prefix1/icons/menu"), QSize(), QIcon::Normal, QIcon::Off);
    menu_button->setIcon(icon5);
    menu_button->setIconSize(QSize(50, 50));



    warning_icon_button = new QToolButton(info_frame);
    warning_icon_button->setObjectName(QStringLiteral("warning_icon_button"));
    warning_icon_button->setGeometry(QRect(17, 210, 61, 80));
    warning_icon_button->setStyleSheet(QLatin1String("background-color: rgb(0, 0, 0, 0);\n"
                                                     "border:0px;"));
    QIcon icon7;
    icon7.addFile(QStringLiteral(":/new/prefix1/icons/warning (1).png"), QSize(), QIcon::Normal, QIcon::Off);
    warning_icon_button->setIcon(icon7);
    warning_icon_button->setIconSize(QSize(100, 100));
    warning_icon_button->setVisible(false);

    keyboard_frame = new QFrame(this);
    keyboard_frame->setObjectName(QStringLiteral("keyboard_frame"));
    keyboard_frame->setGeometry(QRect(0, 125, 800, 355));
    keyboard_frame->setStyleSheet(QLatin1String("border:0px solid;\n"
                                                "background-color: rgb(24, 24, 24);"));
    keyboard_frame->setFrameShape(QFrame::StyledPanel);
    keyboard_frame->setFrameShadow(QFrame::Raised);

    plate_frame = new QFrame(this);
    plate_frame->setObjectName(QStringLiteral("plate_frame"));
    plate_frame->setGeometry(QRect(0, 0, 800, 480));
    plate_frame->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                             "border:1px solid;\n"
                                             "border-top:2px solid;\n"
                                             "border-color: rgb(255, 255, 255);"));
    plate_frame->setFrameShape(QFrame::StyledPanel);
    plate_frame->setFrameShadow(QFrame::Raised);
    plate_frame->setVisible(false);
    plateenter_frame = new QFrame(plate_frame);
    plateenter_frame->setObjectName(QStringLiteral("plateenter_frame"));
    plateenter_frame->setGeometry(QRect(0, 0, 800, 130));
    plateenter_frame->setStyleSheet(QStringLiteral("border:0px;"));
    plateenter_frame->setFrameShape(QFrame::StyledPanel);
    plateenter_frame->setFrameShadow(QFrame::Raised);

    plateLine = new QLineEdit(plateenter_frame);
    plateLine->setObjectName(QStringLiteral("plateLine"));
    plateLine->setGeometry(QRect(8, 45, 781, 70));
    QFont font7;
    font7.setPointSize(45);
    font7.setBold(true);
    font7.setWeight(75);
    plateLine->setFont(font7);
    plateLine->setStyleSheet(QLatin1String("color: rgb(24, 24, 24);\n"
                                           "border:1px solid;\n"
                                           "border-radius:10px;\n"
                                           "border-color: rgb(24, 24, 24);"));
    plateLine->setInputMethodHints(Qt::ImhUppercaseOnly);
    plateLine->setAlignment(Qt::AlignCenter);
    //plateLine->setClearButtonEnabled(true);

    plate_label = new QLabel(plateenter_frame);
    plate_label->setObjectName(QStringLiteral("plate_label"));
    plate_label->setGeometry(QRect(300, 10, 210, 25));
    QFont font8;
    font8.setPointSize(15);
    font8.setBold(false);
    font8.setWeight(50);
    plate_label->setFont(font8);
    plate_label->setAlignment(Qt::AlignCenter);
    plate_label->setStyleSheet(QLatin1String("color: rgb(24, 24, 24);\n"
                                             "border:1px solid;\n"
                                             "border-radius:10px;\n"
                                             "border-color: rgb(24, 24, 24);"));
    plate_label->setText(enterPlate_String);

    km_frame = new QFrame(this);
    km_frame->setObjectName(QStringLiteral("km_frame"));
    km_frame->setGeometry(QRect(0, 0, 800, 480));
    km_frame->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                          "border:1px solid;\n"
                                          "border-top:2px solid;\n"
                                          "border-color: rgb(255, 255, 255);"));
    km_frame->setFrameShape(QFrame::StyledPanel);
    km_frame->setFrameShadow(QFrame::Raised);
    km_frame->setVisible(false);

    kmenter_frame = new QFrame(km_frame);
    kmenter_frame->setObjectName(QStringLiteral("kmenter_frame"));
    kmenter_frame->setGeometry(QRect(0, 0, 800, 150));
    kmenter_frame->setStyleSheet(QStringLiteral("border:0px;"));
    kmenter_frame->setFrameShape(QFrame::StyledPanel);
    kmenter_frame->setFrameShadow(QFrame::Raised);
    kmLine = new QLineEdit(kmenter_frame);
    kmLine->setObjectName(QStringLiteral("kmLine"));
    kmLine->setGeometry(QRect(10, 50, 781, 91));
    kmLine->setFont(font7);
    kmLine->setStyleSheet(QLatin1String("color: rgb(24, 24, 24);\n"
                                        "border:1px solid;\n"
                                        "border-radius:10px;\n"
                                        "border-color: rgb(24, 24, 24);"));
    kmLine->setInputMethodHints(Qt::ImhUppercaseOnly);
    kmLine->setAlignment(Qt::AlignCenter);
    //kmLine->setClearButtonEnabled(true);
    km_label = new QLabel(kmenter_frame);
    km_label->setObjectName(QStringLiteral("km_label"));
    km_label->setGeometry(QRect(267, 10, 276, 30));
    km_label->setAlignment(Qt::AlignCenter);
    km_label->setStyleSheet(QLatin1String("color: rgb(24, 24, 24);\n"
                                          "border:1px solid;\n"
                                          "border-radius:10px;\n"
                                          "border-color: rgb(24, 24, 24);"));
    km_label->setFont(font8);

    key=new Keyboard(this->keyboard_frame);
    key->setGeometry(QRect(QPoint(0,0),QSize(800,355)));
    // key->close();
    keyboard_frame->setVisible(false);

    numpadframe = new QFrame(this);
    numpadframe->setObjectName(QStringLiteral("numpadframe"));
    numpadframe->setGeometry(QRect(0, 145, 801, 331));
    numpadframe->setFrameShape(QFrame::NoFrame);
    numpadframe->setFrameShadow(QFrame::Plain);
    numpadframe->raise();
    gridLayout_2 = new QGridLayout(numpadframe);
    gridLayout_2->setSpacing(6);
    gridLayout_2->setContentsMargins(11, 11, 11, 11);
    gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    numpad_8 = new QPushButton(numpadframe);
    numpad_8->setObjectName(QStringLiteral("numpad_8"));
    sizePolicy.setHeightForWidth(numpad_8->sizePolicy().hasHeightForWidth());
    numpad_8->setSizePolicy(sizePolicy);
    QFont font18;
    font18.setPointSize(20);
    font18.setBold(true);
    font18.setWeight(75);
    numpad_8->setFont(font18);
    numpad_8->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_8, 2, 1, 1, 1);

    numpad_1 = new QPushButton(numpadframe);
    numpad_1->setObjectName(QStringLiteral("numpad_1"));
    sizePolicy.setHeightForWidth(numpad_1->sizePolicy().hasHeightForWidth());
    numpad_1->setSizePolicy(sizePolicy);
    numpad_1->setFont(font18);
    numpad_1->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_1, 0, 0, 1, 1);

    numpad_5 = new QPushButton(numpadframe);
    numpad_5->setObjectName(QStringLiteral("numpad_5"));
    sizePolicy.setHeightForWidth(numpad_5->sizePolicy().hasHeightForWidth());
    numpad_5->setSizePolicy(sizePolicy);
    numpad_5->setFont(font18);
    numpad_5->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_5, 1, 1, 1, 1);

    numpad_9 = new QPushButton(numpadframe);
    numpad_9->setObjectName(QStringLiteral("numpad_9"));
    sizePolicy.setHeightForWidth(numpad_9->sizePolicy().hasHeightForWidth());
    numpad_9->setSizePolicy(sizePolicy);
    numpad_9->setFont(font18);
    numpad_9->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_9, 2, 2, 1, 1);

    numpad_3 = new QPushButton(numpadframe);
    numpad_3->setObjectName(QStringLiteral("numpad_3"));
    sizePolicy.setHeightForWidth(numpad_3->sizePolicy().hasHeightForWidth());
    numpad_3->setSizePolicy(sizePolicy);
    numpad_3->setFont(font18);
    numpad_3->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_3, 0, 2, 1, 1);

    numpad_6 = new QPushButton(numpadframe);
    numpad_6->setObjectName(QStringLiteral("numpad_6"));
    sizePolicy.setHeightForWidth(numpad_6->sizePolicy().hasHeightForWidth());
    numpad_6->setSizePolicy(sizePolicy);
    numpad_6->setFont(font18);
    numpad_6->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_6, 1, 2, 1, 1);

    numpad_7 = new QPushButton(numpadframe);
    numpad_7->setObjectName(QStringLiteral("numpad_7"));
    sizePolicy.setHeightForWidth(numpad_7->sizePolicy().hasHeightForWidth());
    numpad_7->setSizePolicy(sizePolicy);
    numpad_7->setFont(font18);
    numpad_7->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_7, 2, 0, 1, 1);

    numpad_4 = new QPushButton(numpadframe);
    numpad_4->setObjectName(QStringLiteral("numpad_4"));
    sizePolicy.setHeightForWidth(numpad_4->sizePolicy().hasHeightForWidth());
    numpad_4->setSizePolicy(sizePolicy);
    numpad_4->setFont(font18);
    numpad_4->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_4, 1, 0, 1, 1);

    numpad_2 = new QPushButton(numpadframe);
    numpad_2->setObjectName(QStringLiteral("numpad_2"));
    sizePolicy.setHeightForWidth(numpad_2->sizePolicy().hasHeightForWidth());
    numpad_2->setSizePolicy(sizePolicy);
    numpad_2->setFont(font18);
    numpad_2->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_2, 0, 1, 1, 1);

    numpad_clear = new QPushButton(numpadframe);
    numpad_clear->setObjectName(QStringLiteral("numpad_clear"));
    sizePolicy.setHeightForWidth(numpad_clear->sizePolicy().hasHeightForWidth());
    numpad_clear->setSizePolicy(sizePolicy);
    numpad_clear->setFont(font18);
    numpad_clear->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n"
                                              "color: rgb(255, 255, 255);\n"
                                              "border:2px solid #FFFFFF;\n"
                                              "border-radius:10px;"));

    gridLayout->addWidget(numpad_clear, 3, 0, 1, 1);

    numpad_0 = new QPushButton(numpadframe);
    numpad_0->setObjectName(QStringLiteral("numpad_0"));
    sizePolicy.setHeightForWidth(numpad_0->sizePolicy().hasHeightForWidth());
    numpad_0->setSizePolicy(sizePolicy);
    numpad_0->setFont(font18);
    numpad_0->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                          "background-color: rgb(0, 0, 0);\n"
                                          "border:1px solid #FFFFFF;\n"
                                          "border-radius:10px;"));

    gridLayout->addWidget(numpad_0, 3, 1, 1, 1);

    numpad_point = new QPushButton(numpadframe);
    numpad_point->setObjectName(QStringLiteral("numpad_point"));
    sizePolicy.setHeightForWidth(numpad_point->sizePolicy().hasHeightForWidth());
    numpad_point->setSizePolicy(sizePolicy);
    numpad_point->setFont(font18);
    numpad_point->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                              "background-color: rgb(0, 0, 0);\n"
                                              "border:1px solid #FFFFFF;\n"
                                              "border-radius:10px;"));
    kmWarningButton=new QToolButton(kmenter_frame);
    kmWarningButton->setObjectName(QStringLiteral("warning_icon_button"));
    kmWarningButton->setGeometry(QRect(13,55,61, 80));
    kmWarningButton->setStyleSheet(QLatin1String("background-color: rgb(0, 0, 0, 0);\n"
                                                     "border:0px;"));
    QIcon icon98;
    icon98.addFile(QStringLiteral(":/new/prefix1/icons/warning (1).png"), QSize(), QIcon::Normal, QIcon::Off);
    kmWarningButton->setIcon(icon98);
    kmWarningButton->setIconSize(QSize(100, 100));
    kmWarningButton->setVisible(false);

    QFont font6;
    font6.setPointSize(20);
    font6.setBold(true);
    font6.setWeight(75);

    gridLayout->addWidget(numpad_point, 3, 2, 1, 1);
    gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

    gridLayoutnumpad = new QGridLayout();
    gridLayoutnumpad->setSpacing(6);
    gridLayoutnumpad->setObjectName(QStringLiteral("gridLayoutnumpad"));
    numpad_cancel = new QPushButton(numpadframe);
    numpad_cancel->setObjectName(QStringLiteral("numpad_cancel"));
    QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(numpad_cancel->sizePolicy().hasHeightForWidth());
    numpad_cancel->setSizePolicy(sizePolicy1);
    numpad_cancel->setMaximumSize(QSize(400, 62));
    numpad_cancel->setSizeIncrement(QSize(0, 0));
    numpad_cancel->setFont(font6);
    numpad_cancel->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n"
                                               "color: rgb(255, 255, 255);\n"
                                               "border:2px solid #FFFFFF;\n"
                                               "border-radius:10px;"));
    numpad_cancel->setIcon(QIcon(":/new/prefix1/icons/forward.png"));
    numpad_cancel->setIconSize(QSize(50, 50));

    gridLayoutnumpad->addWidget(numpad_cancel, 0, 0, 1, 1);

    numpad_enter = new QPushButton(numpadframe);
    numpad_enter->setObjectName(QStringLiteral("numpad_enter"));
    sizePolicy1.setHeightForWidth(numpad_enter->sizePolicy().hasHeightForWidth());
    numpad_enter->setSizePolicy(sizePolicy1);
    numpad_enter->setMaximumSize(QSize(400, 63));
    numpad_enter->setSizeIncrement(QSize(0, 0));
    numpad_enter->setFont(font6);
    numpad_enter->setStyleSheet(QLatin1String("background-color: rgb(47, 137, 53);\n"
                                              "color: rgb(255, 255, 255);\n"
                                              "border:2px solid #FFFFFF;\n"
                                              "border-radius:10px;"));
    numpad_enter->setIcon(QIcon(":/new/prefix1/icons/checked.png"));
    numpad_enter->setIconSize(QSize(50, 50));
    gridLayout_2->addLayout(gridLayoutnumpad, 1, 0, 1, 1);
    gridLayoutnumpad->addWidget(numpad_enter, 0, 1, 1, 1);


    connect (numpad_point, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_0, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_1, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_2, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_3, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_4, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_5, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_6, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_7, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_8, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_9, SIGNAL( released() ), this, SLOT( Numpad_Sender_Clicked() ) );
    connect (numpad_enter,SIGNAL(released()),this,SLOT(Numpad_Enter_Clicked()));
    connect (numpad_cancel,SIGNAL(released()),this,SLOT(Numpad_Cancel_Clicked()));
    connect (numpad_clear,SIGNAL(released()),this,SLOT(Numpad_Clear_Clicked()));

    connect (key, SIGNAL(KBback() ), this, SLOT( keyboardback() ) );
    connect (key,SIGNAL(sendKBvalue(QString)),this,SLOT(keyboardvalue(QString)));
    connect (key,SIGNAL(KBsendok()),this,SLOT(keyboardEntervalue()));
    connect (key,SIGNAL(KBsendclear()),this,SLOT(keyboardClearvalue()));

    //TEXT Ayarları
    //MainWindow.setWindowTitle(QString());
    saat_lineEdit->setInputMask(QApplication::translate("MainWindow", "00:00:00", 0));
    // saat_lineEdit->setText(QApplication::translate("MainWindow", "17:25", 0));
    //tarih_lineEdit->setInputMask(QApplication::translate("MainWindow", "00/00/0000", 0));
    // tarih_lineEdit->setText(QApplication::translate("MainWindow", "30/01/2018", 0));
    //mepsan_logo->setText(QString());
    device_icon_button->setText(QString());
    status_label->setText(readyToSale_String);
    records_button->setText(records_String);

    enter_plate_button->setText(enterPlate_String);
    //message_textedit->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
    //"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
    //"p, li { white-space: pre-wrap; }\n"
    //"</style></head><body style=\" font-family:'Sans'; font-size:25pt; font-weight:400; font-style:normal;\">\n"
    //"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0));
    menu_button->setText(menu_String);
    //warning_lineEdit->setText(QString());
    //message_icon_button->setText(QString());
    //message_title_label->setText(QApplication::translate("MainWindow", "MESAJLAR", 0));
    //warning_title_label->setText(QApplication::translate("MainWindow", "UYARILAR", 0));
    warning_icon_button->setText(QString());

    numpad_8->setText(QApplication::translate("MainWindow", "8", 0));
    numpad_1->setText(QApplication::translate("MainWindow", "1", 0));
    numpad_5->setText(QApplication::translate("MainWindow", "5", 0));
    numpad_9->setText(QApplication::translate("MainWindow", "9", 0));
    numpad_3->setText(QApplication::translate("MainWindow", "3", 0));
    numpad_6->setText(QApplication::translate("MainWindow", "6", 0));
    numpad_7->setText(QApplication::translate("MainWindow", "7", 0));
    numpad_4->setText(QApplication::translate("MainWindow", "4", 0));
    numpad_2->setText(QApplication::translate("MainWindow", "2", 0));
    numpad_clear->setText(clean_String);
    numpad_0->setText(QApplication::translate("MainWindow", "0", 0));
    numpad_point->setText(QApplication::translate("MainWindow", ".", 0));
    numpad_cancel->setText(cancel_String);
    numpad_enter->setText(enter_String);
    numpadframe->setVisible(false);
    km_label->setText(enterKilometer_String);

    password_frame = new QFrame(this);
    password_frame->setObjectName(QStringLiteral("pswrd_frame"));
    password_frame->setGeometry(QRect(0, 0, 800, 480));
    password_frame->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                                "border:1px solid;\n"
                                                "border-top:2px solid;\n"
                                                "border-color: rgb(255, 255, 255);"));
    password_frame->setFrameShape(QFrame::StyledPanel);
    password_frame->setFrameShadow(QFrame::Raised);
    password_frame->setVisible(false);

    pmenter_frame = new QFrame(password_frame);
    pmenter_frame->setObjectName(QStringLiteral("pmenter_frame"));
    pmenter_frame->setGeometry(QRect(0, 0, 800, 150));
    pmenter_frame->setStyleSheet(QStringLiteral("border:0px;"));
    pmenter_frame->setFrameShape(QFrame::StyledPanel);
    pmenter_frame->setFrameShadow(QFrame::Raised);
    passwrdLine = new QLineEdit(pmenter_frame);
    passwrdLine->setObjectName(QStringLiteral("passwrdLine"));
    passwrdLine->setGeometry(QRect(10, 50, 781, 81));
    QFont passwrdFont;
    passwrdFont.setPointSize(75);
    passwrdFont.setBold(true);
    passwrdLine->setFont(passwrdFont);
    passwrdLine->setStyleSheet(QLatin1String("color: rgb(24, 24, 24);\n"
                                             "border:1px solid;\n"
                                             "border-radius:10px;\n"
                                             "border-color: rgb(24, 24, 24);"));
    passwrdLine->setInputMethodHints(Qt::ImhUppercaseOnly);
    passwrdLine->setEchoMode(QLineEdit::Password);
    passwrdLine->setAlignment(Qt::AlignCenter);
    //passwrdLine->setClearButtonEnabled(true);
    QFont fontPSWRD;
    fontPSWRD.setPointSize(20);
    fontPSWRD.setBold(true);
    fontPSWRD.setWeight(25);
    paswrd_label = new QLabel(pmenter_frame);
    paswrd_label->setObjectName(QStringLiteral("pswrd_label"));
    paswrd_label->setGeometry(QRect(135, 5, 530, 60));
    paswrd_label->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                              "color: rgb(24, 24, 24);\n"
                                              "border:1px solid;\n"
                                              "border-radius:5px;\n"
                                              "border-color: rgb(24, 24, 24);"));
    paswrd_label->setFont(fontPSWRD);
    paswrd_label->setAlignment(Qt::AlignCenter);
    paswrd_label->setText(automationErrorEnterPassword_String);
    paswrd_label->setScaledContents(true);

    Records_Frame= new QFrame(this);
    Records_Frame->setGeometry(0,0,800,480);
    Records_Frame->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                               "border:1px solid;\n"
                                               "border-top:2px solid;\n"
                                               "border-color: rgb(255, 255, 255);"));
    Records_Frame->setFrameShadow(QFrame::Plain);
    Records_Frame->setFrameShape(QFrame::NoFrame);
    Records_Frame->close();




    whitelist_button = new QToolButton(Records_Frame);
    whitelist_button->setObjectName(QStringLiteral("whitelist_button"));
    whitelist_button->setGeometry(QRect(13, 40, 381, 151));
    whitelist_button->setFont(font2);
    whitelist_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                  "background-color: rgb(0, 0, 0);\n"
                                                  "border:1px solid #FFFFFF;\n"
                                                  "border-radius:10px;"));
    QIcon whitelisticon;
    whitelisticon.addFile(QStringLiteral(":/new/prefix1/icons/list.png"), QSize(), QIcon::Normal, QIcon::Off);
    whitelist_button->setIcon(whitelisticon);
    whitelist_button->setIconSize(QSize(100, 100));
    whitelist_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);


    worker_button = new QToolButton(Records_Frame);
    worker_button->setObjectName(QStringLiteral("worker_button"));
    worker_button->setGeometry(QRect(12, 220, 381, 151));
    worker_button->setFont(font2);
    worker_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                               "background-color: rgb(0, 0, 0);\n"
                                               "border:1px solid #FFFFFF;\n"
                                               "border-radius:10px;"));
    QIcon workericon;
    workericon.addFile(QStringLiteral(":/new/prefix1/icons/parking-worker.png"), QSize(), QIcon::Normal, QIcon::Off);
    worker_button->setIcon(workericon);
    worker_button->setIconSize(QSize(100, 100));
    worker_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    sales_button = new QToolButton(Records_Frame);
    sales_button->setObjectName(QStringLiteral("sales_button"));
    sales_button->setGeometry(QRect(407, 40, 381, 151));
    sales_button->setFont(font2);
    sales_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                              "background-color: rgb(0, 0, 0);\n"
                                              "border:1px solid #FFFFFF;\n"
                                              "border-radius:10px;"));
    QIcon salesicon;
    salesicon.addFile(QStringLiteral(":/new/prefix1/icons/database1.png"), QSize(), QIcon::Normal, QIcon::Off);
    sales_button->setIcon(salesicon);
    sales_button->setIconSize(QSize(100, 100));
    sales_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    saved_records_button = new QToolButton(Records_Frame);
    saved_records_button->setObjectName(QStringLiteral("saved_records_button"));
    saved_records_button->setGeometry(QRect(408, 220, 381, 151));
    QFont recordsbuttonFont;
    recordsbuttonFont.setPointSize(23);
    recordsbuttonFont.setItalic(true);
    saved_records_button->setFont(recordsbuttonFont);
    saved_records_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                      "background-color: rgb(0, 0, 0);\n"
                                                      "border:1px solid #FFFFFF;\n"
                                                      "border-radius:10px;"));
    QIcon recordsicon;
    recordsicon.addFile(QStringLiteral(":/new/prefix1/icons/settingss.png"), QSize(), QIcon::Normal, QIcon::Off);
    saved_records_button->setIcon(recordsicon);
    saved_records_button->setIconSize(QSize(100, 100));
    saved_records_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    back_from_records_button = new QToolButton(Records_Frame);
    back_from_records_button->setObjectName(QStringLiteral("back_from_records_button"));
    back_from_records_button->setGeometry(QRect(500, 385, 290, 90));
    back_from_records_button->setFont(font2);
    back_from_records_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                          "background-color: rgb(0, 0, 0);\n"
                                                          "border:1px solid #FFFFFF;\n"
                                                          "border-radius:10px;"));

    back_from_records_button->setIcon(QIcon(":/new/prefix1/icons/forward.png"));;
    back_from_records_button->setIconSize(QSize(75, 75));
    back_from_records_button->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    back_from_Table_button = new QToolButton(Records_Frame);
    back_from_Table_button->setObjectName(QStringLiteral("back_from_Table_button"));
    back_from_Table_button->setGeometry(QRect(540, 410, 251, 65));
    back_from_Table_button->setFont(font2);
    back_from_Table_button->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                        "background-color: rgb(0, 0, 0);\n"
                                                        "border:1px solid #FFFFFF;\n"
                                                        "border-radius:10px;"));

    back_from_Table_button->setIcon(QIcon(":/new/prefix1/icons/forward.png"));;
    back_from_Table_button->setIconSize(QSize(75, 75));
    back_from_Table_button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    back_from_Table_button->setVisible(false);



    back_from_records_button->setText(back_String);
    back_from_Table_button->setText(back_String);
    whitelist_button->setText(whitelist_String);
    worker_button->setText(attendant_String);
    sales_button->setText(sales_String);
    saved_records_button->setText(recordedSettings_String);



    Personmodel=new QSqlQueryModel();
    Person_Frame= new QFrame(Records_Frame);
    Person_Frame->setGeometry(0,0,800,400);
    Person_Frame->setStyleSheet(QLatin1String("background-color: rgb(190, 190, 190);\n"
                                              "border:1px solid;\n"
                                              "border-top:2px solid;\n"
                                              "border-color: rgb(255, 255, 255);"));
    Person_Frame->setFrameShadow(QFrame::Plain);
    Person_Frame->setFrameShape(QFrame::NoFrame);
    Person_Frame->close();


    QFont Tablefont;
    Tablefont.setPointSize(20);
    Tablefont.setItalic(true);
    Persontable=new QTableView(this->Person_Frame);
    Persontable->setGeometry(0,0,800,380);
    Persontable->setFont(Tablefont);
    Persontable->setStyleSheet(QLatin1String("QTableView {background-color: rgb(210, 210, 210);\n"
                                             "border:1px solid;\n"
                                             "border-top:2px solid;\n"
                                             "border-color: rgb(255, 255, 255);}\n"
                                             "QScrollBar:horizontal {height:100px;}\n"
                                             "QScrollBar::handle:horizantal {background: darkgrey; margin: 0px 50px 0px 50px;}\n"
                                             "QScrollBar:vertical {width:100px;}\n"
                                             )); //TABLE VIEW


    Whitelistmodel=new QSqlQueryModel();
    Whitelist_Frame= new QFrame(Records_Frame);
    Whitelist_Frame->setGeometry(0,0,800,400);
    Whitelist_Frame->setStyleSheet(QLatin1String("background-color: rgb(190, 190, 190);\n"
                                                 "border:1px solid;\n"
                                                 "border-top:2px solid;\n"
                                                 "border-color: rgb(255, 255, 255);"));
    Whitelist_Frame->setFrameShadow(QFrame::Plain);
    Whitelist_Frame->setFrameShape(QFrame::NoFrame);
    Whitelist_Frame->close();

    Whitelisttable=new QTableView(this->Whitelist_Frame);
    Whitelisttable->setGeometry(0,0,800,380);
    Whitelisttable->setFont(Tablefont);
    Whitelisttable->setStyleSheet(QLatin1String("QTableView {background-color: rgb(210, 210, 210);\n"
                                                "border:1px solid;\n"
                                                "border-top:2px solid;\n"
                                                "border-color: rgb(255, 255, 255);}\n"
                                                "QScrollBar:horizontal {height:50px;}\n"
                                                "QScrollBar:vertical {width:50px;}\n"
                                                "QScrollBar::handle:horizantal {background: darkgrey; margin: 0px 50px 0px 50px;}\n"
                                                "QScrollBar::handle:vertical {margin: 0px 50px 0px 50px;}\n"
                                                ));


    Nozzlemodel=new QSqlQueryModel();
    Nozzle_Frame= new QFrame(Records_Frame);
    Nozzle_Frame->setGeometry(0,0,800,400);
    Nozzle_Frame->setStyleSheet(QLatin1String("background-color: rgb(190, 190, 190);\n"
                                              "border:1px solid;\n"
                                              "border-top:2px solid;\n"
                                              "border-color: rgb(255, 255, 255);"));
    Nozzle_Frame->setFrameShadow(QFrame::Plain);
    Nozzle_Frame->setFrameShape(QFrame::NoFrame);
    Nozzle_Frame->close();

    Nozzletable=new QTableView(this->Nozzle_Frame);
    Nozzletable->setGeometry(0,0,800,380);
    Nozzletable->setFont(Tablefont);
    Nozzletable->setStyleSheet(QLatin1String("QTableView {background-color: rgb(210, 210, 210);\n"
                                             "border:1px solid;\n"
                                             "border-top:2px solid;\n"
                                             "border-color: rgb(255, 255, 255);}\n"
                                             "QScrollBar:horizontal {height:50px;}\n"
                                             "QScrollBar:vertical {width:50px;}\n"

                                             ));
    //"QScrollBar::handle:horizantal {background: darkgrey; margin: 0px 50px 0px 50px;}\n"



    Salesmodel=new QSqlQueryModel();
    Sales_Frame= new QFrame(Records_Frame);
    Sales_Frame->setGeometry(0,0,800,400);
    Sales_Frame->setStyleSheet(QLatin1String("background-color: rgb(190, 190, 190);\n"
                                             "border:1px solid;\n"
                                             "border-top:2px solid;\n"
                                             "border-color: rgb(255, 255, 255);"));
    Sales_Frame->setFrameShadow(QFrame::Plain);
    Sales_Frame->setFrameShape(QFrame::NoFrame);
    Sales_Frame->close();

    Salestable=new QTableView(this->Sales_Frame);
    Salestable->setGeometry(0,0,800,380);
    Salestable->setFont(Tablefont);
    Salestable->setStyleSheet(QLatin1String("QTableView {background-color: rgb(210, 210, 210);\n"
                                            "border:1px solid;\n"
                                            "border-top:2px solid;\n"
                                            "border-color: rgb(255, 255, 255);}\n"
                                            "QScrollBar:horizontal {height:50px;}\n"
                                            "QScrollBar:vertical {width:50px;}\n"

                                            ));
    //"QScrollBar::handle:horizantal {background: darkgrey; margin: 0px 50px 0px 50px;}\n"


    connect(records_button,SIGNAL(released()),this,SLOT(Enter_Record_Clicked()));
    connect(whitelist_button,SIGNAL(released()),this,SLOT(whitelist_button_clicked()));
    connect(sales_button,SIGNAL(released()),this,SLOT(sales_button_clicked()));
    connect(worker_button,SIGNAL(released()),this,SLOT(worker_button_clicked()));
    connect(saved_records_button,SIGNAL(released()),this,SLOT(records_button_clicked()));
    connect(back_from_Table_button,SIGNAL(released()),this,SLOT(Enter_Record_Clicked()));
    connect(back_from_records_button,SIGNAL(released()),this,SLOT(back_from_records_button_Clicked()));
    connect(menu_button,SIGNAL(released()),this,SLOT(menu_button_Clicked()));

    /***************MENU FRAME**********************************************/
    menu_frame = new QFrame(this);
    menu_frame->setObjectName(QStringLiteral("menu_frame"));
    menu_frame->setGeometry(QRect(0, 0, 800, 480));
    QFont font13;
    font13.setPointSize(10);
    menu_frame->setFont(font13);
    menu_frame->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                            ""));
    menu_frame->setFrameShape(QFrame::StyledPanel);
    menu_frame->setFrameShadow(QFrame::Raised);
    menu_frame->close();

    menu_cancel = new QPushButton(menu_frame);
    menu_cancel->setObjectName(QStringLiteral("menu_cancel"));
    menu_cancel->setGeometry(QRect(50, 380, 325, 70));
    menu_cancel->setFont(font6);
    menu_cancel->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n"
                                             "color: rgb(255, 255, 255);\n"
                                             "border:2px solid #FFFFFF;\n"
                                             "border-radius:10px;"));
    menu_cancel->setIcon(QIcon(":/new/prefix1/icons/forward.png"));
    menu_cancel->setIconSize(QSize(50, 50));
    menu_cancel->setText(cancel_String);


    menu_enter = new QPushButton(menu_frame);
    menu_enter->setObjectName(QStringLiteral("menu_enter"));
    menu_enter->setGeometry(QRect(425, 380, 325, 70));
    menu_enter->setFont(font6);
    menu_enter->setStyleSheet(QLatin1String("background-color: rgb(47, 137, 53);\n"
                                            "color: rgb(255, 255, 255);\n"
                                            "border:2px solid #FFFFFF;\n"
                                            "border-radius:10px;"));
    menu_enter->setIcon(QIcon(":/new/prefix1/icons/checked.png"));
    menu_enter->setIconSize(QSize(50, 50));
    menu_enter->setText(save_String);


    menu_tabWidget = new QTabWidget(menu_frame);
    menu_tabWidget->setObjectName(QStringLiteral("menu_tabWidget"));
    menu_tabWidget->setGeometry(QRect(5, 5, 800, 355));
//    QToolButton *tb =new QToolButton();
//    tb->setText("->");
    //menu_tabWidget->tabBar()->setTabButton(0,QTabWidget::Left,tb);
    QFont font14;
    font14.setPointSize(13);
    font14.setBold(true);
    font14.setItalic(true);
    menu_tabWidget->setFont(font14);
    menu_tabWidget->setUsesScrollButtons(true);
    menu_tabWidget->setStyleSheet(QLatin1String("QTabWidget::pane {\n"
                                                "border-top: 1px solid #000000;\n"
                                                "border-left: 0px solid #D60000;\n"
                                                "border-right:0px solid #D60000;\n"
                                                "top: -1px;\n"
                                                "}\n"
                                                "QTabWidget::tab-bar {\n"
                                                "left: 0px;\n"
                                                "right: 0px;\n"
                                                "}\n"
                                                "QTabBar::tab {\n"
                                                "background: rgb(214,0,0);\n"
                                                "color: rgb(255,255,255);\n"
                                                "height: 40px;\n"
                                                "width: 140px;\n"
                                                "border: 1px solid #FFFFFF;\n"
                                                "border-bottom-color:rgb(0, 0, 0);\n"
                                                "border-top-left-radius: 4px;\n"
                                                "border-top-right-radius: 4px;\n"
                                                "min-width: 8px;\n"
                                                "padding: 2px;\n"
                                                "}\n"
                                                "QTabBar::tab:selected,QTabBar::tab:hover {\n"
                                                "background: rgb(107, 107, 107);\n"
                                                "border-bottom-color: rgb(107, 107, 107); \n"
                                                "}\n"
                                                "QTabBar::selected {\n"
                                                "margin-left: 4px;\n"
                                                "margin-right: 4px;\n"
                                                "}\n"
                                                "QTabBar::tab:!selected {\n"
                                                "margin-top: 5px;\n"
                                                "margin-bottom: -5px;\n"
                                                "margin-left: -4px;\n"
                                                "margin-right: -4px;\n"
                                                "}\n"
                                                "QTabBar::tab:first:selected {\n"
                                                "margin-left: -4px;\n"
                                                "}\n"
                                                "QTabbar::tab:last:selected {\n"
                                                "margin-right: 8px;\n"
                                                "}\n"
                                                "QTabBar::scroller {width: 100px; }"
                                                "QTabBar QToolButton { width: 50px; border-width:2px;}"
                                                //"QToolButton {color: red; border: solid;width: 80px; }\n"
                                                ));

    menu_tabWidget->setFocusPolicy(Qt::NoFocus);
    QFont font56;
    font56.setPointSize(20);
    font56.setBold(true);

    //nextTabPushbutton = new QPushButton(menu_frame);
//    nextTabPushbutton->setObjectName(QStringLiteral("nextTabPushbutton"));
//    //nextTabPushbutton->setGeometry(QRect(0, 5, 40, 50));
//    nextTabPushbutton->setFont(font56);
//    //nextTabPushbutton->setText("<-");
//    nextTabPushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
//                                                    "color: rgb(255,255,255);\n"
//                                                    "border:1px solid #FFFFFF;\n"
//                                                    "border-radius:4px;"));
    //previusTabPushbutton = new QPushButton(menu_frame);
//    previusTabPushbutton->setObjectName(QStringLiteral("previusTabPushbutton"));
//   // previusTabPushbutton->setGeometry(QRect(760, 5, 40, 50));
//    previusTabPushbutton->setFont(font56);
//    //previusTabPushbutton->setText("->");
//    previusTabPushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
//                                                    "color: rgb(255,255,255);\n"
//                                                    "border:1px solid #FFFFFF;\n"
//                                                    "border-radius:4px;"));


    devicesettingtab = new QWidget();
    devicesettingtab->setObjectName(QStringLiteral("devicesettingtab"));
    menu_tabWidget->addTab(devicesettingtab, QString());

    receipttab = new QWidget();
    receipttab->setObjectName(QStringLiteral("receipttab"));
    menu_tabWidget->addTab(receipttab, QString());

    producttab = new QWidget();
    producttab->setObjectName(QStringLiteral("producttab"));
    menu_tabWidget->addTab(producttab, QString());


    kmtab = new QWidget();
    kmtab->setObjectName(QStringLiteral("kmtab"));
    //menu_tabWidget->addTab(kmtab, QString());


    licance_pushbutton = new QPushButton(kmtab);
    licance_pushbutton->setObjectName(QStringLiteral("licance_pushbutton"));
    licance_pushbutton->setGeometry(QRect(123, 50, 250, 70));
    licance_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border:1px solid #FFFFFF;\n"
                                                    "border-radius:10px;"));
    saha_modu_pushbutton = new QPushButton(kmtab);
    saha_modu_pushbutton->setObjectName(QStringLiteral("saha_modu_pushbutton"));
    saha_modu_pushbutton->setGeometry(QRect(425, 50, 250, 70));
    saha_modu_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                      "color: rgb(255,255,255);\n"
                                                      "border:1px solid #FFFFFF;\n"
                                                      "border-radius:10px;"));

    sd_kart_yedek_pushbutton = new QPushButton(kmtab);
    sd_kart_yedek_pushbutton->setObjectName(QStringLiteral("sd_kart_yedek_pushbutton"));
    sd_kart_yedek_pushbutton->setGeometry(QRect(123, 130, 250, 70));
    sd_kart_yedek_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                          "color: rgb(255,255,255);\n"
                                                          "border:1px solid #FFFFFF;\n"
                                                          "border-radius:10px;"));

    sd_kart_al_pushbutton = new QPushButton(kmtab);
    sd_kart_al_pushbutton->setObjectName(QStringLiteral("sd_kart_al_pushbutton"));
    sd_kart_al_pushbutton->setGeometry(QRect(425, 130, 250, 70));
    sd_kart_al_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                       "color: rgb(255,255,255);\n"
                                                       "border:1px solid #FFFFFF;\n"
                                                       "border-radius:10px;"));

    update_pushbutton = new QPushButton(kmtab);
    update_pushbutton->setObjectName(QStringLiteral("update_pushbutton"));
    update_pushbutton->setGeometry(QRect(275, 210, 250, 70));
    update_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                   "color: rgb(255,255,255);\n"
                                                   "border:1px solid #FFFFFF;\n"
                                                   "border-radius:10px;"));
    sifretab = new QWidget();
    sifretab->setObjectName(QStringLiteral("sifretab"));
    menu_tabWidget->addTab(sifretab, QString());



    testmodetab = new QWidget();
    testmodetab->setObjectName(QStringLiteral("testmodetab"));

    testmodestart = new QPushButton(testmodetab);
    testmodestart->setObjectName(QStringLiteral("testmodestart"));
    testmodestart->setGeometry(QRect(295, 140, 190, 61));
    QFont font17;
    font17.setPointSize(15);
    font17.setBold(false);
    font17.setWeight(50);
    testmodestart->setFont(font17);
    testmodestart->setStyleSheet(QLatin1String("background-color: rgb(39, 133, 94);\n"
                                               "color: rgb(255, 255, 255);\n"
                                               "border:2px solid #FFFFFF;\n"
                                               "border-radius:10px;"));




    access_test_label = new QLabel(testmodetab);
    access_test_label->setObjectName(QStringLiteral("access_test_label"));
    access_test_label->setGeometry(QRect(70, 18, 161, 91));
    access_test_label->setFont(font1);
    access_test_label->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                   "background-color: rgb(0, 0, 0);\n"
                                                   "border:1px solid #FFFFFF;\n"
                                                   "border-radius:10px;"));
    access_test_label->setAlignment(Qt::AlignCenter);

    printer_test_label = new QLabel(testmodetab);
    printer_test_label->setObjectName(QStringLiteral("printer_test_label"));
    printer_test_label->setGeometry(QRect(310, 20, 161, 91));
    printer_test_label->setFont(font1);
    printer_test_label->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                    "background-color: rgb(0, 0, 0);\n"
                                                    "border:1px solid #FFFFFF;\n"
                                                    "border-radius:10px;"));
    printer_test_label->setAlignment(Qt::AlignCenter);

    cardreader_test_label = new QLabel(testmodetab);
    cardreader_test_label->setObjectName(QStringLiteral("cardreader_test_label"));
    cardreader_test_label->setGeometry(QRect(550, 20, 161, 91));
    cardreader_test_label->setFont(font1);
    cardreader_test_label->setStyleSheet(QLatin1String("color: rgb(255,255,255);\n"
                                                       "background-color: rgb(0, 0, 0);\n"
                                                       "border:1px solid #FFFFFF;\n"
                                                       "border-radius:10px;"));
    cardreader_test_label->setAlignment(Qt::AlignCenter);



    icon9.addFile(QStringLiteral(":/new/prefix1/icons/checked.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconcancel.addFile(QStringLiteral(":/new/prefix1/icons/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
    QFont font21;
    font21.setPointSize(12);

    accesspoint_test_status = new QToolButton(testmodetab);
    accesspoint_test_status->setObjectName(QStringLiteral("accesspoint_test_status"));
    accesspoint_test_status->setGeometry(QRect(50, 0, 61, 61));
    accesspoint_test_status->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255,0);\n"
                                                         "border:0px;"));
    accesspoint_test_status->setIcon(iconcancel);
    accesspoint_test_status->setIconSize(QSize(50, 50));

    printer_test_status = new QToolButton(testmodetab);
    printer_test_status->setObjectName(QStringLiteral("printer_test_status"));
    printer_test_status->setGeometry(QRect(280, 0, 61, 61));
    printer_test_status->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255,0);\n"
                                                     "border:0px;"));
    printer_test_status->setIcon(icon9);
    printer_test_status->setIconSize(QSize(50, 50));

    cardreader_test_status = new QToolButton(testmodetab);
    cardreader_test_status->setObjectName(QStringLiteral("cardreader_test_status"));
    cardreader_test_status->setGeometry(QRect(520, 0, 61, 61));
    cardreader_test_status->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255,0);\n"
                                                        "border:0px;"));
    cardreader_test_status->setIcon(icon9);
    cardreader_test_status->setIconSize(QSize(50, 50));
    cardreader_test_status->setVisible(false);
    printer_test_status->setVisible(false);
    accesspoint_test_status->setVisible(false);
//**************************** SAAT-TARİH**********************//
    datesettingtab = new QWidget();
    datesettingtab->setObjectName(QStringLiteral("datesettingtab"));
    menu_tabWidget->addTab(datesettingtab, QString());

    QFont dateSetFont;
    dateSetFont.setPointSize(25);

    tarih_sor = new QLabel(datesettingtab);
    tarih_sor->setObjectName(QStringLiteral("tarih_sor"));
    tarih_sor->setGeometry(QRect(220, 100, 100, 50));
    tarih_sor->setFont(dateSetFont);
    tarih_sor->setStyleSheet(QStringLiteral("border:0px;"));

    saat_sor = new QLabel(datesettingtab);
    saat_sor->setObjectName(QStringLiteral("saat_sor"));
    saat_sor->setGeometry(QRect(220, 160, 100, 50));
    saat_sor->setFont(dateSetFont);
    saat_sor->setStyleSheet(QStringLiteral("border:0px;"));


    tarih_1_lineEdit = new QDateEdit(datesettingtab);
    tarih_1_lineEdit->setObjectName(QStringLiteral("tarih_1_lineEdit"));
    tarih_1_lineEdit->setGeometry(QRect(370, 100, 210, 50));
    tarih_1_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                  "color: rgb(255,255,255);\n"
                                                  "border-radius:10px;"));
    tarih_1_lineEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    tarih_1_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
  //  tarih_1_lineEdit->setMaxLength(21);
    tarih_1_lineEdit->setFont(dateSetFont);
    tarih_1_lineEdit->setAlignment(Qt::AlignCenter);
    tarih_1_lineEdit->setDisplayFormat("dd/MM/yyyy");
    tarih_1_lineEdit->setDateTime(QDateTime::currentDateTime());

    saat_1_lineEdit = new QTimeEdit(datesettingtab);
    saat_1_lineEdit->setObjectName(QStringLiteral("saat_1_lineEdit"));
    saat_1_lineEdit->setGeometry(QRect(370, 160, 210, 50));
    saat_1_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                 "color: rgb(255,255,255);\n"
                                                 "border-radius:10px;"));
    saat_1_lineEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    saat_1_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    //saat_1_lineEdit->setMaxLength(21);
    saat_1_lineEdit->setDisplayFormat("hh:mm:ss");

    saat_1_lineEdit->setFont(dateSetFont);
    saat_1_lineEdit->setAlignment(Qt::AlignCenter);


    numpadDateTimeEdit = new QLineEdit(datesettingtab);
    numpadDateTimeEdit->setObjectName(QStringLiteral("receipt_2_lineEdit"));
    numpadDateTimeEdit->setGeometry(QRect(170, 15, 510, 60));
    QFont numpadFont;
    numpadFont.setPointSize(25);
    numpadFont.setBold(true);
    numpadDateTimeEdit->setFont(numpadFont);
    numpadDateTimeEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                  "color: rgb(255,255,255);\n"
                                                  "border-radius:10px;"));
    numpadDateTimeEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    numpadDateTimeEdit->setEchoMode(QLineEdit::Normal);
    numpadDateTimeEdit->setAlignment(Qt::AlignCenter);
    //numpadDateTimeEdit->setVisible(false);

    setDate_pushbutton = new QPushButton(datesettingtab);
    setDate_pushbutton->setObjectName(QStringLiteral("setDate_pushbutton"));
    setDate_pushbutton->setGeometry(QRect(220, 230, 360, 60));
    setDate_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(0, 190, 50);\n"
                                                   "color: rgb(255,255,255);\n"
                                                   "border:1px solid #000000;\n"
                                                   "border-radius:10px;"));
    QFont dateButtonFont;
    dateButtonFont.setPointSize(15);
    setDate_pushbutton->setFont(numpadFont);
    setDate_pushbutton->setText("DEĞİŞTİR");
//***************saat-tarih******************//

    menu_tabWidget->addTab(testmodetab, QString());
//**************************satış kontrol tab*******//
    QFont font77;
    font77.setPointSize(16);
    font77.setItalic(true);
    font77.setUnderline(true);

    saleSettingTab = new QWidget();
    saleSettingTab->setObjectName(QStringLiteral("saleSettingTab"));
    menu_tabWidget->addTab(saleSettingTab, QString());

    freeSaleLabel = new QLabel(saleSettingTab);
    freeSaleLabel->setObjectName(QStringLiteral("kmsor_label"));
    freeSaleLabel->setGeometry(QRect(180, 135, 151, 61));
    freeSaleLabel->setFont(font77);
    freeSaleLabel->setText(freePlate_String);
    freeSaleLabel->setVisible(false);//visible false yaptık kullanacakken açıklama satırı yap

    freeSaleCheck = new QCheckBox(saleSettingTab);
    freeSaleCheck->setObjectName(QStringLiteral("kmsor_check"));
    freeSaleCheck->setGeometry(QRect(300, 135, 151, 61));
    freeSaleCheck->setFocusPolicy(Qt::NoFocus);
    freeSaleCheck->setStyleSheet(QLatin1String("QCheckBox {\n"
                                             "border:0px;\n"
                                             "}\n"
                                             "QCheckBox::indicator{\n"
                                             "width: 50px;\n"
                                             "height:50px;\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:1px solid #FFFFFF;\n"
                                             "border-radius:5px;\n"
                                             "}\n"
                                             "QCheckBox::indicator:checked {\n"
                                             "	image: url(:/new/prefix1/icons/checkmark.png);\n"
                                             "}\n"
                                             "QCheckBox {\n"
                                             "	color: rgb(255, 255, 255);\n"
                                             "}\n"
                                             ""));
    //kmsor_check->setChecked(false);
    freeSaleCheck->setTristate(false);
    freeSaleCheck->setText(QString());
    freeSaleCheck->setVisible(false);//visible false yaptık kullanacakken açıklama satırı yap
//*************plate sale******

    plateSaleLabel = new QLabel(saleSettingTab);
    plateSaleLabel->setObjectName(QStringLiteral("kmsor_label"));
    plateSaleLabel->setGeometry(QRect(180, 65, 151, 61));
    plateSaleLabel->setFont(font77);
    plateSaleLabel->setText(plateSale_String);
    plateSaleLabel->setVisible(false);//visible false yaptık kullanacakken açıklama satırı yap

    plateSaleCheck = new QCheckBox(saleSettingTab);
    plateSaleCheck->setObjectName(QStringLiteral("kmsor_check"));
    plateSaleCheck->setGeometry(QRect(300, 65, 151, 61));
    plateSaleCheck->setFocusPolicy(Qt::NoFocus);
    plateSaleCheck->setStyleSheet(QLatin1String("QCheckBox {\n"
                                             "border:0px;\n"
                                             "}\n"
                                             "QCheckBox::indicator{\n"
                                             "width: 50px;\n"
                                             "height:50px;\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:1px solid #FFFFFF;\n"
                                             "border-radius:5px;\n"
                                             "}\n"
                                             "QCheckBox::indicator:checked {\n"
                                             "	image: url(:/new/prefix1/icons/checkmark.png);\n"
                                             "}\n"
                                             "QCheckBox {\n"
                                             "	color: rgb(255, 255, 255);\n"
                                             "}\n"
                                             ""));
    //kmsor_check->setChecked(false);
    plateSaleCheck->setTristate(false);
    plateSaleCheck->setText(QString());
    plateSaleCheck->setVisible(false);//visible false yaptık kullanacakken açıklama satırı yap


//*****************plate sale *****

//********************satış kontrol tab*******//
    generalset=new QWidget();
    generalset->setObjectName(QStringLiteral("generalset"));

    //menu_tabWidget->addTab(generalset, QString());



    access_point_read_pushbutton = new QPushButton(generalset);
    access_point_read_pushbutton->setObjectName(QStringLiteral("access_point_read_pushbutton"));
    access_point_read_pushbutton->setGeometry(QRect(100, 50, 225, 80));
    access_point_read_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                              "color: rgb(255,255,255);\n"
                                                              "border:1px solid #FFFFFF;\n"
                                                              "border-radius:10px;"));

    access_point_read_pushbutton->setText(readAccesspoint_String);


    access_point_write_pushbutton = new QPushButton(generalset);
    access_point_write_pushbutton->setObjectName(QStringLiteral("access_point_write_pushbutton"));
    access_point_write_pushbutton->setGeometry(QRect(475, 50, 225, 80));
    access_point_write_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                               "color: rgb(255,255,255);\n"
                                                               "border:1px solid #FFFFFF;\n"
                                                               "border-radius:10px;"));

    access_point_write_pushbutton->setText(writeAccesspoint_String);


    tts_active_pushbutton = new QPushButton(generalset);
    tts_active_pushbutton->setObjectName(QStringLiteral("tts_active_pushbutton"));
    tts_active_pushbutton->setGeometry(QRect(282, 150, 226, 80));
    tts_active_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(68,160,220);\n"
                                                       "color: rgb(255,255,255);\n"
                                                       "border:1px solid #FFFFFF;\n"
                                                       "border-radius:10px;"));

    tts_active_pushbutton->setText(visOpen_String);


    otomasyon_adres_label = new QLabel(generalset);
    otomasyon_adres_label->setObjectName(QStringLiteral("otomasyon_adres_label"));
    otomasyon_adres_label->setGeometry(QRect(40, 250, 150, 40));
    otomasyon_adres_label->setFont(font1);
    otomasyon_adres_label->setStyleSheet(QLatin1String("color: rgb(0,0,0);\n"
                                                       "background-color: rgb(0,0,0,0);\n"
                                                       "border:0px solid #FFFFFF;\n"
                                                       "border-radius:0px;"));
    otomasyon_adres_label->setAlignment(Qt::AlignCenter);
    otomasyon_adres_label->setText(autoAdr01_String);


    access_point_info_label = new QLabel(generalset);
    access_point_info_label->setObjectName(QStringLiteral("access_point_info_label"));
    access_point_info_label->setGeometry(QRect(100, 5, 600, 25));
    access_point_info_label->setFont(font1);
    access_point_info_label->setStyleSheet(QLatin1String("color: rgb(0,0,0);\n"
                                                         "background-color: rgb(0,0,0,0);\n"
                                                         "border:0px solid #FFFFFF;\n"
                                                         "border-radius:0px;"));
    access_point_info_label->setAlignment(Qt::AlignCenter);
    access_point_info_label->setText(accesspointSettingsReadingSuccessful_String);

    nozzle_adres_label = new QLabel(generalset);
    nozzle_adres_label->setObjectName(QStringLiteral("nozzle_adres_label"));
    nozzle_adres_label->setGeometry(QRect(230, 250, 150, 40));
    nozzle_adres_label->setFont(font1);
    nozzle_adres_label->setStyleSheet(QLatin1String("color: rgb(0,0,0);\n"
                                                    "background-color: rgb(0,0,0,0);\n"
                                                    "border:0px solid #FFFFFF;\n"
                                                    "border-radius:0px;"));
    nozzle_adres_label->setAlignment(Qt::AlignCenter);
    nozzle_adres_label->setText(nozAdr01_String);

    channel_label = new QLabel(generalset);
    channel_label->setObjectName(QStringLiteral("channel_label"));
    channel_label->setGeometry(QRect(420, 250, 150, 40));
    channel_label->setFont(font1);
    channel_label->setStyleSheet(QLatin1String("color: rgb(0,0,0);\n"
                                               "background-color: rgb(0,0,0,0);\n"
                                               "border:0px solid #FFFFFF;\n"
                                               "border-radius:0px;"));
    channel_label->setAlignment(Qt::AlignCenter);
    channel_label->setText(channel01_String);


    pause_time_label = new QLabel(generalset);
    pause_time_label->setObjectName(QStringLiteral("pause_time_label"));
    pause_time_label->setGeometry(QRect(610, 250, 150, 40));
    pause_time_label->setFont(font1);
    pause_time_label->setStyleSheet(QLatin1String("color: rgb(0,0,0);\n"
                                                  "background-color: rgb(0,0,0,0);\n"
                                                  "border:0px solid #FFFFFF;\n"
                                                  "border-radius:0px;"));
    pause_time_label->setAlignment(Qt::AlignCenter);
    pause_time_label->setText(stopZ08_String);


    otomasyon_adres_new_label = new QLabel(generalset);
    otomasyon_adres_new_label->setObjectName(QStringLiteral("otomasyon_adres_new_label"));
    otomasyon_adres_new_label->setGeometry(QRect(40, 20, 150, 20));
    otomasyon_adres_new_label->setFont(font1);
    otomasyon_adres_new_label->setAlignment(Qt::AlignCenter);
    otomasyon_adres_new_label->setStyleSheet(QStringLiteral("border:0px;"));

    otomasyon_adres_new_label->setText(otoAdr_String);

    otomasyon_adres_new_label->setVisible(false);


    nozzle_adres_new_label = new QLabel(generalset);
    nozzle_adres_new_label->setObjectName(QStringLiteral("nozzle_adres_new_label"));
    nozzle_adres_new_label->setGeometry(QRect(230, 20, 150, 20));
    nozzle_adres_new_label->setFont(font1);
    nozzle_adres_new_label->setAlignment(Qt::AlignCenter);
    nozzle_adres_new_label->setStyleSheet(QStringLiteral("border:0px;"));

    nozzle_adres_new_label->setText(nozzAdr_String);

    nozzle_adres_new_label->setVisible(false);


    channel_new_label = new QLabel(generalset);
    channel_new_label->setObjectName(QStringLiteral("channel_new_label"));
    channel_new_label->setGeometry(QRect(420, 20, 150, 20));
    channel_new_label->setFont(font1);
    channel_new_label->setAlignment(Qt::AlignCenter);
    channel_new_label->setStyleSheet(QStringLiteral("border:0px;"));

    channel_new_label->setText(channelNo_String);

    channel_new_label->setVisible(false);


    pause_time_new_label = new QLabel(generalset);
    pause_time_new_label->setObjectName(QStringLiteral("pause_time_new_label"));
    pause_time_new_label->setGeometry(QRect(610, 20, 150, 20));
    pause_time_new_label->setFont(font1);
    pause_time_new_label->setAlignment(Qt::AlignCenter);
    pause_time_new_label->setStyleSheet(QStringLiteral("border:0px;"));

    pause_time_new_label->setText(stopZ_String);

    pause_time_new_label->setVisible(false);


    otomasyon_adres_lineEdit = new QLineEdit(generalset);
    otomasyon_adres_lineEdit->setObjectName(QStringLiteral("otomasyon_adres_lineEdit"));
    otomasyon_adres_lineEdit->setGeometry(QRect(40, 50, 150, 40));
    otomasyon_adres_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                          "color: rgb(255,255,255);\n"
                                                          "border-radius:10px;"));
    otomasyon_adres_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    otomasyon_adres_lineEdit->setMaxLength(21);
    otomasyon_adres_lineEdit->setFont(font21);
    otomasyon_adres_lineEdit->setAlignment(Qt::AlignCenter);


    nozzle_adres_lineEdit = new QLineEdit(generalset);
    nozzle_adres_lineEdit->setObjectName(QStringLiteral("nozzle_adres_lineEdit"));
    nozzle_adres_lineEdit->setGeometry(QRect(230, 50, 150, 40));
    nozzle_adres_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                       "color: rgb(255,255,255);\n"
                                                       "border-radius:10px;"));
    nozzle_adres_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    nozzle_adres_lineEdit->setMaxLength(21);
    nozzle_adres_lineEdit->setFont(font21);
    nozzle_adres_lineEdit->setAlignment(Qt::AlignCenter);


    channel_lineEdit = new QLineEdit(generalset);
    channel_lineEdit->setObjectName(QStringLiteral("channel_lineEdit"));
    channel_lineEdit->setGeometry(QRect(420, 50, 150, 40));
    channel_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                  "color: rgb(255,255,255);\n"
                                                  "border-radius:10px;"));
    channel_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    channel_lineEdit->setMaxLength(21);
    channel_lineEdit->setFont(font21);
    channel_lineEdit->setAlignment(Qt::AlignCenter);


    pause_time_lineEdit = new QLineEdit(generalset);
    pause_time_lineEdit->setObjectName(QStringLiteral("pause_time_lineEdit"));
    pause_time_lineEdit->setGeometry(QRect(610, 50, 150, 40));
    pause_time_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                     "color: rgb(255,255,255);\n"
                                                     "border-radius:10px;"));
    pause_time_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    pause_time_lineEdit->setMaxLength(21);
    pause_time_lineEdit->setFont(font21);
    pause_time_lineEdit->setAlignment(Qt::AlignCenter);


    otomasyon_adres_pushbutton = new QPushButton(generalset);
    otomasyon_adres_pushbutton->setObjectName(QStringLiteral("otomasyon_adres_pushbutton"));
    otomasyon_adres_pushbutton->setGeometry(QRect(40, 50, 150, 40));
    otomasyon_adres_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                            "color: rgb(255,255,255);\n"
                                                            "border:1px solid #FFFFFF;\n"
                                                            "border-radius:10px;"));
    otomasyon_adres_pushbutton->setText("");

    nozzle_adres_pushbutton = new QPushButton(generalset);
    nozzle_adres_pushbutton->setObjectName(QStringLiteral("nozzle_adres_pushbutton"));
    nozzle_adres_pushbutton->setGeometry(QRect(230, 50, 150, 40));
    nozzle_adres_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                         "color: rgb(255,255,255);\n"
                                                         "border:1px solid #FFFFFF;\n"
                                                         "border-radius:10px;"));
    nozzle_adres_pushbutton->setText("");


    channel_pushbutton = new QPushButton(generalset);
    channel_pushbutton->setObjectName(QStringLiteral("channel_pushbutton"));
    channel_pushbutton->setGeometry(QRect(420, 50, 150, 40));
    channel_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border:1px solid #FFFFFF;\n"
                                                    "border-radius:10px;"));

    channel_pushbutton->setText("");


    pause_time_pushbutton = new QPushButton(generalset);
    pause_time_pushbutton->setObjectName(QStringLiteral("pause_time_pushbutton"));
    pause_time_pushbutton->setGeometry(QRect(610, 50, 150, 40));
    pause_time_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                       "color: rgb(255,255,255);\n"
                                                       "border:1px solid #FFFFFF;\n"
                                                       "border-radius:10px;"));

    pause_time_pushbutton->setText("");


    otomasyon_adres_lineEdit->setVisible(false);
    nozzle_adres_lineEdit->setVisible(false);
    channel_lineEdit->setVisible(false);
    pause_time_lineEdit->setVisible(false);
    otomasyon_adres_pushbutton->setVisible(false);
    nozzle_adres_pushbutton->setVisible(false);
    channel_pushbutton->setVisible(false);
    pause_time_pushbutton->setVisible(false);



    QFont font16;
    font16.setPointSize(16);
    font16.setItalic(true);
    font16.setUnderline(true);

    for(int i=0;i<=6;i++)
    {
        product_comboBox_label[i] = new QLabel(producttab);
        product_comboBox_label[i]->setObjectName("product_comboBox_label"+QString::number(i));
        if(i<3)
        {
            if(i==0){product_comboBox_label[i]->setGeometry(QRect(10, 10+(i*81), 151, 81));  }
            else {product_comboBox_label[i]->setGeometry(QRect(10, 10+(20*(i))+((i)*81), 151, 81));}
        }
        else
        {
            product_comboBox_label[i]->setGeometry(QRect(390, 10+(10*((i-3)))+((i-3)*81), 151, 81));
        }
        product_comboBox_label[i]->setFont(font16);
        product_comboBox_label[i]->setText(nozzle_String+QString::number(i+1)+":");



        product_comboBox[i] = new QComboBox(producttab);
        product_comboBox[i]->setObjectName("product_comboBox"+QString::number(i));
        if(i<3)
        {
            if(i==0){product_comboBox[i]->setGeometry(QRect(130, 10+(i*81), 230, 81));}
            else {product_comboBox[i]->setGeometry(QRect(130, 10+(20*(i))+((i)*81), 230, 81));}
        }
        else
        {
            product_comboBox[i]->setGeometry(QRect(541, 10+(20*(i-3))+((i-3)*81), 194, 81));
        }
        product_comboBox[i]->setFont(font);
        product_comboBox[i]->setLayoutDirection(Qt::LeftToRight);
        product_comboBox[i]->setStyleSheet(QLatin1String("QComboBox {\n"
                                                         "background-color: rgb(107, 107, 107);\n"
                                                         "color: rgb(255, 255, 255);\n"
                                                         "border:1px solid #FFFFFF ;}\n"
                                                         "QComboBox::drop-down {\n"
                                                         "subcontrol-origin: padding;\n"
                                                         "subcontrol-position: top right;\n"
                                                         "width:70px;\n"
                                                         "\n"
                                                         "}"));
        product_comboBox[i]->setEditable(false);
        product_comboBox[i]->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        product_comboBox[i]->clear();

        QStringList products=MWdatabase->getProductname().split("#");
        products.removeLast();
        product_comboBox[i]->insertItems(0,products);




        product_comboBox_label[i]->setVisible(false);
        product_comboBox[i]->setVisible(false);
    }


    for(int i=0;i<Number_of_Nozzle;i++)
    {
        product_comboBox_label[i]->setVisible(true);
        product_comboBox[i]->setVisible(true);
    }

    adress_label = new QLabel(devicesettingtab);
    adress_label->setObjectName(QStringLiteral("adress_label"));
    adress_label->setGeometry(QRect(10, 10, 151, 61));
    adress_label->setFont(font16);
    adress_label->setText(deviceAdress_String);


    adress_spinBox = new QSpinBox(devicesettingtab);
    adress_spinBox->setObjectName(QStringLiteral("adress_spinBox"));
    adress_spinBox->setGeometry(QRect(171, 10, 151, 61));
    QFont fontspinBox;
    fontspinBox.setPointSize(30);
    fontspinBox.setItalic(true);
    adress_spinBox->setFont(fontspinBox);
    adress_spinBox->setStyleSheet(QLatin1String("QSpinBox {border:1px solid #FFFFFF; color: black;}\n"
                                                "QSpinBox::up-button { width:70px; }\n"
                                                "QSpinBox::down-button { width:70px; };\n"
                                                ""));
    adress_spinBox->setWrapping(true);
    adress_spinBox->setAlignment(Qt::AlignCenter);
    adress_spinBox->setReadOnly(false);
    adress_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    adress_spinBox->setMaximum(30);
    adress_spinBox->setMinimum(1);
    adress_spinBox->setValue(1);
    adress_spinBox->setFocusPolicy(Qt::NoFocus);

    Nozzle_Num_label = new QLabel(devicesettingtab);
    Nozzle_Num_label->setObjectName(QStringLiteral("Nozzle_Num_label"));
    Nozzle_Num_label->setGeometry(QRect(10, 91, 151, 71));
    Nozzle_Num_label->setFont(font16);
    Nozzle_Num_label->setText(pumpNozzleNumber_String);


    Nozzle_Num_spinBox = new QSpinBox(devicesettingtab);
    Nozzle_Num_spinBox->setObjectName(QStringLiteral("Nozzle_Num_spinBox"));
    Nozzle_Num_spinBox->setGeometry(QRect(171, 91, 151, 61));
    Nozzle_Num_spinBox->setFont(fontspinBox);
    Nozzle_Num_spinBox->setStyleSheet(QLatin1String("QSpinBox {border:1px solid #FFFFFF; color: black;}\n"
                                                    "QSpinBox::up-button { width:70px; }\n"
                                                    "QSpinBox::down-button { width:70px; };\n"
                                                    ""));
    Nozzle_Num_spinBox->setWrapping(true);
    Nozzle_Num_spinBox->setAlignment(Qt::AlignCenter);
    Nozzle_Num_spinBox->setReadOnly(false);
    Nozzle_Num_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    Nozzle_Num_spinBox->setMaximum(6);
    Nozzle_Num_spinBox->setMinimum(1);
    Nozzle_Num_spinBox->setValue(1);
    Nozzle_Num_spinBox->setFocusPolicy(Qt::NoFocus);

    connect(Nozzle_Num_spinBox,SIGNAL(valueChanged(int)),this,SLOT(Nozzle_Num_Changed(int)));




    ////
    CardR_Address_label = new QLabel(devicesettingtab);
    CardR_Address_label->setObjectName(QStringLiteral("CardR_Address_label"));
    CardR_Address_label->setGeometry(QRect(10, 172, 151, 71));
    CardR_Address_label->setFont(font16);
    CardR_Address_label->setText(cardReaderAdress_String);



    CardR_Address_spinBox = new QSpinBox(devicesettingtab);
    CardR_Address_spinBox->setObjectName(QStringLiteral("CardR_Address_spinBox"));
    CardR_Address_spinBox->setGeometry(QRect(171, 172, 151, 61));
    CardR_Address_spinBox->setFont(fontspinBox);
    CardR_Address_spinBox->setStyleSheet(QLatin1String("QSpinBox {border:1px solid #FFFFFF; color: black;}\n"
                                                       "QSpinBox::up-button { width:70px; }\n"
                                                       "QSpinBox::down-button { width:70px; };\n"
                                                       ""));
    CardR_Address_spinBox->setWrapping(true);
    CardR_Address_spinBox->setAlignment(Qt::AlignCenter);
    CardR_Address_spinBox->setReadOnly(false);
    CardR_Address_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    CardR_Address_spinBox->setMaximum(99);
    CardR_Address_spinBox->setMinimum(1);
    CardR_Address_spinBox->setValue(1);
    CardR_Address_spinBox->setFocusPolicy(Qt::NoFocus);



    Volumedotindex_label = new QLabel(devicesettingtab);
    Volumedotindex_label->setObjectName(QStringLiteral("Volumedotindex_label"));
    Volumedotindex_label->setGeometry(QRect(402, 10, 151, 61));
    Volumedotindex_label->setFont(font16);
    Volumedotindex_label->setText(litreDecPoint_String);


    Volumedotindex_spinBox = new QSpinBox(devicesettingtab);
    Volumedotindex_spinBox->setObjectName(QStringLiteral("Volumedotindex_spinBox"));
    Volumedotindex_spinBox->setGeometry(QRect(563, 10, 151, 61));
    Volumedotindex_spinBox->setFont(fontspinBox);
    Volumedotindex_spinBox->setStyleSheet(QLatin1String("QSpinBox {border:1px solid #FFFFFF; color: black;}\n"
                                                        "QSpinBox::up-button { width:70px; }\n"
                                                        "QSpinBox::down-button { width:70px; };\n"
                                                        ""));
    Volumedotindex_spinBox->setWrapping(true);
    Volumedotindex_spinBox->setAlignment(Qt::AlignCenter);
    Volumedotindex_spinBox->setReadOnly(false);
    Volumedotindex_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    Volumedotindex_spinBox->setMaximum(5);
    Volumedotindex_spinBox->setMinimum(1);
    Volumedotindex_spinBox->setValue(1);
    Volumedotindex_spinBox->setFocusPolicy(Qt::NoFocus);


    Amountdotindex_label = new QLabel(devicesettingtab);
    Amountdotindex_label->setObjectName(QStringLiteral("Amountdotindex_label"));
    Amountdotindex_label->setGeometry(QRect(402, 91, 151, 61));
    Amountdotindex_label->setFont(font16);
    Amountdotindex_label->setText(amountDecPoint_String);


    Amountdotindex_spinBox = new QSpinBox(devicesettingtab);
    Amountdotindex_spinBox->setObjectName(QStringLiteral("Amountdotindex_spinBox"));
    Amountdotindex_spinBox->setGeometry(QRect(563, 91, 151, 61));
    Amountdotindex_spinBox->setFont(fontspinBox);
    Amountdotindex_spinBox->setStyleSheet(QLatin1String("QSpinBox {border:1px solid #FFFFFF; color: black;}\n"
                                                        "QSpinBox::up-button { width:70px; }\n"
                                                        "QSpinBox::down-button { width:70px; };\n"
                                                        ""));
    Amountdotindex_spinBox->setWrapping(true);
    Amountdotindex_spinBox->setAlignment(Qt::AlignCenter);
    Amountdotindex_spinBox->setReadOnly(false);
    Amountdotindex_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    Amountdotindex_spinBox->setMaximum(5);
    Amountdotindex_spinBox->setMinimum(1);
    Amountdotindex_spinBox->setValue(1);
    Amountdotindex_spinBox->setFocusPolicy(Qt::NoFocus);


    Upricedotindex_label = new QLabel(devicesettingtab);
    Upricedotindex_label->setObjectName(QStringLiteral("Upricedotindex_label"));
    Upricedotindex_label->setGeometry(QRect(402, 172, 151, 61));
    Upricedotindex_label->setFont(font16);
    Upricedotindex_label->setText(uPriceDecPoint_String);


    Upricedotindex_spinBox = new QSpinBox(devicesettingtab);
    Upricedotindex_spinBox->setObjectName(QStringLiteral("Upricedotindex_spinBox"));
    Upricedotindex_spinBox->setGeometry(QRect(563, 172, 151, 61));
    Upricedotindex_spinBox->setFont(fontspinBox);
    Upricedotindex_spinBox->setStyleSheet(QLatin1String("QSpinBox {border:1px solid #FFFFFF; color: black;}\n"
                                                        "QSpinBox::up-button { width:70px; }\n"
                                                        "QSpinBox::down-button { width:70px; };\n"
                                                        ""));
    Upricedotindex_spinBox->setWrapping(true);
    Upricedotindex_spinBox->setAlignment(Qt::AlignCenter);
    Upricedotindex_spinBox->setReadOnly(false);
    Upricedotindex_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    Upricedotindex_spinBox->setMaximum(5);
    Upricedotindex_spinBox->setMinimum(1);
    Upricedotindex_spinBox->setValue(1);
    Upricedotindex_spinBox->setFocusPolicy(Qt::NoFocus);



    kmsor_label = new QLabel(saleSettingTab);
    kmsor_label->setObjectName(QStringLiteral("kmsor_label"));
    kmsor_label->setGeometry(QRect(380, 135, 151, 61));
    kmsor_label->setFont(font16);
    kmsor_label->setText(askKm_String);

    kmsor_check = new QCheckBox(saleSettingTab);
    kmsor_check->setObjectName(QStringLiteral("kmsor_check"));
    kmsor_check->setGeometry(QRect(510, 135, 151, 61));
    kmsor_check->setFocusPolicy(Qt::NoFocus);
    kmsor_check->setStyleSheet(QLatin1String("QCheckBox {\n"
                                             "border:0px;\n"
                                             "}\n"
                                             "QCheckBox::indicator{\n"
                                             "width: 50px;\n"
                                             "height:50px;\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:1px solid #FFFFFF;\n"
                                             "border-radius:5px;\n"
                                             "}\n"
                                             "QCheckBox::indicator:checked {\n"
                                             "	image: url(:/new/prefix1/icons/checkmark.png);\n"
                                             "}\n"
                                             "QCheckBox {\n"
                                             "	color: rgb(255, 255, 255);\n"
                                             "}\n"
                                             ""));
    //kmsor_check->setChecked(false);
    kmsor_check->setTristate(false);
    kmsor_check->setText(QString());


    QFont font26;
    font26.setPointSize(10);
    receipt_1_label = new QLabel(receipttab);
    receipt_1_label->setObjectName(QStringLiteral("receipt_1_label"));
    receipt_1_label->setGeometry(QRect(30, 20, 91, 30));
    receipt_1_label->setFont(font1);
    receipt_1_label->setStyleSheet(QStringLiteral("border:0px;"));
    receipt_2_label = new QLabel(receipttab);
    receipt_2_label->setObjectName(QStringLiteral("receipt_2_label"));
    receipt_2_label->setGeometry(QRect(30, 70, 91, 30));
    receipt_2_label->setFont(font1);
    receipt_2_label->setStyleSheet(QStringLiteral("border:0px;"));
    receipt_3_label = new QLabel(receipttab);
    receipt_3_label->setObjectName(QStringLiteral("receipt_3_label"));
    receipt_3_label->setGeometry(QRect(410, 20, 91, 30));
    receipt_3_label->setFont(font1);
    receipt_3_label->setStyleSheet(QStringLiteral("border:0px;"));
    receipt_4_label = new QLabel(receipttab);
    receipt_4_label->setObjectName(QStringLiteral("receipt_4_label"));
    receipt_4_label->setGeometry(QRect(410, 70, 91, 30));
    receipt_4_label->setFont(font1);
    receipt_4_label->setStyleSheet(QStringLiteral("border:0px;"));

    sifre_1_label = new QLabel(sifretab);
    sifre_1_label->setObjectName(QStringLiteral("receipt_1_label"));
    sifre_1_label->setGeometry(QRect(20, 20, 180, 50));
    sifre_1_label->setFont(font1);
    sifre_1_label->setStyleSheet(QStringLiteral("border:0px;"));

    sifre_2_label = new QLabel(sifretab);
    sifre_2_label->setObjectName(QStringLiteral("receipt_2_label"));
    sifre_2_label->setGeometry(QRect(20, 90, 180, 50));
    sifre_2_label->setFont(font1);
    sifre_2_label->setStyleSheet(QStringLiteral("border:0px;"));

    usc_ip_label = new QLabel(sifretab);
    usc_ip_label->setObjectName(QStringLiteral("receipt_1_label"));
    usc_ip_label->setGeometry(QRect(410, 20, 180, 50));
    usc_ip_label->setFont(font1);
    usc_ip_label->setStyleSheet(QStringLiteral("border:0px;"));
    usc_port_label = new QLabel(sifretab);
    usc_port_label->setObjectName(QStringLiteral("receipt_2_label"));
    usc_port_label->setGeometry(QRect(410, 90, 180, 50));
    usc_port_label->setFont(font1);
    usc_port_label->setStyleSheet(QStringLiteral("border:0px;"));

    gateway_label = new QLabel(sifretab);
    gateway_label->setObjectName(QStringLiteral("receipt_2_label"));
    gateway_label->setGeometry(QRect(410, 160, 180, 50));
    gateway_label->setFont(font1);
    gateway_label->setStyleSheet(QStringLiteral("border:0px;"));

    DinamikIP_check = new QCheckBox(sifretab);
    DinamikIP_check->setObjectName(QStringLiteral("staticIP_check"));
    DinamikIP_check->setGeometry(QRect(200, 20, 151, 61));
    DinamikIP_check->setFocusPolicy(Qt::NoFocus);
    DinamikIP_check->setStyleSheet(QLatin1String("QCheckBox {\n"
                                             "border:0px;\n"
                                             "}\n"
                                             "QCheckBox::indicator{\n"
                                             "width: 50px;\n"
                                             "height:50px;\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:1px solid #FFFFFF;\n"
                                             "border-radius:5px;\n"
                                             "}\n"
                                             "QCheckBox::indicator:checked {\n"
                                             "	image: url(:/new/prefix1/icons/checkmark.png);\n"
                                             "}\n"
                                             "QCheckBox {\n"
                                             "	color: rgb(255, 255, 255);\n"
                                             "}\n"
                                             ""));
    DinamikIP_check->setChecked(false);
  //  DinamikIP_check->setCheckState(Qt::Unchecked);
    DinamikIP_check->setTristate(false);
    DinamikIP_check->setText(QString());

    SabitIP_check = new QCheckBox(sifretab);
    SabitIP_check->setObjectName(QStringLiteral("dinamicIP_check"));
    SabitIP_check->setGeometry(QRect(200, 90, 151, 61));
    SabitIP_check->setFocusPolicy(Qt::NoFocus);
    SabitIP_check->setStyleSheet(QLatin1String("QCheckBox {\n"
                                             "border:0px;\n"
                                             "}\n"
                                             "QCheckBox::indicator{\n"
                                             "width: 50px;\n"
                                             "height:50px;\n"
                                             "background-color: rgb(0, 0, 0);\n"
                                             "border:1px solid #FFFFFF;\n"
                                             "border-radius:5px;\n"
                                             "}\n"
                                             "QCheckBox::indicator:checked {\n"
                                             "	image: url(:/new/prefix1/icons/checkmark.png);\n"
                                             "}\n"
                                             "QCheckBox {\n"
                                             "	color: rgb(255, 255, 255);\n"
                                             "}\n"
                                             ""));
    SabitIP_check->setChecked(false);
    SabitIP_check->setTristate(false);
    //SabitIP_check->setCheckState(Qt::Checked);
    SabitIP_check->setText(QString());


    receipt_1_lineEdit = new QLineEdit(receipttab);
    receipt_1_lineEdit->setObjectName(QStringLiteral("receipt_1_lineEdit"));
    receipt_1_lineEdit->setGeometry(QRect(140, 10, 250, 40));
    receipt_1_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border-radius:10px;"));
    receipt_1_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    receipt_1_lineEdit->setMaxLength(21);
    receipt_1_lineEdit->setFont(font21);
    receipt_1_lineEdit->setAlignment(Qt::AlignCenter);

    receipt_2_lineEdit = new QLineEdit(receipttab);
    receipt_2_lineEdit->setObjectName(QStringLiteral("receipt_2_lineEdit"));
    receipt_2_lineEdit->setGeometry(QRect(140, 66, 250, 40));
    receipt_2_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border-radius:10px;"));
    receipt_2_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    receipt_2_lineEdit->setMaxLength(21);
    receipt_2_lineEdit->setFont(font21);
    receipt_2_lineEdit->setAlignment(Qt::AlignCenter);

    receipt_3_lineEdit = new QLineEdit(receipttab);
    receipt_3_lineEdit->setObjectName(QStringLiteral("receipt_3_lineEdit"));
    receipt_3_lineEdit->setGeometry(QRect(520, 10, 251, 40));
    receipt_3_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border-radius:10px;"));
    receipt_3_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    receipt_3_lineEdit->setMaxLength(21);
    receipt_3_lineEdit->setFont(font21);
    receipt_3_lineEdit->setAlignment(Qt::AlignCenter);

    receipt_4_lineEdit = new QLineEdit(receipttab);
    receipt_4_lineEdit->setObjectName(QStringLiteral("receipt_4_lineEdit"));
    receipt_4_lineEdit->setGeometry(QRect(520, 66, 251, 40));
    receipt_4_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border-radius:10px;"));
    receipt_4_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    receipt_4_lineEdit->setMaxLength(21);
    receipt_4_lineEdit->setFont(font21);
    receipt_4_lineEdit->setAlignment(Qt::AlignCenter);



    usc_ip_lineedit = new QLineEdit(sifretab);
    usc_ip_lineedit->setObjectName(QStringLiteral("receipt_1_lineEdit"));
    usc_ip_lineedit->setGeometry(QRect(540, 30, 200, 40));
    usc_ip_lineedit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                 "color: rgb(255,255,255);\n"
                                                 "border-radius:10px;"));
    usc_ip_lineedit->setInputMethodHints(Qt::ImhUppercaseOnly);
    usc_ip_lineedit->setMaxLength(21);
    usc_ip_lineedit->setFont(font21);
    usc_ip_lineedit->setAlignment(Qt::AlignCenter);

    usc_port_lineedit = new QLineEdit(sifretab);
    usc_port_lineedit->setObjectName(QStringLiteral("receipt_2_lineEdit"));
    usc_port_lineedit->setGeometry(QRect(540, 100, 200, 40));
    usc_port_lineedit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                   "color: rgb(255,255,255);\n"
                                                   "border-radius:10px;"));
    usc_port_lineedit->setInputMethodHints(Qt::ImhUppercaseOnly);
    usc_port_lineedit->setMaxLength(21);
    usc_port_lineedit->setFont(font21);
    usc_port_lineedit->setAlignment(Qt::AlignCenter);

    subnetMax_lineedit = new QLineEdit(sifretab);
    subnetMax_lineedit->setObjectName(QStringLiteral("receipt_2_lineEdit"));
    subnetMax_lineedit->setGeometry(QRect(540, 170, 200, 40));
    subnetMax_lineedit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                   "color: rgb(255,255,255);\n"
                                                   "border-radius:10px;"));
    subnetMax_lineedit->setText(gateWay);
    subnetMax_lineedit->setInputMethodHints(Qt::ImhUppercaseOnly);
    subnetMax_lineedit->setMaxLength(21);
    subnetMax_lineedit->setFont(font21);
    subnetMax_lineedit->setAlignment(Qt::AlignCenter);

    sifre_1_lineEdit = new QLineEdit(sifretab);
    sifre_1_lineEdit->setObjectName(QStringLiteral("receipt_1_lineEdit"));
    sifre_1_lineEdit->setGeometry(QRect(180, 30, 200, 40));
    sifre_1_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                  "color: rgb(255,255,255);\n"
                                                  "border-radius:10px;"));
    sifre_1_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    sifre_1_lineEdit->setMaxLength(21);
    sifre_1_lineEdit->setFont(font21);
    sifre_1_lineEdit->setAlignment(Qt::AlignCenter);
    sifre_1_lineEdit->setVisible(false);

    sifre_2_lineEdit = new QLineEdit(sifretab);
    sifre_2_lineEdit->setObjectName(QStringLiteral("receipt_2_lineEdit"));
    sifre_2_lineEdit->setGeometry(QRect(180, 100, 200, 40));
    sifre_2_lineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                  "color: rgb(255,255,255);\n"
                                                  "border-radius:10px;"));
    numpadLineEdit = new QLineEdit(sifretab);
    numpadLineEdit->setObjectName(QStringLiteral("receipt_2_lineEdit"));
    numpadLineEdit->setGeometry(QRect(200, 15, 400, 60));
    numpadLineEdit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                  "color: rgb(255,255,255);\n"
                                                  "border-radius:10px;"));
    numpadLineEdit->setFont(font21);
    numpadLineEdit->setVisible(false);
    sifre_2_lineEdit->setInputMethodHints(Qt::ImhUppercaseOnly);
    sifre_2_lineEdit->setMaxLength(21);
    sifre_2_lineEdit->setFont(font21);
    sifre_2_lineEdit->setAlignment(Qt::AlignCenter);
    sifre_2_lineEdit->setVisible(false);


    receipt_2_pushbutton = new QPushButton(receipttab);
    receipt_2_pushbutton->setObjectName(QStringLiteral("receipt_2_pushbutton"));
    receipt_2_pushbutton->setGeometry(QRect(140, 66, 250, 40));
    receipt_2_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                      "color: rgb(255,255,255);\n"
                                                      "border:1px solid #FFFFFF;\n"
                                                      "border-radius:10px;"));
    receipt_1_pushbutton = new QPushButton(receipttab);
    receipt_1_pushbutton->setObjectName(QStringLiteral("receipt_1_pushbutton"));
    receipt_1_pushbutton->setGeometry(QRect(140, 10, 250, 40));
    receipt_1_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                      "color: rgb(255,255,255);\n"
                                                      "border:1px solid #FFFFFF;\n"
                                                      "border-radius:10px;"));
    receipt_3_pushbutton = new QPushButton(receipttab);
    receipt_3_pushbutton->setObjectName(QStringLiteral("receipt_3_pushbutton"));
    receipt_3_pushbutton->setGeometry(QRect(520, 10, 251, 40));
    receipt_3_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                      "color: rgb(255,255,255);\n"
                                                      "border:1px solid #FFFFFF;\n"
                                                      "border-radius:10px;"));
    receipt_4_pushbutton = new QPushButton(receipttab);
    receipt_4_pushbutton->setObjectName(QStringLiteral("receipt_4_pushbutton"));
    receipt_4_pushbutton->setGeometry(QRect(520, 66, 251, 40));
    receipt_4_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                      "color: rgb(255,255,255);\n"
                                                      "border:1px solid #FFFFFF;\n"
                                                      "border-radius:10px;"));

    usc_ip_pushbutton = new QPushButton(sifretab);
    usc_ip_pushbutton->setObjectName(QStringLiteral("receipt_2_pushbutton"));
    usc_ip_pushbutton->setGeometry(QRect(540, 30, 200, 40));
    usc_ip_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                   "color: rgb(255,255,255);\n"
                                                   "border:1px solid #FFFFFF;\n"
                                                   "border-radius:10px;"));
    usc_port_pushbutton = new QPushButton(sifretab);
    usc_port_pushbutton->setObjectName(QStringLiteral("receipt_1_pushbutton"));
    usc_port_pushbutton->setGeometry(QRect(540, 100, 200, 40));
    usc_port_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                     "color: rgb(255,255,255);\n"
                                                     "border:1px solid #FFFFFF;\n"
                                                     "border-radius:10px;"));

    subnetMask_pushbutton = new QPushButton(sifretab);
    subnetMask_pushbutton->setObjectName(QStringLiteral("receipt_1_pushbutton"));
    subnetMask_pushbutton->setGeometry(QRect(540, 170, 200, 40));
    subnetMask_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                     "color: rgb(255,255,255);\n"
                                                     "border:1px solid #FFFFFF;\n"
                                                     "border-radius:10px;"));


    sifre_2_pushbutton = new QPushButton(sifretab);
    sifre_2_pushbutton->setObjectName(QStringLiteral("receipt_2_pushbutton"));
    sifre_2_pushbutton->setGeometry(QRect(180, 100, 200, 40));
    sifre_2_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border:1px solid #FFFFFF;\n"
                                                    "border-radius:10px;"));
    sifre_2_pushbutton->setVisible(false);
    sifre_1_pushbutton = new QPushButton(sifretab);
    sifre_1_pushbutton->setObjectName(QStringLiteral("receipt_1_pushbutton"));
    sifre_1_pushbutton->setGeometry(QRect(180, 30, 200, 40));
    sifre_1_pushbutton->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24,0);\n"
                                                    "color: rgb(255,255,255);\n"
                                                    "border:1px solid #FFFFFF;\n"
                                                    "border-radius:10px;"));
    sifre_1_pushbutton->setVisible(false);


    tarih_sor->setText(date_String);
    saat_sor->setText(time_String);

    receipt_1_label->setText(QApplication::translate("MainWindow", "LINE 1:", 0));
    receipt_2_label->setText(QApplication::translate("MainWindow", "LINE 2:", 0));
    receipt_3_label->setText(QApplication::translate("MainWindow", "LINE 3:", 0));
    receipt_4_label->setText(QApplication::translate("MainWindow", "LINE 4:", 0));
    sifre_1_label->setText(menuPass_String);
    sifre_2_label->setText(manualAllowPassword_String);
    usc_ip_label->setText(QApplication::translate("MainWindow", "IP ADRES  :", 0));
    usc_port_label->setText(QApplication::translate("MainWindow", "NETMASK :", 0));
    gateway_label->setText(QApplication::translate("MainWindow", "GATEWAY  :", 0));
    receipt_2_pushbutton->setText(QString());
    receipt_1_pushbutton->setText(QString());
    receipt_3_pushbutton->setText(QString());
    receipt_4_pushbutton->setText(QString());
    sifre_1_pushbutton->setText(QString());
    sifre_2_pushbutton->setText(QString());
    saha_modu_pushbutton->setText(fieldMode_String);
    licance_pushbutton->setText(activateLicence_String);
    sd_kart_yedek_pushbutton->setText(backUpToSdCard_String);
    sd_kart_al_pushbutton->setText(takeFromSdCard_String);
    update_pushbutton->setText(updateNow_String);
    testmodestart->setText(startTest_String);
    access_test_label->setText(accesspoint_String);
    printer_test_label->setText(printer_String);
    cardreader_test_label->setText(cardReader_String);
    accesspoint_test_status->setText(QString());
    printer_test_status->setText(QString());
    cardreader_test_status->setText(QString());

    menu_tabWidget->setTabText(menu_tabWidget->indexOf(testmodetab),testMode_String);
    menu_tabWidget->setTabText(menu_tabWidget->indexOf(saleSettingTab),saleControl_String);

    menu_tabWidget->setTabText(menu_tabWidget->indexOf(datesettingtab),dateSetting_String);
    menu_tabWidget->setTabText(menu_tabWidget->indexOf(devicesettingtab),deviceSet_String);
    menu_tabWidget->setTabText(menu_tabWidget->indexOf(receipttab),receiptSet_String);
    menu_tabWidget->setTabText(menu_tabWidget->indexOf(producttab),productSet_String);
    //menu_tabWidget->setTabText(menu_tabWidget->indexOf(kmtab), QApplication::translate("MainWindow", "KM AYARI", 0));
    menu_tabWidget->setTabText(menu_tabWidget->indexOf(sifretab),networkSettings_String);


//    menu_tabWidget->setTabIcon(0,QIcon(":/new/prefix1/icons/cancel.png"));
//    menu_tabWidget->setIconSize(QSize(50,50));
    menu_tabWidget->setTabText(menu_tabWidget->indexOf(generalset), QApplication::translate("MainWindow", "ACC POINT", 0));

    connect(menu_tabWidget,SIGNAL(currentChanged(int)),this,SLOT(getTabNo()));
    connect(receipt_1_pushbutton,SIGNAL(released()),this,SLOT(receipt_1_pushbutton_clicked()));
    connect(receipt_2_pushbutton,SIGNAL(released()),this,SLOT(receipt_2_pushbutton_clicked()));
    connect(receipt_3_pushbutton,SIGNAL(released()),this,SLOT(receipt_3_pushbutton_clicked()));
    connect(receipt_4_pushbutton,SIGNAL(released()),this,SLOT(receipt_4_pushbutton_clicked()));
    connect(sifre_1_pushbutton,SIGNAL(released()),this,SLOT(sifre_1_pushbutton_clicked()));
    connect(sifre_2_pushbutton,SIGNAL(released()),this,SLOT(sifre_2_pushbutton_clicked()));
    connect(setDate_pushbutton,SIGNAL(released()),this,SLOT(setDate_pushbutton_clicked()));
   // connect(nextTabPushbutton,SIGNAL(released()),this,SLOT(nexttabPushbutton_clicked()));
    //connect(previusTabPushbutton,SIGNAL(released()),this,SLOT(previusTabPushbutton_clicked()));
    connect(usc_ip_pushbutton,SIGNAL(released()),this,SLOT(usc_ip_pushbutton_clicked()));
    connect(DinamikIP_check,SIGNAL(clicked()),this,SLOT(DinamikIP_check_clicked()));
    connect(SabitIP_check,SIGNAL(clicked()),this,SLOT(SabitIP_check_clicked()));
    connect(usc_port_pushbutton,SIGNAL(released()),this,SLOT(usc_port_pushbutton_clicked()));
    connect(subnetMask_pushbutton,SIGNAL(released()),this,SLOT(subnetMask_pushbutton_clicked()));
    connect(menu_enter,SIGNAL(released()),this,SLOT(menu_enter_button_Clicked()));
    connect(menu_cancel,SIGNAL(released()),this,SLOT(menu_cancel_button_Clicked()));
    connect(licance_pushbutton,SIGNAL(released()),this,SLOT(licance_pushbutton_Clicked()));
    connect(saha_modu_pushbutton,SIGNAL(released()),this,SLOT(saha_modu_pushbutton_Clicked()));
    connect(sd_kart_yedek_pushbutton,SIGNAL(released()),this,SLOT(sd_kart_yedek_pushbutton_Clicked()));
    connect(sd_kart_al_pushbutton,SIGNAL(released()),this,SLOT(sd_kart_al_pushbutton_Clicked()));
    connect(update_pushbutton,SIGNAL(released()),this,SLOT(update_from_flash()));
    connect(testmodestart,SIGNAL(released()),this,SLOT(testmodu_clicked()));
    connect(access_point_read_pushbutton,SIGNAL(released()),this,SLOT(access_point_read_clicked()));
    connect(access_point_write_pushbutton,SIGNAL(released()),this,SLOT(access_point_write_clicked()));
    connect(tts_active_pushbutton,SIGNAL(released()),this,SLOT(tts_active_pushbutton_clicked()));
    connect(otomasyon_adres_pushbutton,SIGNAL(released()),this,SLOT(otomasyon_adres_pushbutton_clicked()));
    connect(nozzle_adres_pushbutton,SIGNAL(released()),this,SLOT(nozzle_adres_pushbutton_clicked()));
    connect(channel_pushbutton,SIGNAL(released()),this,SLOT(channel_pushbutton_clicked()));
    connect(pause_time_pushbutton,SIGNAL(released()),this,SLOT(pause_time_pushbutton_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Mwindowaction()
{
    Mwindowtimer->stop();
    BuzzerOFF();
    Mwindowlock=false;
}
void MainWindow::mysleep(unsigned int delay)
{
    unsigned long dd;

    for(dd=0;dd<=30*delay;dd++)
    {
        dd++;
    }

}

void MainWindow::clrcBuzzerON()
{   saat_1_lineEdit->setDateTime(QDateTime::currentDateTime());
    if(buzzer_on_flg==1)
    {
        buzzer_on_flg=0;
        if(test) qDebug()<<"BuzzerONN";
        system("echo 1 > /sys/class/gpio/gpio141/value");
        mysleep(800);
        system("echo 0 > /sys/class/gpio/gpio141/value");
    }

}

void MainWindow::BuzzerON()
{
    system("echo 1 > /sys/class/gpio/gpio141/value");
}

void MainWindow::BuzzerOFF()
{
    system("echo 0 > /sys/class/gpio/gpio141/value");
}



void MainWindow::update_from_flash()
{

    if(Mwindowlock==false)
    {
        BuzzerON();
        int update_status=0;
        QDir dir("/media/root/");
        QFileInfoList list_usb=dir.entryInfoList();
        for(int i=0;i<list_usb.size();i++)
        {
            QString file_name=list_usb.at(i).fileName();
            if(file_name=="MEPSAN")
            {
                update_status=1;
                QDir dir2("/media/root/MEPSAN/");
                list_usb=dir2.entryInfoList();
                for(int j=0;j<list_usb.size();j++)
                {
                    QString file_name=list_usb.at(j).fileName();
                    if(file_name=="PSM")
                    {
                        system("rm -f /exec/PSM");
                        system("cp -f /media/root/MEPSAN/PSM /exec/");
                        system("chmod 777 /exec/*");
                        update_status=2;
                        break;
                    }
                }
                break;
            }
        }
        //update_status=1;
        if(update_status==2)
        {

            menu_frame->setVisible(false);
            //tabframe->setVisible(false);
            numpadframe->setVisible(false);
            keyboard_frame->setVisible(false);
            main_frame->setVisible(true);
            top_frame->setVisible(true);
            menu_frame->close();
            menutabwidgetindex=0;
            password_frame->setVisible(false);
            paswrd_label->setVisible(false);
            pmenter_frame->setVisible(false);
            warning_label->setText(updateSuccessful_String);
            status_label->setText(systemInitializing_String);
            LCD_status=31;
        }
        else if(update_status==1)
        {
            menu_frame->setVisible(false);
            numpadframe->setVisible(false);
            keyboard_frame->setVisible(false);
            main_frame->setVisible(true);
            top_frame->setVisible(true);
            menu_frame->close();
            menutabwidgetindex=0;
            password_frame->setVisible(false);
            pmenter_frame->setVisible(false);
            warning_label->setText(updateFailed_String);
            status_label->setText(noNewUpdatesFound_String);
            LCD_status=32;
        }
        else
        {
            menu_frame->setVisible(false);
            numpadframe->setVisible(false);
            keyboard_frame->setVisible(false);
            main_frame->setVisible(true);
            top_frame->setVisible(true);
            menu_frame->close();
            menutabwidgetindex=0;
            password_frame->setVisible(false);
            pmenter_frame->setVisible(false);
            warning_label->setText(updateFailed_String);
            status_label->setText(updateDeviceNotFound_String);
            LCD_status=32;
        }
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}



void MainWindow::show_time()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString txt2=date.toString("dd.MM.yyyy") ;
    QString txt = time.toString("hh:mm:ss");
    QString txt3= time.toString("ss");
    watchdog_count++;
    if(watchdog_count>=3)
    {
        //system("echo gokturk > /dev/watchdog");
        watchdog_count=0;
        if(nozzle_configuration==false)
        {
            if(MWdatabase->NozzleValues())
            {
                settings.setlog(" LOAD_SETTING Config_values_Read_nozzle");
                nozzle_configuration=true;
            }
            else
            {
                settings.setlog(" LOAD_SETTING Config_values_nozzle_error");
                nozzle_configuration=false;
            }
        }
        if(setting_configuration==false)
        {
            if(MWdatabase->SettingsValues())
            {
                settings.setlog(" LOAD_SETTING Config_values_Read_setting");
                setting_configuration=true;
            }
            else
            {
                settings.setlog(" LOAD_SETTING Config_values_settings_error");
                setting_configuration=false;
            }
        }
        if(nozzle_configuration==true && setting_configuration==true)
        {
            config_error=false;
            if(Nozzle_up_flg==0)
                settings.setPriceStatus(pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].unitprice.toHex(),0);
        }
        else
        {
            config_error=true;
        }
    }
    if(test) qDebug()<<"--------------wait_km_flg-----------"<<wait_km_flg;
    if(test) qDebug()<<QString::number(wait_km_count)<<"  "<<QString::number(wait_km_second);
    if(wait_km_flg==true)
    {
        wait_km_count++;
        if(wait_km_count>=wait_km_second)
        {
            if(test) qDebug()<<"---------km girilmedi----------";
            wait_km_flg=false;
            wait_km_count=0;
            pump[pumpNo].status=4;
            LCD_status=5;
            LCD_refresh_flg=1;
            if((USC_is_exist==false) ||(USC_com_error_flg==true) ) ///sonradann
            {
                Paid_Confirm=true;
                Paid_Confrim_Time=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                Paid_Confrim_Time.prepend("20");
                Paid_Fiscal_Time=Paid_Confrim_Time;
                if(test)qDebug()<<"Paid_Confrim_Time"<<Paid_Confrim_Time;
                settings.setlog("SATIS_DEBUG USC-FALSE-Paid_Confrim "+Paid_Confrim_Time);
            }
        }
    }

    saat_lineEdit->setText(txt);
    //tarih_lineEdit->setText(QString::number(USC_com_count));
    tarih_lineEdit->setText(txt2);
    net_logo->setVisible(true);
    if(connection_is_exist==false){}
    else
    {
        net_logo->setIcon(QIcon(":/new/prefix1/icons/net-active.png"));
        if(txt3.toInt()%2==0){net_logo->setVisible(false);}
    }
    if(test_modu)
    {
        if(clrc_check_flg)
        {
            cardreader_test_status->setIcon(icon9);
        }
        else
        {
            cardreader_test_status->setIcon(iconcancel);
        }
        if(rfid_check_flg)
        {
            accesspoint_test_status->setIcon(icon9);
        }
        else
        {
            accesspoint_test_status->setIcon(iconcancel);
        }
        if(printer_check_flg)
        {
            printer_test_status->setIcon(icon9);
        }
        else
        {
            printer_test_status->setIcon(iconcancel);
        }
    }
    if(LCD_status==31)
    {
        updateisstarting++;
        if(updateisstarting>=4)
        {
            system("shutdown -r now");
        }
    }
    if(LCD_status==32)
    {
        updateisstarting++;
        if(updateisstarting>=4)
        {
            if(licenceCheck==0)
            {
                LCD_status=1;
                updateisstarting=0;
                status_label->setText(saleToReady_String);
                warning_label->clear();
            }
            else
            {
                LCD_status=13;
                updateisstarting=0;
                status_label->setText(invalidLicence_String);
                warning_label->clear();
            }
        }
    }
    if(LCD_status==35)
    {
        passwrdLine->setEchoMode(QLineEdit::Normal);
        QFont passwrdFont;
        passwrdFont.setPointSize(30);
        passwrdFont.setBold(true);
        passwrdLine->setFont(passwrdFont);
        main_frame->setVisible(false);
        top_frame->setVisible(false);
        plate_frame->setVisible(false);
        keyboard_frame->setVisible(false);
        km_frame->setVisible(false);
        paswrd_label->setGeometry(QRect(105, 5, 570, 60));
        paswrd_label->setText(enterDateTime_String);

        paswrd_label->setStyleSheet(QLatin1String("background-color: rgb(172, 20, 20);\n"
                                                  "color: rgb(255, 255, 255);\n"
                                                  "border:1px solid;\n"
                                                  "border-radius:5px;\n"
                                                  "border-color: rgb(24, 24, 24);"));
        password_frame->setVisible(true);
        numpadframe->setGeometry(0,130,801,350);
        numpadframe->setVisible(true);
        password_frame->raise();
        numpadframe->raise();
        keyboardcomingflag=0;
        cannotchange_frame=true;
    }

    if(read_configuration_success==1)
    {
        access_point_info_label->setText(accesspointSettingsReadingSuccessful_String);
        otomasyon_adres_label->setText(otoAdr_String+read_acc_auto_adr);
        nozzle_adres_label->setText(nozzAdr_String+read_acc_nozzle_adr);
        channel_label->setText(channel1_String+read_acc_channel);
        pause_time_label->setText(stopZ1_String+pause_time_lineEdit->text());
        otomasyon_adres_lineEdit->setText(read_acc_auto_adr);
        nozzle_adres_lineEdit->setText(read_acc_nozzle_adr);
        channel_lineEdit->setText(read_acc_channel);
        pause_time_lineEdit->setText(QString::number(tts_pause_count));
        read_configuration_success=0;
    }
    else if(read_configuration_success==2)
    {
        access_point_info_label->setText(accesspointSettingsReadingFailed_String);
        otomasyon_adres_label->setText(otoAdr1_String);
        nozzle_adres_label->setText(nozAdr1_String);
        channel_label->setText(channel1_String);
        pause_time_label->setText(channel1_String+pause_time_lineEdit->text());
        read_configuration_success=0;
    }
    if(write_configuration_success==1)
    {
        access_point_info_label->setText(accesspointSettingsUploadSuccessful_String);
        write_config_mesaj=0;
        read_configuration=true;
        write_configuration_success=0;

    }
    else if(write_configuration_success==2)
    {
        access_point_info_label->setText(accesspointSettingsUploadFailed_String);
        read_configuration=true;
        write_configuration_success=0;
        write_config_mesaj=0;
    }
    if(write_config_mesaj>5 && read_configuration)
    {
        access_point_info_label->setText(accesspointSettingsReadingPleaseWait_String);
    }
    else write_config_mesaj++;


}


void MainWindow::Nozzle_Num_Changed(int line)
{
    for(int i=0;i<7;i++)
    {
        product_comboBox_label[i]->setVisible(false);
        product_comboBox[i]->setVisible(false);
    }
    for(int i=0;i<line;i++)
    {
        product_comboBox_label[i]->setVisible(true);
        product_comboBox[i]->setVisible(true);
    }


}

void MainWindow::net_logo_clicked()
{
    if(Mwindowlock==false)
    {
        QString wParameter = " -w";
        QString pingWaitTime = " 1";
        process.start("sh",QStringList()<<"-c"<<"ip route | grep default");
        process.waitForFinished();
        QString output(process.readAllStandardOutput());
        if(output.length()>0){
            QStringList splitProcess = output.split(" ");
            qDebug()<<output<<splitProcess.at(2);
            QString gateway=splitProcess.at(2);
            QString pingTo = "ping "+gateway+wParameter+pingWaitTime;
            pingProcess = new QProcess;
            int exitCode = pingProcess->execute(pingTo);
            qDebug()<<exitCode<<pingTo;
            if(exitCode==1){
                Static_ip="OFFLINE";
            }
            else{
                getMacAddress();
            }
        } else {
            Static_ip="OFFLINE";
        }
        BuzzerON();
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
        //QSettings settings(SettingsFile, QSettings::IniFormat);
        version=settings.value("Myversion").toString();
        reply = new QMessageBox(this);
        reply->setStyleSheet("background: grey; color: white; font-size:20px; width:330px; height:60px; ");
        //reply->setGeometry(300,190,400,200);
        reply->setText(ipAdress_String+" : "+Static_ip+"\n\n"+version_String+" : "+version+"\n\n"+macAdress_String+" : "+macAddress);
        reply->setStandardButtons(QMessageBox::Cancel);
        reply->setWindowFlags(Qt::FramelessWindowHint);

        int ret=reply->exec();

        if(ret==QMessageBox::Cancel)
        {
            reply->close();
            BuzzerON();
        }

        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}
void MainWindow::locwiz_Clicked(){
    if(Mwindowlock==false)
    {
        reply2 = new QMessageBox(this);
        yesButton=new QPushButton;
        yesButton=reply2->addButton(acceptPopup_String,QMessageBox::ActionRole);
//        QString nParameter = " -n";
//        QString pingCount = " 1";
        QString wParameter = " -w";
        QString pingWaitTime = " 1";

        process.start("sh",QStringList()<<"-c"<<"ip route | grep default");
        process.waitForFinished();
        QString output(process.readAllStandardOutput());
        if(output.length()>0){
            QStringList splitProcess = output.split(" ");
            qDebug()<<output<<splitProcess.at(2);
            QString gateway=splitProcess.at(2);

            QString pingTo = "ping "+gateway+wParameter+pingWaitTime;
            pingProcess = new QProcess;
            int exitCode = pingProcess->execute(pingTo);
            qDebug()<<exitCode<<pingTo;
            if(exitCode==0){
                yesButton->setEnabled(true);

            }
            else{
                yesButton->setEnabled(false);
            }
        } else {
            yesButton->setEnabled(false);
        }

    BuzzerON();
    Mwindowlock=true;
    Mwindowtimer->start(locktimer);


    reply2->setStyleSheet("border:1px; border-radius:5px; background-color: lightblue; color: black; font-size:25px; width:200px; height:70px; ");
    //reply->setGeometry(300,190,400,200);
    reply2->setText(textPopup_String);
    reply2->setIconPixmap(QPixmap(":/new/prefix1/icons/looc.png").scaledToWidth(100));
    QPushButton *cancelButton=reply2->addButton(cancelPopup_String,QMessageBox::ActionRole);
    QPixmap pixmap(":/new/prefix1/icons/checked.png");
    yesButton->setIcon(pixmap);
    QPixmap pixmap2(":/new/prefix1/icons/cancel.png");
    cancelButton->setIcon(pixmap2);
    yesButton->setStyleSheet("background: grey  ; color: green  ;border:1px solid #FFFFFF; border-radius:5px;");
    cancelButton->setStyleSheet("background: grey  ; color: red  ;border:1px solid #FFFFFF; border-radius:5px;");
    reply2->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint);

    reply2->exec();

    tankLevel = QSqlDatabase::database("tankLevel");
    if(reply2->clickedButton()==cancelButton)
    {
        reply2->close();
        BuzzerON();
    }
    if(reply2->clickedButton()==yesButton){
        system("echo 3 > /proc/sys/vm/drop_caches");
        system("sync");


        if(!tankLevel.open())
        {
            settings.setlog("DATABASE_DEBUG constructor-Error: Connection with database fail");
        }
        else
        {

            settings.setlog("DATABASE_DEBUG Databe:constructor Connection ok");
            QSqlQuery query(tankLevel);
            query.exec("PRAGMA journal_mode=WAL");
            query.prepare("SELECT COUNT(*) FROM TANK");
            query.exec();
            query.first();
            int level=query.value(0).toInt();

            if(query.exec())
                qDebug()<<level<<query.lastError();

            if(level>0){

               //openChromium(SettingFile->readSetting("appToFull").toString());
                openChromium(Static_ip);


            }
            else{
//                openChromium(SettingFile->readSetting("appToEmpty").toString());
                openChromium(Static_ip);
            }
            tankLevel.commit();
            tankLevel.close();

            }

        }

    Mwindowlock=true;
    Mwindowtimer->start(locktimer);

    }

}
void MainWindow::openChromium(QString goTo){
    qDebug()<<"opened";
    if(SettingFile->readSetting("browserStart").toString()=="true"){
        browserStart="chromium-browser --no-sandbox";
    }
    else{
        browserStart=" ";
    }

    if(SettingFile->readSetting("keyboard").toString()=="true"){
        keyboard=" ";
    }
    else{
        keyboard=" --disable-extensions";
    }

    if(SettingFile->readSetting("kioskMode").toString()=="true"){
        kioskMode=" --kiosk";
    }
    else{
        kioskMode=" --disable-statusbar";
    }
    if(SettingFile->readSetting("noNotifications").toString()=="true"){
        noNotifications=" --disable-notifications";
    }
    else{
        noNotifications=" ";
    }
    if(SettingFile->readSetting("touch").toString()=="true"){
        touch=" --touch-devices=";
    }
    else{
        touch=" ";
    }
    if(SettingFile->readSetting("fullScreen").toString()=="true"){
        fullScreen=" --start-fullscreen";
    }
    else{
        fullScreen=" --start-maximized";
    }

    if(SettingFile->readSetting("noErrorDialogs").toString()=="true"){
        noErrorDialogs=" --noerrdialogs";
    }
    else{
        noErrorDialogs="";
    }

    if(SettingFile->readSetting("disableTranslate").toString()=="true"){
        disableTranslate=" --disable-translate --disable-features=TranslateUI";
    }
    else{
        disableTranslate="";
    }

    if(SettingFile->readSetting("fastStart").toString()=="true"){
        fastStart=" --fast --fast-start";
    }
    else{
        fastStart="";
    }

    if(SettingFile->readSetting("noInfoBar").toString()=="true"){
        noInfoBar=" --disable-infobars";
    }
    else{
        noInfoBar="";
    }

    if(SettingFile->readSetting("app").toString()=="true"){
        app=" --app=http://";
    }
    else{
        app="";
    }

    if(SettingFile->readSetting("incognito").toString()=="true"){
        incognito=" --incognito";
    }
    else{
        incognito="";
    }

    if(SettingFile->readSetting("disableGpu").toString()=="true"){
        disableGpu=" --disable-gpu";
    }
    else{
        disableGpu="";
    }

    if(SettingFile->readSetting("disableD3d11").toString()=="true"){
        disableD3d11=" --disable-d3d11";
    }
    else{
        disableD3d11="";
    }

    if(SettingFile->readSetting("disableLocalStorage").toString()=="true"){
        disableLocalStorage=" --disable-local-storage";
    }
    else{
        disableLocalStorage="";
    }

    if(SettingFile->readSetting("disableOffnePages").toString()=="true"){
        disableOffnePages=" --disable-offne-pages";
    }
    else{
        disableOffnePages="";
    }
    if(SettingFile->readSetting("disablePluginPowerSaver").toString()=="true"){
        disablePluginPowerSaver=" --disable-plugin-power-saver";
    }
    else{
        disablePluginPowerSaver="";
    }
    if(SettingFile->readSetting("disableSync").toString()=="true"){
        disableSync=" --disable-sync";
    }
    else{
        disableSync="";
    }
    if(SettingFile->readSetting("disableWebgl").toString()=="true"){
        disableWebgl=" --disable-webgl";
    }
    else{
        disableWebgl="";
    }
    if(SettingFile->readSetting("noExperiments").toString()=="true"){
        noExperiments=" --no-experiments";
    }
    else{
        noExperiments="";
    }

    if(SettingFile->readSetting("disableSuggestionsService").toString()=="true"){
        disableSuggestionsService=" --disable-suggestions-service";
    }
    else{
        disableSuggestionsService="";
    }

    if(SettingFile->readSetting("disableSavePasswordBuble").toString()=="true"){
        disableSavePasswordBuble=" --disable-save-password-bubble";
    }
    else{
        disableSavePasswordBuble="";
    }

    if(SettingFile->readSetting("disableAppcontainer").toString()=="true"){
        disableAppcontainer=" --disable-appcontainer";
    }
    else{
        disableAppcontainer="";
    }


    //appTo=SettingFile->readSetting("appTo").toString();
    qDebug()<<usc_ip<<" ip";
    appTo=usc_ip;
    keyBlock=SettingFile->readSetting("keyBlock").toString();


    if(keyBlock=="true"){
        system("xmodmap -e 'keycode 37= '");
        system("xmodmap -e 'keycode 105= '");
        system("xmodmap -e 'keycode 67=Escape NoSymbol Escape '");
        system("xmodmap -e 'keycode 64= '");
        system("xmodmap -e 'keycode 23= '");
        system("xmodmap -e 'keycode 133=Control_L NoSymbol Control_L Alt_L Meta_L Alt_L Meta_L '");
        system("xmodmap -e 'keycode 134=Alt_L Meta_L Alt_L Meta_L '");
        system("xmodmap -e 'keycode 66=Tab ISO_Left_Tab Tab ISO_Left_Tab '");
        system("xmodmap -e 'keycode 95= '");
        system("xmodmap -e 'keycode 96= '");
        system("xmodmap -e 'keycode 68= '");
        system("xmodmap -e 'keycode 69=Escape NoSymbol Escape '");
        system("xmodmap -e 'keycode 71=Escape NoSymbol Escape '");
        system("xmodmap -e 'keycode 72= '");
        system("xmodmap -e 'keycode 73= '");
        system("xmodmap -e 'keycode 74= '");
        system("xmodmap -e 'keycode 75= '");
        system("xmodmap -e 'keycode 76= '");
        system("xmodmap -e 'keycode 9= '");
        system("xmodmap -e 'keycode 146= '");
        system("sync");


    }
    else{
        system("xmodmap -e 'keycode 37=Control_L NoSymbol Control_L'");
        system("xmodmap -e 'keycode 105=Control_R NoSymbol Control_R'");
        system("xmodmap -e 'keycode 67=F1 F1 F1 F1 F1 F1 XF86Switch_VT_1'");
        system("xmodmap -e 'keycode 64=Alt_L Meta_L Alt_L Meta_L'");
        system("xmodmap -e 'keycode 23=Tab ISO_Left_Tab Tab ISO_Left_Tab'");
        system("xmodmap -e 'keycode 133=Super_L NoSymbol Super_L'");
        system("xmodmap -e 'keycode 134=Super_R NoSymbol Super_R'");
        system("xmodmap -e 'keycode 66=Caps_Lock NoSymbol Caps_Lock'");
        system("xmodmap -e 'keycode 95=F11 F11 F11 F11 F11 F11 XF86Switch_VT_11 F11 F11 XF86Switch_VT_11 '");
        system("xmodmap -e 'keycode 96=F12 F12 F12 F12 F12 F12 XF86Switch_VT_12 F12 F12 XF86Switch_VT_12 '");
        system("xmodmap -e 'keycode 68=F2 F2 F2 F2 F2 F2 XF86Switch_VT_2 F2 F2 XF86Switch_VT_2 '");
        system("xmodmap -e 'keycode 69=F3 F3 F3 F3 F3 F3 XF86Switch_VT_3 F3 F3 XF86Switch_VT_3 '");
        system("xmodmap -e 'keycode 71=F5 F5 F5 F5 F5 F5 XF86Switch_VT_5 F5 F5 XF86Switch_VT_5 '");
        system("xmodmap -e 'keycode 72=F6 F6 F6 F6 F6 F6 XF86Switch_VT_6 F6 F6 XF86Switch_VT_6 '");
        system("xmodmap -e 'keycode 73=F7 F7 F7 F7 F7 F7 XF86Switch_VT_7 F7 F7 XF86Switch_VT_7 '");
        system("xmodmap -e 'keycode 74=F8 F8 F8 F8 F8 F8 XF86Switch_VT_8 F8 F8 XF86Switch_VT_8 '");
        system("xmodmap -e 'keycode 75=F9 F9 F9 F9 F9 F9 XF86Switch_VT_9 F9 F9 XF86Switch_VT_9 '");
        system("xmodmap -e 'keycode 76=F10 F10 F10 F10 F10 F10 XF86Switch_VT_10 F10 F10 XF86Switch_VT_10 '");
        system("xmodmap -e 'keycode 9=Escape NoSymbol Escape '");
        system("xmodmap -e 'keycode 146=Help NoSymbol Help '");
        system("sync");
    }

    process.start("sh",QStringList()<<"-c"<<"xinput list --id-only 'ft5x0x'");
    process.waitForFinished();
    QString output(process.readAllStandardOutput());

    QString touchDevice=touch+output;

    QString command=browserStart+noNotifications+fullScreen+
            kioskMode+noErrorDialogs+disableTranslate+
            fastStart+noInfoBar+keyboard+incognito+disableGpu+
            disableD3d11+disableLocalStorage+disableOffnePages+
            disablePluginPowerSaver+disablePluginsDiscovery+
            disableSavePasswordBuble+disableSuggestionsService+
            disableSync+disableWebgl+app+goTo+noExperiments+
            disableAppcontainer+touchDevice;
    qDebug()<<command;
    qDebug()<<usc_ip<<goTo;
    system(command.toStdString().c_str());



}
void MainWindow::DinamikIP_check_clicked()
{
    qDebug()<<"dinamik clicked";
    qDebug()<<DinamikIP_check->isChecked();
    if(DinamikIP_check->isChecked()){
        usc_ip_label->setVisible(false);
        usc_ip_lineedit->setVisible(false);
        usc_ip_pushbutton->setVisible(false);

        usc_port_label->setVisible(false);
        usc_port_lineedit->setVisible(false);
        usc_port_pushbutton->setVisible(false);


        subnetMax_lineedit->setVisible(false);
        subnetMask_pushbutton->setVisible(false);
        gateway_label->setVisible(false);
        SabitIP_check->setCheckState(Qt::Unchecked);

    }
    else{
        usc_ip_label->setVisible(true);
        usc_ip_lineedit->setVisible(true);
        usc_ip_pushbutton->setVisible(true);

        usc_port_label->setVisible(true);
        usc_port_lineedit->setVisible(true);
        usc_port_pushbutton->setVisible(true);


        subnetMax_lineedit->setVisible(true);
        subnetMask_pushbutton->setVisible(true);
        gateway_label->setVisible(true);
    }
}

void MainWindow::SabitIP_check_clicked()
{
    qDebug()<<"statik clicked";
    if(SabitIP_check->isChecked()){
        usc_ip_label->setVisible(true);
        usc_ip_lineedit->setVisible(true);
        usc_ip_pushbutton->setVisible(true);

        usc_port_label->setVisible(true);
        usc_port_lineedit->setVisible(true);
        usc_port_pushbutton->setVisible(true);


        subnetMax_lineedit->setVisible(true);
        subnetMask_pushbutton->setVisible(true);
        gateway_label->setVisible(true);
        DinamikIP_check->setCheckState(Qt::Unchecked);
    }
    else{
        usc_ip_label->setVisible(false);
        usc_ip_lineedit->setVisible(false);
        usc_ip_pushbutton->setVisible(false);

        usc_port_label->setVisible(false);
        usc_port_lineedit->setVisible(false);
        usc_port_pushbutton->setVisible(false);


        subnetMax_lineedit->setVisible(false);
        subnetMask_pushbutton->setVisible(false);
        gateway_label->setVisible(false);



    }

}

void MainWindow::usc_ip_pushbutton_clicked()
{
    qDebug()<<"KEYBOARDDDD";
    if(Mwindowlock==false)
    {
        BuzzerON();
        numpadLineEdit->setVisible(true);
        numpadLineEdit->raise();
        numpadframe->show();
        numpadframe->raise();
        numpadframe->setVisible(true);
        numpadLineEdit->setText(usc_ip_lineedit->text());
        hideAllNetworkComponents();

        Numpad_Output.clear();
        Numpad_Output=usc_ip_lineedit->text();
        qDebug()<<Numpad_Output;
        keyboardcomingflag=9;
        sifre_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        sifre_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_port_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_ip_lineedit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::usc_port_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=usc_port_lineedit->text();
        numpadLineEdit->setVisible(true);
        numpadLineEdit->raise();
        numpadframe->show();
        numpadframe->raise();
        numpadframe->setVisible(true);
        hideAllNetworkComponents();
        numpadLineEdit->setText(usc_port_lineedit->text());
        keyboardcomingflag=10;
        sifre_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        sifre_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_port_lineedit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_ip_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::subnetMask_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=subnetMax_lineedit->text();
        numpadLineEdit->setVisible(true);
        numpadLineEdit->raise();
        hideAllNetworkComponents();
        numpadframe->show();
        numpadframe->raise();
        numpadframe->setVisible(true);
        numpadLineEdit->setText(subnetMax_lineedit->text());
        keyboardcomingflag=11;
        sifre_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        sifre_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        subnetMax_lineedit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_ip_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::licance_pushbutton_Clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        QCryptographicHash calculateMD5(QCryptographicHash::Sha256);
        calculateMD5.addData(getMacAddress().toUtf8());
        //QSettings settings(SettingsFile, QSettings::IniFormat);
        settings.setValue("Licence",calculateMD5.result().toHex());
        sync();
        system("sync");
        key->outputText.clear();
        status_label->setText(settingsSaved_String);
        warning_label->setText(systemWillBeRestarted_String);
        menu_frame->close();
        menutabwidgetindex=0;
        password_frame->setVisible(false);
        numpadframe->setVisible(false);
        LCD_status=31;
        //    unitprice_lineedit->setText("");
        //    numpadoutputtext.clear();
        //    menu_frame->setVisible(false);
        //    tabframe->setVisible(false);
        //    numpadframe->setVisible(false);
        //    keyboard_frame->setVisible(false);
        //    main_frame->setVisible(true);
        //    top_frame->setVisible(true);
        //    menu_tabWidget->setCurrentIndex(0);
        //    numpadframe->setVisible(false);
        //    LCD_status=11;
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::sd_kart_yedek_pushbutton_Clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        QDir dir("/media/root/");
        QFileInfoList list_usb=dir.entryInfoList();
        for(int i=0;i<list_usb.size();i++)
        {
            QString file_name=list_usb.at(i).fileName();
            if(file_name=="MEPSAN")
            {
                system("cp -f /root/Database/Iett_database /media/root/MEPSAN/Iett_database");
                system("cp -f /var/www/html/setting_save.txt /media/root/MEPSAN/setting_save.txt");
                break;
            }
        }
        sync();
        system("sync");
        key->outputText.clear();
        status_label->setText(settingsSaved_String);
        warning_label->setText(systemWillBeRestarted_String);
        menu_frame->close();
        menutabwidgetindex=0;
        password_frame->setVisible(false);
        numpadframe->setVisible(false);
        LCD_status=31;
        //    unitprice_lineedit->setText("");
        //    numpadoutputtext.clear();
        //    menu_frame->setVisible(false);
        //    tabframe->setVisible(false);
        //    numpadframe->setVisible(false);
        //    keyboard_frame->setVisible(false);
        //    main_frame->setVisible(true);
        //    top_frame->setVisible(true);
        //    menu_tabWidget->setCurrentIndex(0);
        //    numpadframe->setVisible(false);
        //    LCD_status=11;
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }

}

void MainWindow::sd_kart_al_pushbutton_Clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        QDir dir("/media/root/");
        QFileInfoList list_usb=dir.entryInfoList();
        for(int i=0;i<list_usb.size();i++)
        {
            QString file_name=list_usb.at(i).fileName();
            if(file_name=="MEPSAN")
            {
                system("cp -f /media/root/MEPSAN/Iett_database  /root/Database/Iett_database");
                system("cp -f /media/root/MEPSAN/setting_save.txt /var/www/html/setting_save.txt");
                break;
            }
        }
        sync();
        system("sync");
        key->outputText.clear();
        status_label->setText(settingsSaved_String);
        warning_label->setText(systemWillBeRestarted_String);
        menu_frame->close();
        menutabwidgetindex=0;
        password_frame->setVisible(false);
        numpadframe->setVisible(false);
        LCD_status=31;
        //    unitprice_lineedit->setText("");
        //    numpadoutputtext.clear();
        //    menu_frame->setVisible(false);
        //    tabframe->setVisible(false);
        //    numpadframe->setVisible(false);
        //    keyboard_frame->setVisible(false);
        //    main_frame->setVisible(true);
        //    top_frame->setVisible(true);
        //    menu_tabWidget->setCurrentIndex(0);
        //    numpadframe->setVisible(false);
        //    LCD_status=11;
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }

}

void MainWindow::saha_modu_pushbutton_Clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        Crypto  Cryptwindow;
        //QSettings settings(SettingsFile, QSettings::IniFormat);
        QCryptographicHash calculateMD5(QCryptographicHash::Sha256);
        calculateMD5.addData(getMacAddress().toUtf8());
        settings.setValue("Licence",calculateMD5.result().toHex());
        sync();
        system("sync");
        key->outputText.clear();
        status_label->setText(settingsSaved_String);
        warning_label->setText(systemWillBeRestarted_String);
        menu_frame->close();
        menutabwidgetindex=0;
        password_frame->setVisible(false);
        numpadframe->setVisible(false);
        settings.setValue("TS_no",0);
        system("rm -f /root/Database/Iett_database");
        sync();
        system("sync");
        key->outputText.clear();
        menu_frame->close();
        menutabwidgetindex=0;
        password_frame->setVisible(false);
        numpadframe->setVisible(false);
        pump_lock_count=0;
        saved_day=QDate::currentDate().toString("dd.MM.yyyy");
        QByteArray Bytearraysaved_day=QString::fromStdString(Cryptwindow.encrypt(saved_day.toStdString())).toLatin1();
        if(test)qDebug()<<"saved_day_windows"<<Bytearraysaved_day;
        settings.setValue("saved_day",Bytearraysaved_day.toHex());
        settings.setValue("whitelist_all", true);
        settings.setValue("whitelist_version", "0");
        settings.setValue("person_version", "0");
        settings.setValue("Version_config", "0");
        sync();
        system("sync");
        LCD_status=31;
        //    unitprice_lineedit->setText("");
        //    numpadoutputtext.clear();
        //    menu_frame->setVisible(false);
        //    tabframe->setVisible(false);
        //    numpadframe->setVisible(false);
        //    keyboard_frame->setVisible(false);
        //    main_frame->setVisible(true);
        //    top_frame->setVisible(true);
        //    menu_tabWidget->setCurrentIndex(0);
        //    numpadframe->setVisible(false);
        //    LCD_status=11;
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::testmodu_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if(testmodestart->text().indexOf("TEST")>=0)
        {

            testmodestart->setText(printReport_String);
            test_modu=true;
            if(clrc_check_flg)
            {
                cardreader_test_status->setIcon(icon9);
            }
            else
            {
                cardreader_test_status->setIcon(iconcancel);
            }
            if(rfid_check_flg)
            {
                accesspoint_test_status->setIcon(icon9);
            }
            else
            {
                accesspoint_test_status->setIcon(iconcancel);
            }
            if(printer_check_flg)
            {
                printer_test_status->setIcon(icon9);
            }
            else
            {
                printer_test_status->setIcon(iconcancel);
            }
            cardreader_test_status->setVisible(true);
            printer_test_status->setVisible(true);
            accesspoint_test_status->setVisible(true);
        }
        else
        {
            testmodestart->setText(startTest_String);
            print_test_mode=true;
            test_modu=false;
            cardreader_test_status->setVisible(false);
            printer_test_status->setVisible(false);
            accesspoint_test_status->setVisible(false);
        }
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::screen_lines(QString line_1, QString line_2)
{
    if(cannotchange_frame==false)
    {
        status_label->setText(line_1);
        warning_label->setText(line_2);
    }

}

void MainWindow::frameChange(int frameNumber)
{
    if(test)qDebug()<<"FRAME Change"<<frameNumber;
    if(frameNumber==15)
    {
        if(password_is_deactive==false)
        {
            paswrd_label->setText(automationErrorEnterPassword_String);

            paswrd_label->setStyleSheet(QLatin1String("background-color: rgb(172,20,20);\n"
                                                      "color: rgb(255, 255, 255);\n"
                                                      "border:1px solid;\n"
                                                      "border-radius:5px;\n"
                                                      "border-color: rgb(24, 24, 24);"));
            if(menu_passwrd==true) //menu şifre ekranndan otomasyon hatası ekranına geçişte temizlik
            {
                menu_passwrd=false;
                Numpad_Output.clear();
                passwrdLine->setText("");
            }
        }
        else
        {
            if(test) qDebug()<<"otopswrd"<<otopswrd<<LCD_status_last<<LCD_status<<pump[pumpNo].status;
            USC_com_error_count=0;
            USC_com_error_flg=false;
            system_started=false;
            USC_is_exist=false;
            //pump[pumpNo].status=pump[pumpNo].laststatus;
            if((pump[pumpNo].laststatus==2) || (pump[pumpNo].laststatus==6)|| (pump[pumpNo].laststatus==3))
            {
                Sale_resume_flg=true;
            }
            if((LCD_status_last==1) || (LCD_status_last==2)|| (LCD_status_last==11)|| (LCD_status_last==7)){LCD_status=LCD_status_last; LCD_refresh_flg=1;}
            if(LCD_status_last==8){LCD_status=LCD_status_last; LCD_refresh_flg=1;} //sonradan
            if(pump[pumpNo].status==0){LCD_status=1;LCD_refresh_flg=1;}
            for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
            {
                TEXT_fromMaster[jj]=0x20;
            }
        }
        if(system_lock==true)
        {
            password_frame->setVisible(false);
            numpadframe->setVisible(false);
            main_frame->setVisible(true);
            top_frame->setVisible(true);
            LCD_status=13;
            cannotchange_frame=false;
            warning_label->setText(deviceIsLocked_String);
            menu_frame->setVisible(false);  // değiştimm
            numpadframe->setVisible(false);
            keyboard_frame->setVisible(false);
            main_frame->setVisible(true);
            top_frame->setVisible(true);
            menu_frame->close();
            menutabwidgetindex=0;
            password_frame->setVisible(false);
            pmenter_frame->setVisible(false);

        }

    }
    //qDebug()<<"CANNOT CHANGE FRAME"<<cannotchange_frame;
    //qDebug()<<"StoredScreenRFID_fromMaster----------FRAME"<<StoredScreenRFID_fromMaster;
    if(cannotchange_frame==false)
    {
        switch (frameNumber) {

        case 2:
            main_frame->setVisible(false);
            top_frame->setVisible(false);
            key->setGeometry(QRect(QPoint(0,0),QSize(800,355)));
            keyboard_frame->setGeometry(QRect(0, 125, 800, 355));
            plate_frame->setVisible(true);
            keyboard_frame->setVisible(true);
            plate_frame->raise();
            keyboard_frame->raise();
            break;
        case 7:
            device_icon_button->setIcon(QIcon(":/new/prefix1/icons/logo.png"));
            plateLine->clear();
            kmLine->clear();
            plate_frame->setVisible(false);
            keyboard_frame->setVisible(false);
            km_frame->setVisible(false);
            numpadframe->setVisible(false);
            password_frame->setVisible(false);
            top_frame->setVisible(true);
            main_frame->setVisible(true);
            break;
        case 8:
            device_icon_button->setIcon(QIcon(":/new/prefix1/icons/gasoline-pump.png"));
            plateLine->clear();
            kmLine->clear();
            plate_frame->setVisible(false);
            keyboard_frame->setVisible(false);
            km_frame->setVisible(false);
            numpadframe->setVisible(false);
            password_frame->setVisible(false);
            top_frame->setVisible(true);
            main_frame->setVisible(true);
            break;
        case 9:
            plateLine->clear();
            plate_frame->setVisible(false);
            keyboard_frame->setVisible(false);
            km_frame->setVisible(false);
            numpadframe->setVisible(false);
            password_frame->setVisible(false);
            top_frame->setVisible(true);
            main_frame->setVisible(true);
            warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                       "border-color: rgb(255, 255, 255);\n"
                                                       "color: rgb(0, 0, 0); background-color: rgb(255, 0, 0)"));
            warning_icon_button->setVisible(true);
            break;

        case 11:
            numpadframe->setGeometry(0,130,801,350);
            main_frame->setVisible(false);
            top_frame->setVisible(false);
            plate_frame->setVisible(false);
            keyboard_frame->setVisible(false);
            km_frame->setVisible(true);
            numpadframe->setVisible(true);
            break;
        case 15:
            if(system_lock==false)
            {
                if(password_is_deactive==false)
                {qDebug()<<"numpad2";
                    main_frame->setVisible(false);
                    top_frame->setVisible(false);
                    plate_frame->setVisible(false);
                    keyboard_frame->setVisible(false);
                    km_frame->setVisible(false);
                    paswrd_label->setText(automationErrorEnterPassword_String);

                    paswrd_label->setStyleSheet(QLatin1String("background-color: rgb(172, 20, 20);\n"
                                                              "color: rgb(255, 255, 255);\n"
                                                              "border:1px solid;\n"
                                                              "border-radius:5px;\n"
                                                              "border-color: rgb(24, 24, 24);"));
                    password_frame->setVisible(true);
                    numpadframe->setGeometry(0,130,801,350);
                    numpadframe->setVisible(true);
                    password_frame->raise();
                    numpadframe->raise();
                    keyboardcomingflag=0;
                    cannotchange_frame=true;
                }
                else
                {
                    if(test) qDebug()<<"otopswrd"<<otopswrd<<LCD_status_last<<LCD_status<<pump[pumpNo].status;
                    USC_com_error_count=0;
                    USC_com_error_flg=false;
                    system_started=false;
                    USC_is_exist=false;
                    //pump[pumpNo].status=pump[pumpNo].laststatus;
                    if((pump[pumpNo].laststatus==2) || (pump[pumpNo].laststatus==6)|| (pump[pumpNo].laststatus==3))
                    {
                        Sale_resume_flg=true;
                    }
                    if((LCD_status_last==1) || (LCD_status_last==2)|| (LCD_status_last==11)|| (LCD_status_last==7)){LCD_status=LCD_status_last; LCD_refresh_flg=1;}
                    if(LCD_status_last==8){LCD_status=LCD_status_last; LCD_refresh_flg=1;} //sonradan
                    if(pump[pumpNo].status==0){LCD_status=1;LCD_refresh_flg=1;}
                    sifre.clear();
                    for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                    {
                        TEXT_fromMaster[jj]=0x20;
                    }
                    cannotchange_frame=false;
                    password_frame->setVisible(false);

                }
            }
            break;

        default:
            plateLine->clear();
            plate_frame->setVisible(false);
            keyboard_frame->setVisible(false);
            km_frame->setVisible(false);
            numpadframe->setVisible(false);
            password_frame->setVisible(false);
            top_frame->setVisible(true);
            main_frame->setVisible(true);
            device_icon_button->setIcon(QIcon(":/new/prefix1/icons/logo.png"));
            warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                       "border-color: rgb(255, 255, 255);\n"
                                                       "color: rgb(0, 0, 0); background-color: rgb(142, 142, 142)"));
            warning_icon_button->setVisible(false);
            Numpad_Output.clear();
            break;
        }
        if(USC_is_exist==false){top_button->setText("UPC");}else top_button->setText(uscActive_String);
        if(StoredScreenRFID_fromMaster==1){    device_icon_button->setIcon(QIcon(":/new/prefix1/icons/card2.png"));}
        if((StoredScreenRFID_fromMaster==8) || (meter_com_err==true))
        {
            warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                       "border-color: rgb(255, 255, 255);\n"
                                                       "color: rgb(0, 0, 0); background-color: rgb(255, 0, 0)"));
            warning_icon_button->setVisible(true);
        }
        if(frameNumber==6){device_icon_button->setIcon(QIcon(":/new/prefix1/icons/warning.png"));}
        if(config_error==true)
        {
            warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                       "border-color: rgb(255, 255, 255);\n"
                                                       "color: rgb(0, 0, 0); background-color: rgb(255, 0, 0)"));
            warning_icon_button->setVisible(true);
            if(setting_configuration==false)
                status_label->setText(deviceSettingsAreMissing_String);
            if(nozzle_configuration==false)
                status_label->setText(nozzleSettingsAreMissing_String);

            warning_label->setText(waitingSettingsFromLocwiz_String);
            config_kontrol=true;
            cannotchange_frame=true;
        }
        if((LCD_status==13) && (system_lock==false))
        {
            warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                       "border-color: rgb(255, 255, 255);\n"
                                                       "color: rgb(0, 0, 0); background-color: rgb(255, 0, 0)"));
            warning_icon_button->setVisible(true);
            status_label->setText(invalidLicence_String);
            warning_label->setText("");
            cannotchange_frame=true;

        }
        //qDebug()<<"LCD STATUS AND SYSTEM LOCK STATUS"<<LCD_status<<"     "<<system_lock;

    }
    else
    {
        if(config_error==false && config_kontrol)
        {
            config_kontrol=false;
            warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                       "border-color: rgb(255, 255, 255);\n"
                                                       "color: rgb(0, 0, 0); background-color: rgb(142, 142, 142)"));
            warning_icon_button->setVisible(false);
            status_label->setText(readyToSale_String);
            warning_label->setText("");
            cannotchange_frame=false;
            QFont font16;
            font16.setPointSize(15);
            font16.setItalic(true);
            font16.setUnderline(true);
            for(int i=0;i<=6;i++)
            {
                product_comboBox_label[i] = new QLabel(producttab);
                product_comboBox_label[i]->setObjectName("product_comboBox_label"+QString::number(i));
                if(i<3)
                {
                    if(i==0){product_comboBox_label[i]->setGeometry(QRect(10, 10+(i*81), 151, 81));  }
                    else {product_comboBox_label[i]->setGeometry(QRect(10, 10+(20*(i))+((i)*81), 151, 81));}
                }
                else
                {
                    product_comboBox_label[i]->setGeometry(QRect(390, 10+(10*((i-3)))+((i-3)*81), 151, 81));
                }
                product_comboBox_label[i]->setFont(font16);
                product_comboBox_label[i]->setText(nozzle_String+QString::number(i+1)+":");



                product_comboBox[i] = new QComboBox(producttab);
                product_comboBox[i]->setObjectName("product_comboBox"+QString::number(i));
                if(i<3)
                {
                    if(i==0){product_comboBox[i]->setGeometry(QRect(161, 10+(i*81), 191, 81));}
                    else {product_comboBox[i]->setGeometry(QRect(161, 10+(20*(i))+((i)*81), 194, 81));}
                }
                else
                {
                    product_comboBox[i]->setGeometry(QRect(541, 10+(20*(i-3))+((i-3)*81), 194, 81));
                }
                QFont font;
                font.setPointSize(23);
                product_comboBox[i]->setFont(font);
                product_comboBox[i]->setLayoutDirection(Qt::LeftToRight);
                product_comboBox[i]->setStyleSheet(QLatin1String("QComboBox {\n"
                                                                 "background-color: rgb(107, 107, 107);\n"
                                                                 "color: rgb(255, 255, 255);\n"
                                                                 "border:1px solid #FFFFFF ;}\n"
                                                                 "QComboBox::drop-down {\n"
                                                                 "subcontrol-origin: padding;\n"
                                                                 "subcontrol-position: top right;\n"
                                                                 "width:70px;\n"
                                                                 "\n"
                                                                 "}"));
                product_comboBox[i]->setEditable(false);
                product_comboBox[i]->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

                product_comboBox[i]->clear();

                QStringList products=MWdatabase->getProductname().split("#");
                products.removeLast();
                product_comboBox[i]->insertItems(0,products);




                product_comboBox_label[i]->setVisible(false);
                product_comboBox[i]->setVisible(false);
            }


            for(int i=0;i<Number_of_Nozzle;i++)
            {
                product_comboBox_label[i]->setVisible(true);
                product_comboBox[i]->setVisible(true);
            }
            cannotchange_frame=false;
        }
    }
    if(system_lock==true) ///((LCD_status==13) || (LCD_status==15)) &&  değiştim
    {
        warning_label->setStyleSheet(QLatin1String("border:2px solid;\n"
                                                   "border-color: rgb(255, 255, 255);\n"
                                                   "color: rgb(0, 0, 0); background-color: rgb(255, 0, 0)"));
        warning_icon_button->setVisible(true);

        status_label->setText(systemLocked_String);
        warning_label->setText(deviceIsLocked_String);
        menu_frame->setVisible(false);
        numpadframe->setVisible(false);
        keyboard_frame->setVisible(false);
        main_frame->setVisible(true);
        top_frame->setVisible(true);
        menu_frame->close();
        menutabwidgetindex=0;
        password_frame->setVisible(false);
        pmenter_frame->setVisible(false);
    }
}


void MainWindow::Numpad_Sender_Clicked()
{

    if(Mwindowlock==false)
    {
        BuzzerON();
        QPushButton *Numpadbutton = (QPushButton *)sender();
        Numpad_Output.append(Numpadbutton->text());
        if((LCD_status==40||LCD_status==15) || (LCD_status==1) ||((LCD_status==13) && (system_lock==false))) //şifre linedit
        {
            if(LCD_status==1)
            {
                if(keyboardcomingflag==3)
                {
                    sifre_1_lineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==4)
                {
                    sifre_2_lineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==9)
                {
                    usc_ip_lineedit->setText(Numpad_Output);
                    numpadLineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==10)
                {
                    usc_port_lineedit->setText(Numpad_Output);
                    numpadLineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==11)
                {
                    subnetMax_lineedit->setText(Numpad_Output);
                    numpadLineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==20)
                {
                    otomasyon_adres_lineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==21)
                {
                    nozzle_adres_lineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==22)
                {
                    channel_lineEdit->setText(Numpad_Output);
                }
                else if(keyboardcomingflag==23)
                {
                    pause_time_lineEdit->setText(Numpad_Output);
                }


            }
            passwrdLine->setText(Numpad_Output);
        }
        if(LCD_status==40){
            if(keyboardcomingflag==24)
            {
//                    tarih_1_lineEdit->setText(Numpad_Output);
                numpadDateTimeEdit->setText(Numpad_Output);
                //Numpad_Output=numpadDateTimeEdit->text();
                qDebug()<<"ooo"<<Numpad_Output.length();
            }
        }
        if(LCD_status==11)
        {
            kmWarningButton->setVisible(false);
            QFont font7;
            font7.setPointSize(45);
            font7.setBold(true);
            kmLine->setFont(font7);
            if(Numpadbutton->text()==".")
            {
                Numpad_Output.remove((Numpad_Output.length()-1),1);
            }
            kmLine->setText(Numpad_Output);
        }
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
        if(LCD_status==35 )
        {
            if(Numpadbutton->text()=="." || Numpad_Output.length()>16)
            {
                Numpad_Output.remove((Numpad_Output.length()-1),1);
            }
            passwrdLine->setText(Numpad_Output);
            if(Numpad_Output.length()==2)
            {
                Numpad_Output+="/";
                passwrdLine->setText(Numpad_Output);
            }
            else if(Numpad_Output.length()==5)
            {
                Numpad_Output+="/";
                passwrdLine->setText(Numpad_Output);
            }
            else if(Numpad_Output.length()==10)
            {
                Numpad_Output+=" ";
                passwrdLine->setText(Numpad_Output);
            }
            else if(Numpad_Output.length()==13)
            {
                Numpad_Output+=":";
                passwrdLine->setText(Numpad_Output);
            }
        }
        if(LCD_status==40 )
        {
            if(Numpadbutton->text()=="." || Numpad_Output.length()>16)
            {
                Numpad_Output.remove((Numpad_Output.length()-1),1);
            }
            numpadDateTimeEdit->setText(Numpad_Output);
            if(Numpad_Output.length()==2)
            {
                Numpad_Output+="/";
                numpadDateTimeEdit->setText(Numpad_Output);
            }
            else if(Numpad_Output.length()==5)
            {
                Numpad_Output+="/";
                numpadDateTimeEdit->setText(Numpad_Output);
            }
            else if(Numpad_Output.length()==10)
            {
                Numpad_Output+=" ";
                numpadDateTimeEdit->setText(Numpad_Output);
            }
            else if(Numpad_Output.length()==13)
            {
                Numpad_Output+=":";
                numpadDateTimeEdit->setText(Numpad_Output);
            }
        }

    }

}
void MainWindow::Numpad_Enter_Clicked()
{qDebug()<<"yuh";
    if(keyboardcomingflag==9 || keyboardcomingflag==10 || keyboardcomingflag==11){
        raiseAllNeworkComponents();
        numpadLineEdit->setVisible(false);
        usc_ip_lineedit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                     "color: rgb(255,255,255);\n"
                                                     "border-radius:10px;"));
        usc_port_lineedit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                      "color: rgb(255,255,255);\n"
                                                      "border-radius:10px;"));
        subnetMax_lineedit->setStyleSheet(QLatin1String("background-color: rgb(24, 24, 24);\n"
                                                       "color: rgb(255,255,255);\n"
                                                       "border-radius:10px;"));
    }
    if(Mwindowlock==false)
    {
        BuzzerON();
        if(test)qDebug()<<"Numpad_Enter_Clicked"<<commokpswrd<<Numpad_Output<<LCD_status<<LCD_status_last<<menutabwidgetindex;

        if(((LCD_status==1) && (NewPlateExist==0)) ||((LCD_status==13) && (system_lock==false)))
        {
            if(menutabwidgetindex==3)
            {
                password_frame->setVisible(false);
                menu_frame->setGeometry(QRect(0, 0, 800, 480));
                menu_tabWidget->setGeometry(QRect(5, 5, 800, 350));
                keyboard_frame->setGeometry(QRect(0, 150, 800, 330));
                key->setGeometry(QRect(QPoint(0,0),QSize(800,355)));
                keyboard_frame->setVisible(false);
                numpadframe->setGeometry(QRect(0, 145, 801, 331));
                numpadframe->setVisible(false);
                menu_frame->setVisible(true);
                menu_enter->setVisible(true);
                menu_enter->raise();
                menu_cancel->setVisible(true);
                menu_cancel->raise();

            }
            else if(write_configuration_active)
            {
                access_point_info_label->setVisible(true);
                access_point_read_pushbutton->setVisible(true);
                access_point_write_pushbutton->setVisible(true);
                tts_active_pushbutton->setVisible(true);
                menu_enter->setVisible(true);
                menu_cancel->setVisible(true);
                otomasyon_adres_label->setVisible(true);
                nozzle_adres_label->setVisible(true);
                channel_label->setVisible(true);
                pause_time_label->setVisible(true);
                otomasyon_adres_new_label->setVisible(false);
                nozzle_adres_new_label->setVisible(false);
                channel_new_label->setVisible(false);
                pause_time_new_label->setVisible(false);
                otomasyon_adres_lineEdit->setVisible(false);
                nozzle_adres_lineEdit->setVisible(false);
                channel_lineEdit->setVisible(false);
                pause_time_lineEdit->setVisible(false);
                otomasyon_adres_pushbutton->setVisible(false);
                nozzle_adres_pushbutton->setVisible(false);
                pause_time_pushbutton->setVisible(false);
                channel_pushbutton->setVisible(false);
                numpadframe->setVisible(false);
                write_configuration_active=false;
                acc_otomasyon_adresi=otomasyon_adres_lineEdit->text().toLatin1();

                acc_nozzle_adresi=nozzle_adres_lineEdit->text().toLatin1();
                acc_channel_no_adresi=channel_lineEdit->text().toLatin1();
                acc_pause_time="08";
                tts_pause_count=pause_time_lineEdit->text().toInt();
                settings.setValue("tts_pause_count",tts_pause_count);
                write_configuration=true;
                access_point_info_label->setText(loadingAccesspointSettingsPleaseWait_String);
                write_configuration=true;
            }
            else
            {
                if(commokpswrd==Numpad_Output)
                {
                    //2222
                    if(((LCD_status==1)&& (NewPlateExist==0))||((LCD_status==13) && (system_lock==false)))
                    {
                        //QSettings settings(SettingsFile, QSettings::IniFormat);
                        cannotchange_frame=true;
                        read_configuration=true;
                        read_configuration_success=0;
                        //Number_of_Nozzle=settings.value("Number_of_Nozzle").toInt(&ok);
                        ABU_address=settings.value("ABU_address").toInt();
                        //                        unit_price_dot_index=settings.value("Unit_price_dot_index").toInt(&ok);
                        //                        amount_dot_index=settings.value("Amount_dot_index").toInt(&ok);
                        //                        volume_dot_index=settings.value("Volume_dot_index").toInt(&ok);
                        //                        int tempcard_reader_address=settings.value("CardReader_Addr").toInt(&ok);
                        //                        dec_to_bcd(tempcard_reader_address);
                        //                        CardReader_Addr[0]=bcd[1]+0x30;
                        //                        CardReader_Addr[1]=bcd[0]+0x30;
                        //                        printing_header_data[0]=settings.value("Printing_header_data_0").toString();
                        //                        printing_header_data[1]=settings.value("Printing_header_data_1").toString();
                        //                        printing_header_data[2]=settings.value("Printing_header_data_2").toString();
                        //                        printing_header_data[3]=settings.value("Printing_header_data_3").toString();
                        //                        End_info[0]=settings.value("Printing_End_info_0").toString();
                        //                        End_info[1]=settings.value("Printing_End_info_1").toString();
                        //                        ask_km_after=settings.value("ask_km").toBool();
                        commokpswrd=settings.value("commokpswrd").toString();
                        otopswrd=settings.value("otopswrd").toString();
                        usc_ip=settings.value("usc_ip").toString();
                        usc_port=settings.value("usc_port").toString();
                        for(int ll=1; ll<=Number_of_Nozzle;ll++)
                        {
                            product_comboBox[ll-1]->setCurrentIndex(product_comboBox[ll-1]->findText(pump[pumpNo].nozzle[ll].Product_name));
                            //qDebug()<<pump[pumpNo].nozzle[ll].Product_name;
                        }
                        if(ttspause)
                        {
                            tts_active_pushbutton->setText(serviceOpen_String);
                        }
                        else
                        {
                            tts_active_pushbutton->setText(serviceClosed_String);
                        }
                        pause_time_lineEdit->setText(QString::number(tts_pause_count));
                        adress_spinBox->setValue(ABU_address);
                        Nozzle_Num_spinBox->setValue(Number_of_Nozzle);
                        CardR_Address_spinBox->setValue(tempcard_reader_address);
                        Volumedotindex_spinBox->setValue(volume_dot_index);
                        Amountdotindex_spinBox->setValue(amount_dot_index);
                        Upricedotindex_spinBox->setValue(unit_price_dot_index);
                        receipt_1_lineEdit->setText(printing_header_data[0]);
                        receipt_2_lineEdit->setText(printing_header_data[1]);
                        receipt_3_lineEdit->setText(End_info[0]);
                        receipt_4_lineEdit->setText(End_info[1]);
                        kmsor_check->setChecked(ask_km);
                        freeSaleCheck->setChecked(askSellingStyle);
                        plateSaleCheck->setChecked(askPlateSale);
                        key->outputText.clear();
                        sifre_1_lineEdit->setText(commokpswrd);
                        sifre_1_lineEdit->setVisible(false);
                        sifre_2_lineEdit->setText(otopswrd);
                        sifre_2_lineEdit->setVisible(false);
                        usc_ip_lineedit->setText(usc_ip);
                        usc_port_lineedit->setText(usc_port);
                        if(menu_tabWidget->isTabEnabled(7))
                        {
                            menu_tabWidget->removeTab(7);
                        }
                        menu_frame->show();
                        menu_frame->raise();

                    }
                }
                else if(Numpad_Output==createPasswd())
                {
                    menu_tabWidget->addTab(kmtab, QString());
                    menu_tabWidget->setTabText(menu_tabWidget->indexOf(kmtab), QApplication::translate("MainWindow", "GENEL AYARLAR", 0));
                    //QSettings settings(SettingsFile, QSettings::IniFormat);
                    cannotchange_frame=true;
                    //Number_of_Nozzle=settings.value("Number_of_Nozzle").toInt(&ok);
                    ABU_address=settings.value("ABU_address").toInt();
                    //                    unit_price_dot_index=settings.value("Unit_price_dot_index").toInt(&ok);
                    //                    amount_dot_index=settings.value("Amount_dot_index").toInt(&ok);
                    //                    volume_dot_index=settings.value("Volume_dot_index").toInt(&ok);
                    //                    int tempcard_reader_address=settings.value("CardReader_Addr").toInt(&ok);
                    //                    dec_to_bcd(tempcard_reader_address);
                    //                    CardReader_Addr[0]=bcd[1]+0x30;
                    //                    CardReader_Addr[1]=bcd[0]+0x30;
                    //                    printing_header_data[0]=settings.value("Printing_header_data_0").toString();
                    //                    printing_header_data[1]=settings.value("Printing_header_data_1").toString();
                    //                    printing_header_data[2]=settings.value("Printing_header_data_2").toString();
                    //                    printing_header_data[3]=settings.value("Printing_header_data_3").toString();
                    //                    End_info[0]=settings.value("Printing_End_info_0").toString();
                    //                    End_info[1]=settings.value("Printing_End_info_1").toString();
                    //                    ask_km_after=settings.value("ask_km").toBool();
                    commokpswrd=settings.value("commokpswrd").toString();
                    otopswrd=settings.value("otopswrd").toString();
                    usc_ip=settings.value("usc_ip").toString();
                    usc_port=settings.value("usc_port").toString();
                    for(int ll=1; ll<=Number_of_Nozzle;ll++)
                    {
                        product_comboBox[ll-1]->setCurrentIndex(product_comboBox[ll-1]->findText(settings.value("Product_name_"+QString::number(ll)).toString()));
                    }
                    adress_spinBox->setValue(ABU_address);
                    Nozzle_Num_spinBox->setValue(Number_of_Nozzle);
                    CardR_Address_spinBox->setValue(tempcard_reader_address);
                    Volumedotindex_spinBox->setValue(volume_dot_index);
                    Amountdotindex_spinBox->setValue(amount_dot_index);
                    Upricedotindex_spinBox->setValue(unit_price_dot_index);
                    receipt_1_lineEdit->setText(printing_header_data[0]);
                    receipt_2_lineEdit->setText(printing_header_data[1]);
                    receipt_3_lineEdit->setText(End_info[0]);
                    receipt_4_lineEdit->setText(End_info[1]);
                    kmsor_check->setChecked(ask_km_after);
                    key->outputText.clear();
                    sifre_1_lineEdit->setText(commokpswrd);
                    sifre_1_lineEdit->setVisible(false);
                    sifre_2_lineEdit->setText(otopswrd);
                    sifre_2_lineEdit->setVisible(false);
                    usc_ip_lineedit->setText(usc_ip);
                    usc_port_lineedit->setText(usc_port);
                    menu_frame->show();
                    menu_frame->raise();
                }
                else
                {

                    Numpad_Output.clear();
                    paswrd_label->setText(wrongPassword_String);
                    passwrdLine->setText(Numpad_Output);

                }
            }
        }

        if(LCD_status==11) // km giriniz enter ***CHN***
        {
            kmWarningButton->setVisible(false);
            customerCardLock=true;
            vehiclekmtr=kmLine->text();
            QFont font96;
            font96.setPointSize(26);
            font96.setBold(true);
            kmLine->setFont(font96);
            kmLine->setText(waiting_String);
            key->outputText.clear();
            StoredScreenRFID_fromMaster=11;
            qDebug()<<Customer_plate<<"ne??"<<MWdatabase->kontrolSalesPlate(Customer_plate);
            if(askSellingStyle==true) {

                if(MWdatabase->kontrolSalesPlate(Customer_plate)==false){
                    if(customerCard==true){
                        if(MWdatabase->getKm(CustomerID) <= vehiclekmtr.toInt())
                        {

                            MWdatabase->updateKm(vehiclekmtr);
                            customerCard=false;
                            kmConfirm_flg=true;
                            LCD_status=1;
                        }
                        else
                        {
                            kmConfirm_flg=false;
                            QFont font97;
                            font97.setPointSize(28);
                            font97.setBold(true);
                            kmLine->setFont(font97);
                            kmWarningButton->setVisible(true);
                            kmLine->setText(kmControle_String);
                            key->outputText.clear();
                            vehiclekmtr=kmLine->text();
                        }
                    } else {
                    if(USC_is_exist==false)
                    {
                        qDebug()<<"pumpern";
                        StoredScreenRFID_fromMaster=2; //vehicle
                        TAG_KM_read=false;//km ekranına gitmezse satış başlar
                        Confirmation_flg=1;
                        ask_nozzle_total_volume=true;
                        KM_Confirmation_flg=true;
                        Sale_Error_code=2;
                        Update_flg=true;
                        LCD_refresh_flg=1;
                        plate_entered_with_button=false;
                        TAG_read=false;
                        Card_read_flg=0;
                        if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                        settings.setlog("SATIS_DEBUG AFTER_KM SATIS AUTHORIZE GONDER");
                        Confirmation_flg_ok=true;
                        LCD_status=1;

                    }
                    else
                    {
                        qDebug()<<"serbest müşteri2";
                        Confirmation_flg_without_km=false;  ////sonradan
                        KM_Confirmation_flg=true;
                        Confirmation_flg_ok=true;
                    }
                    }

                } else {
                    qDebug()<<"özel müşteri";
                    if(customerCard==true)
                    {qDebug()<<"özel müşteri2";
                        if(MWdatabase->getKm(CustomerID) <= vehiclekmtr.toInt())
                        {

                            MWdatabase->updateKm(vehiclekmtr);
                            customerCard=false;
                            kmConfirm_flg=true;
                            LCD_status=1;
                        }
                        else
                        {
                            kmConfirm_flg=false;
                            QFont font97;
                            font97.setPointSize(28);
                            font97.setBold(true);
                            kmLine->setFont(font97);
                            kmWarningButton->setVisible(true);
                            kmLine->setText(kmControle_String);
                            key->outputText.clear();
                            vehiclekmtr=kmLine->text();
                        }

                    }
                    else
                    {
                        qDebug()<<"özel müşteri3"<<StoredScreenRFID_fromMaster;
                        kmConfirm_flg=false;

                        if(MWdatabase->getKm(Customer_plate) <= vehiclekmtr.toInt())
                        {
                            MWdatabase->updateKm(vehiclekmtr);
                            kmConfirm_flg=true;
                            LCD_status=1;
                            StoredScreenRFID_fromMaster=3;
                        }
                        else
                        {
                            kmConfirm_flg=false;
                            QFont font99;
                            font99.setPointSize(28);
                            font99.setBold(true);
                            kmLine->setFont(font99);
                            kmWarningButton->setVisible(true);
                            kmLine->setText(kmControle_String);
                            key->outputText.clear();
                            vehiclekmtr=kmLine->text();
                        }
                    }

                }

            } else {
                if(customerCard==true)
                {
                    if(MWdatabase->getKm(CustomerID) <= vehiclekmtr.toInt())
                    {

                        MWdatabase->updateKm(vehiclekmtr);
                        customerCard=false;
                        kmConfirm_flg=true;
                        LCD_status=1;
                    }
                    else
                    {
                        kmConfirm_flg=false;
                        QFont font97;
                        font97.setPointSize(28);
                        font97.setBold(true);
                        kmLine->setFont(font97);
                        kmWarningButton->setVisible(true);
                        kmLine->setText(kmControle_String);
                        key->outputText.clear();
                        vehiclekmtr=kmLine->text();
                    }

                } else {
                    kmConfirm_flg=false;

                    if(MWdatabase->getKm(Customer_plate) <= vehiclekmtr.toInt())
                    {
                        MWdatabase->updateKm(vehiclekmtr);
                        kmConfirm_flg=true;
                        LCD_status=1;
                        if(StoredScreenRFID_fromMaster!=2){
                        StoredScreenRFID_fromMaster=3;
                        }
                    }
                    else
                    {
                        kmConfirm_flg=false;
                        QFont font99;
                        font99.setPointSize(28);
                        font99.setBold(true);
                        kmLine->setFont(font99);
                        kmWarningButton->setVisible(true);
                        kmLine->setText(kmControle_String);
                        key->outputText.clear();
                        vehiclekmtr=kmLine->text();
                    }
                }

            }

            if(kmConfirm_flg==true)
            {
                //Pump_Filling_status=1; //chnn
                kmConfirm_flg=false;

                if(ask_km_after==false)
                {
                    if(USC_is_exist==false)
                    {

//                        if(customerCard==false){
//                            qDebug()<<"buradayken78";
//                           StoredScreenRFID_fromMaster=2;
//                        }

                        //StoredScreenRFID_fromMaster=2; //vehicle
                        TAG_KM_read=false;//km ekranına gitmezse satış başlar
                        Confirmation_flg=1;
                        ask_nozzle_total_volume=true;
                        KM_Confirmation_flg=true;
                        Sale_Error_code=2;
                        Update_flg=true;
                        LCD_refresh_flg=1;
                        plate_entered_with_button=false;
                        TAG_read=false;
                        Card_read_flg=0;
                        if(test)qDebug()<<"SATIS AUTHORIZE GONDER";
                        settings.setlog("SATIS_DEBUG AFTER_KM SATIS AUTHORIZE GONDER");
                        Confirmation_flg_ok=true;
                        LCD_status=1;
                    }
                    else
                    {
                        Confirmation_flg_without_km=false;  ////sonradan
                        KM_Confirmation_flg=true;
                        Confirmation_flg_ok=true;
                    }

                }

            }
        }
        if((LCD_status==15) ||(LCD_status==8)) //şifre giriniz Enter
        {
            if(password_is_deactive==false)
            {
                if(otopswrd==Numpad_Output)
                {
                    if(test) qDebug()<<"otopswrd"<<otopswrd<<LCD_status_last<<LCD_status<<pump[pumpNo].status;
                    USC_com_error_count=0;
                    USC_com_error_flg=false;
                    system_started=false;
                    USC_is_exist=false;

                    if((pump[pumpNo].laststatus==2) || (pump[pumpNo].laststatus==6)|| (pump[pumpNo].laststatus==3))
                    {
                        Sale_resume_flg=true;
                    }
                    if((LCD_status_last==1) || (LCD_status_last==2)|| (LCD_status_last==11)|| (LCD_status_last==7)){LCD_status=LCD_status_last; LCD_refresh_flg=1;}
                    if(LCD_status_last==8){LCD_status=LCD_status_last; LCD_refresh_flg=1;} //sonradan
                    if(pump[pumpNo].status==0){LCD_status=1;LCD_refresh_flg=1;}
                    sifre.clear();
                    for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                    {
                        TEXT_fromMaster[jj]=0x20;
                    }
                    cannotchange_frame=false;
                    password_frame->setVisible(false);
                }
                else
                {

                    Numpad_Output.clear();
                    paswrd_label->setText(wrongPassword_String);
                    passwrdLine->setText(Numpad_Output);
                }

            }

        }


        }
        if(LCD_status==35)
        {
            if(Numpad_Output.length()==16)
            {
                QString date_clock=passwrdLine->text();
                QStringList date_clock_list=date_clock.split(" ");
                if(date_clock_list.length()==2)
                {
                    QString date=date_clock_list.at(0);
                    QString clock=date_clock_list.at(1);
                    QStringList date_list=date.split("/");
                    QStringList clock_list=clock.split(":");
                    QString day=date_list.at(0);
                    QString month=date_list.at(1);
                    QString year=date_list.at(2);
                    QString hour=clock_list.at(0);
                    QString minute=clock_list.at(1);
                    if(QDate::isValid(year.toInt(),month.toInt(),day.toInt()))
                    {
                        if(QTime::isValid(hour.toInt(),minute.toInt(),0,0))
                        {
                            QDate dnow(2018,03,13);
                            QDate enow(year.toInt(),month.toInt(),day.toInt());
                            //qDebug()<<"Today is :"<<dnow.toString("dd.MM.yyyy")<<"Days :"<<dnow.daysTo(enow);

                            if(dnow.daysTo(enow)<0)
                            {
                                Numpad_Output=enterValidDate_String;
                                passwrdLine->setText(Numpad_Output);
                                saat_tarih_error=true;
                            }
                            else
                            {
                                date=month+"/"+day+"/"+year+" "+hour+":"+minute;
                                QProcess proc;
                                proc.start("date -s \""+date+":00"+"\"");
                                proc.waitForFinished();
                                system("hwclock -w");
                                LCD_status=31;
                                Numpad_Output=systemStarting_String;
                                passwrdLine->setText(Numpad_Output);
                            }
                        }
                        else
                        {
                            Numpad_Output=incorrectTime_String;
                            passwrdLine->setText(Numpad_Output);
                            saat_tarih_error=true;
                        }
                    }
                    else
                    {
                        Numpad_Output=incorrectDate_String;
                        passwrdLine->setText(Numpad_Output);
                        saat_tarih_error=true;
                    }
                }
            }
        }

        else
        {
            if(LCD_status==40)
            { qDebug()<<"ok4"<<Numpad_Output.length();
                if(Numpad_Output.length()==16)
                { qDebug()<<"girdi";
                    QString date_clock=numpadDateTimeEdit->text();
                    qDebug()<<date_clock;
                    QStringList date_clock_list=date_clock.split(" ");
                    if(date_clock_list.length()==2)
                    {
                        QString date=date_clock_list.at(0);
                        QString clock=date_clock_list.at(1);
                        QStringList date_list=date.split("/");
                        QStringList clock_list=clock.split(":");
                        QString day=date_list.at(0);
                        QString month=date_list.at(1);
                        QString year=date_list.at(2);
                        QString hour=clock_list.at(0);
                        QString minute=clock_list.at(1);
                        if(QDate::isValid(year.toInt(),month.toInt(),day.toInt()))
                        {
                            if(QTime::isValid(hour.toInt(),minute.toInt(),0,0))
                            {
                                QDate dnow(2018,03,13);
                                QDate enow(year.toInt(),month.toInt(),day.toInt());
                                //qDebug()<<"Today is :"<<dnow.toString("dd.MM.yyyy")<<"Days :"<<dnow.daysTo(enow);

                                if(dnow.daysTo(enow)<0)
                                {
                                    Numpad_Output=enterValidDate_String;
                                    numpadDateTimeEdit->setText(Numpad_Output);
                                    saat_tarih_error=true;
                                }
                                else
                                {
                                    date=month+"/"+day+"/"+year+" "+hour+":"+minute;
                                    QProcess proc;
                                    proc.start("date -s \""+date+":00"+"\"");
                                    proc.waitForFinished();
                                    system("hwclock -w");
                                    LCD_status=31;
                                    Numpad_Output=systemStarting_String;
                                    numpadDateTimeEdit->setText(Numpad_Output);
                                }
                            }
                            else
                            {
                                Numpad_Output=incorrectTime_String;
                                numpadDateTimeEdit->setText(Numpad_Output);
                                saat_tarih_error=true;
                            }
                        }
                        else
                        {
                            Numpad_Output=incorrectDate_String;
                            numpadDateTimeEdit->setText(Numpad_Output);
                            saat_tarih_error=true;
                        }
                    }
                }
            }
            else
            {
                Numpad_Output.clear();
                numpadDateTimeEdit->setText(Numpad_Output);
                Mwindowlock=true;
                Mwindowtimer->start(locktimer);
            }
            Numpad_Output.clear();
            passwrdLine->setText(Numpad_Output);
            Mwindowlock=true;
            Mwindowtimer->start(locktimer);
        }

}

void MainWindow::Numpad_Cancel_Clicked()
{
    if(Mwindowlock==false)
    {
        BuzzerON();
        if(test) qDebug()<<"Numpad_Cancel_Clicked"<<Numpad_Output;
        if(LCD_status==40){
            numpadDateTimeEdit->setVisible(false);
            numpadframe->setVisible(false);
            keyboard_frame->setVisible(false);
            setDate_pushbutton->setVisible(true);
            saat_1_lineEdit->setVisible(true);
            tarih_1_lineEdit->setVisible(true);
            saat_sor->setVisible(true);
            tarih_sor->setVisible(true);
            menu_enter->setVisible(false);
            menu_cancel->setVisible(false);
    }
        if(LCD_status!=15)
        {
            if(LCD_status==17){Unit_price_for_save[screen_UP_nozzle]=Lastprice[screen_UP_nozzle];}
            if(LCD_status==2){ Card_read_flg=0;}
            if(Pump_Filling_status!=1 && ((LCD_status==1) || (LCD_status==2) || (LCD_status==4) || (LCD_status==12) || (LCD_status==14)|| (LCD_status==15) || (LCD_status==16) || (LCD_status==17)   )) //satış esnasında
            {
                if(meter_com_err==false)
                {
                    Customer_plate.clear();
                    Keypad_data_for_LCD.clear();
                    CustomerID.clear();
                    for(int jj=0; jj<16;jj++)
                    {
                        TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                    }
                    if(test)qDebug()<<"............11111111";
                    qDebug()<<"cl8";
                    vehiclekmtr.clear();
                    sifre.clear();
                    Cancel_flg=1;
                    Enter_flg=0;
                    usleep(300);
                    LCD_refresh_flg=1;
                    tag_read_count=4;
                    Tag_read_count_flg=0;
                    Tag_read_flg=0;
                    NewPlateExist=0;
                    if(LCD_status!=1 && LCD_status!=12 && LCD_status!=14 && LCD_status!=15 && LCD_status!=16 && LCD_status!=17){
                        delete_last_sale_flg=1;
                    }
                }
                if(last_screen_flg==13){
                    LCD_status=13;
                    LCD_refresh_flg=1;
                    last_screen_flg=0;
                }
                else{
                    LCD_status=1;
                    LCD_refresh_flg=1;
                }
            }
            Numpad_Output.clear();

        }
        if(LCD_status==15) //şifre linedit
        {
            if(system_lock==true)
            {
                if(test) qDebug()<<"SİSTEM KİLİTLENDİ";
                password_frame->setVisible(false);
            }
            else
            {
                if(password_is_deactive==false)
                {
                    paswrd_label->setText(automationErrorEnterPassword_String);
                    passwrdLine->setText(Numpad_Output);
                    if(test) qDebug()<<"OTOMASYON HATASI";
                }
                else
                {
                    if(test) qDebug()<<"otopswrd"<<otopswrd<<LCD_status_last<<LCD_status<<pump[pumpNo].status;
                    USC_com_error_count=0;
                    USC_com_error_flg=false;
                    system_started=false;
                    USC_is_exist=false;
                    //pump[pumpNo].status=pump[pumpNo].laststatus;
                    if((pump[pumpNo].laststatus==2) || (pump[pumpNo].laststatus==6)|| (pump[pumpNo].laststatus==3))
                    {
                        Sale_resume_flg=true;
                    }
                    if((LCD_status_last==1) || (LCD_status_last==2)|| (LCD_status_last==11)|| (LCD_status_last==7)){LCD_status=LCD_status_last; LCD_refresh_flg=1;}
                    if(LCD_status_last==8){LCD_status=LCD_status_last; LCD_refresh_flg=1;} //sonradan
                    if(pump[pumpNo].status==0){LCD_status=1;LCD_refresh_flg=1;}
                    sifre.clear();
                    for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                    {
                        TEXT_fromMaster[jj]=0x20;
                    }
                    cannotchange_frame=false;
                    password_frame->setVisible(false);

                }
            }
        }
        if(LCD_status==11) //şifre linedit
        {
            kmLine->clear();
            key->outputText.clear();
            QFont font94;
            font94.setPointSize(28);
            font94.setBold(true);
            kmLine->setFont(font94);
            kmLine->setText(pleaseEnterKm_String);
            kmWarningButton->setVisible(true);
        }
        if((LCD_status==1) && (NewPlateExist==0) ||((LCD_status==13) && (system_lock==false)))
        {
            if(menutabwidgetindex==0)
            {
                password_frame->setVisible(false);
                numpadframe->setVisible(false);
            }
            else
            {
                password_frame->setVisible(false);
                numpadframe->setVisible(false);
                menu_frame->setVisible(false);
                if(menutabwidgetindex==3)
                {
                    menu_frame->setGeometry(QRect(0, 0, 800, 480));
                    menu_tabWidget->setGeometry(QRect(5, 5, 800, 355));
                    keyboard_frame->setGeometry(QRect(0, 150, 800, 330));
                    key->setGeometry(QRect(QPoint(0,0),QSize(800,355)));
                    keyboard_frame->setVisible(false);
                    numpadframe->setGeometry(QRect(0, 145, 801, 331));
                    numpadframe->setVisible(false);
                    menu_enter->setVisible(true);
                    menu_cancel->setVisible(true);
                    password_frame->setVisible(false);

                }
                if(write_configuration_active)
                {
                    access_point_info_label->setVisible(true);
                    access_point_read_pushbutton->setVisible(true);
                    access_point_write_pushbutton->setVisible(true);
                    tts_active_pushbutton->setVisible(true);
                    menu_enter->setVisible(true);
                    menu_cancel->setVisible(true);
                    otomasyon_adres_label->setVisible(true);
                    nozzle_adres_label->setVisible(true);
                    channel_label->setVisible(true);
                    pause_time_label->setVisible(true);
                    otomasyon_adres_new_label->setVisible(false);
                    nozzle_adres_new_label->setVisible(false);
                    channel_new_label->setVisible(false);
                    pause_time_new_label->setVisible(false);
                    otomasyon_adres_lineEdit->setVisible(false);
                    nozzle_adres_lineEdit->setVisible(false);
                    channel_lineEdit->setVisible(false);
                    pause_time_lineEdit->setVisible(false);
                    otomasyon_adres_pushbutton->setVisible(false);
                    nozzle_adres_pushbutton->setVisible(false);
                    pause_time_pushbutton->setVisible(false);
                    channel_pushbutton->setVisible(false);
                    numpadframe->setVisible(false);
                    write_configuration_active=false;
                }
            }
            cannotchange_frame=false;
        }
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
    }
}


void MainWindow::Numpad_Clear_Clicked()
{
    if(keyboardcomingflag==9){
        BuzzerON();
        Mwindowlock=true;
        Numpad_Output.remove((Numpad_Output.length()-1),1);
        numpadLineEdit->setText(Numpad_Output);
        usc_ip_lineedit->setText(Numpad_Output);
        Mwindowtimer->start(locktimer);
    }
    else if(keyboardcomingflag==10){
        BuzzerON();
        Mwindowlock=true;
        Numpad_Output.remove((Numpad_Output.length()-1),1);
        numpadLineEdit->setText(Numpad_Output);
        usc_port_lineedit->setText(Numpad_Output);
        Mwindowtimer->start(locktimer);
    }

    else if(keyboardcomingflag==11){
        BuzzerON();
        Mwindowlock=true;
        Numpad_Output.remove((Numpad_Output.length()-1),1);
        numpadLineEdit->setText(Numpad_Output);
        subnetMax_lineedit->setText(Numpad_Output);
        Mwindowtimer->start(locktimer);
    }

    else{
    if(Mwindowlock==false)
    {
        BuzzerON();
        Mwindowlock=true;
        Numpad_Output.remove((Numpad_Output.length()-1),1);
        if(test) qDebug()<<"Numpad_Cancel_Clicked"<<Numpad_Output<<LCD_status;
        if(LCD_status==15) //şifre linedit
        {
            if(password_is_deactive==false)
            {
                paswrd_label->setText(automationErrorEnterPassword_String);
                passwrdLine->setText(Numpad_Output);
            }
            else
            {
                if(test) qDebug()<<"otopswrd"<<otopswrd<<LCD_status_last<<LCD_status<<pump[pumpNo].status;
                USC_com_error_count=0;
                USC_com_error_flg=false;
                system_started=false;
                USC_is_exist=false;
                //pump[pumpNo].status=pump[pumpNo].laststatus;
                if((pump[pumpNo].laststatus==2) || (pump[pumpNo].laststatus==6)|| (pump[pumpNo].laststatus==3))
                {
                    Sale_resume_flg=true;
                }
                if((LCD_status_last==1) || (LCD_status_last==2)|| (LCD_status_last==11)|| (LCD_status_last==7)){LCD_status=LCD_status_last; LCD_refresh_flg=1;}
                if(LCD_status_last==8){LCD_status=LCD_status_last; LCD_refresh_flg=1;} //sonradan
                if(pump[pumpNo].status==0){LCD_status=1;LCD_refresh_flg=1;}
                sifre.clear();
                for(quint8 jj=0;jj<TEXT_fromMaster.length();jj++)
                {
                    TEXT_fromMaster[jj]=0x20;
                }
                cannotchange_frame=false;
                password_frame->setVisible(false);
            }


        }
        if(LCD_status==11)
        {
            kmLine->setText(Numpad_Output);
        }
        if(LCD_status==1 || LCD_status==13)
        {
            passwrdLine->setText(Numpad_Output);
            if(keyboardcomingflag==3)
            {
                sifre_1_lineEdit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==4)
            {
                sifre_2_lineEdit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==9)
            {
                usc_ip_lineedit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==10)
            {
                usc_port_lineedit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==20)
            {
                otomasyon_adres_lineEdit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==21)
            {
                nozzle_adres_lineEdit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==22)
            {
                channel_lineEdit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==23)
            {
                pause_time_lineEdit->setText(Numpad_Output);
            }
            else if(keyboardcomingflag==24)
            {
                numpadDateTimeEdit->setText(Numpad_Output);
            }
        }
        if(LCD_status==35)
        {
            if(Numpad_Output.length()==2 || Numpad_Output.length()==5 || Numpad_Output.length()==10 || Numpad_Output.length()==13)
            {
                Numpad_Output.remove((Numpad_Output.length()-1),1);
            }
            if(saat_tarih_error)
            {
                Numpad_Output.clear();
                saat_tarih_error=false;
            }
            passwrdLine->setText(Numpad_Output);
        }
        if(LCD_status==40)
        {
            if(Numpad_Output.length()==2 || Numpad_Output.length()==5 || Numpad_Output.length()==10 || Numpad_Output.length()==13)
            {
                Numpad_Output.remove((Numpad_Output.length()-1),1);
            }
            if(saat_tarih_error)
            {
                Numpad_Output.clear();
                saat_tarih_error=false;
            }
            numpadDateTimeEdit->setText(Numpad_Output);
        }
        Mwindowtimer->start(locktimer);
    }
    }
}

void MainWindow::Enter_Plate_Clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        if(allow_button_plate==true)
        {
            if((LCD_status==1) && (protect_flg==false)&& (StoredScreenRFID_fromMaster!=5) && (StoredScreenRFID_fromMaster!=6) && (StoredScreenRFID_fromMaster!=8))
            {
                Keypad_data_for_LCD.clear();
                plateLine->setText("");
                for(int pp=1;pp<=Number_of_Nozzle;pp++)
                {
                    pump[pumpNo].nozzle[pp].PLATE.clear();
                }
                pump[pumpNo].PLATE.clear();
                Customer_plate.clear();
                Plate_screen_flg=true;
                key->setGeometry(QRect(QPoint(0,0),QSize(800,355)));
                keyboard_frame->setGeometry(QRect(0, 125, 800, 355));
            }
        }
        Mwindowtimer->start(locktimer);
    }
}
void MainWindow::Enter_Record_Clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if(((LCD_status==1) ||((LCD_status==13) && (system_lock==false)) ||(config_error==true))&& (NewPlateExist==0))
        {
            main_frame->close();
            Records_Frame->show();
            Whitelist_Frame->close();
            Person_Frame->close();
            Sales_Frame->close();
            Nozzle_Frame->close();
            back_from_records_button->setVisible(true);
            back_from_Table_button->setVisible(false);
        }
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}


void MainWindow::back_from_records_button_Clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Records_Frame->close();
        cannotchange_frame=false;
        main_frame->show();
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::records_button_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if(test) qDebug()<<"show_Whitelist_error1";
        Nozzlemodel->setQuery(MWdatabase->Show_Nozzle());
        if(test) qDebug()<<"show_person_error2";
        Nozzletable->setModel(Nozzlemodel);
        if(test) qDebug()<<"show_person_error3";
        Nozzletable->resizeColumnsToContents();
        if(test) qDebug()<<"show_person_error4";
        Nozzle_Frame->show();
        back_from_records_button->setVisible(false);
        back_from_Table_button->setVisible(true);
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }

}

void MainWindow::whitelist_button_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if(test) qDebug()<<"show_Whitelist_error1";
        Whitelistmodel->setQuery(MWdatabase->Show_Whitelist());
        if(test) qDebug()<<"show_person_error2";
        Whitelisttable->setModel(Whitelistmodel);
        if(test) qDebug()<<"show_person_error3";
        Whitelisttable->resizeColumnsToContents();
        if(test) qDebug()<<"show_person_error4";
        Whitelist_Frame->show();
        back_from_records_button->setVisible(false);
        back_from_Table_button->setVisible(true);
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::sales_button_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if(test) qDebug()<<"show_sales_error1";
        Salesmodel->setQuery(MWdatabase->Show_Sales());
        if(test) qDebug()<<"show_sales_error2";
        Salestable->setModel(Salesmodel);
        if(test) qDebug()<<"show_sales_error3";
        Salestable->resizeColumnsToContents();
        if(test) qDebug()<<"show_sales_error4";
        Sales_Frame->show();
        back_from_records_button->setVisible(false);
        back_from_Table_button->setVisible(true);
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::worker_button_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if(test) qDebug()<<"show_person_error1";
        Personmodel->setQuery(MWdatabase->Show_Person());
        if(test) qDebug()<<"show_person_error2";
        Persontable->setModel(Personmodel);
        if(test) qDebug()<<"show_person_error3";
        Persontable->resizeColumnsToContents();
        if(test) qDebug()<<"show_person_error4";
        Person_Frame->show();
        back_from_records_button->setVisible(false);
        back_from_Table_button->setVisible(true);
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}



void MainWindow::keyboardback()
{
    if(test) qDebug()<<"Keyboardback_Clicked"<<Numpad_Output<<LCD_status;
    if(LCD_status==17){Unit_price_for_save[screen_UP_nozzle]=Lastprice[screen_UP_nozzle];}
    if(Pump_Filling_status!=1 && ((LCD_status==1) || (LCD_status==4) || (LCD_status==12) || (LCD_status==14)|| (LCD_status==15) || (LCD_status==16) || (LCD_status==17)   )) //satış esnasında
    {
        if(meter_com_err==false)
        {
            Customer_plate.clear();
            Keypad_data_for_LCD.clear();
            CustomerID.clear();
            for(int jj=0; jj<16;jj++)
            {
                TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
            }
            if(test)qDebug()<<"............22222222";
            vehiclekmtr.clear();
            sifre.clear();
            Cancel_flg=1;
            Enter_flg=0;
            usleep(300);
            LCD_refresh_flg=1;
            tag_read_count=4;
            Tag_read_count_flg=0;
            Tag_read_flg=0;
            NewPlateExist=0;
            Person_is_Ok=false;
            if(LCD_status!=1 && LCD_status!=12 && LCD_status!=14 && LCD_status!=15 && LCD_status!=16 && LCD_status!=17){
                delete_last_sale_flg=1;
            }
        }
        if(last_screen_flg==13){
            LCD_status=13;
            LCD_refresh_flg=1;
            last_screen_flg=0;
        }
        else{
            LCD_status=1;
            LCD_refresh_flg=1;
        }
    }
    if(LCD_status==2){
        Card_read_flg=0;
        if(plate_entered_with_button==true)
        {
            if(meter_com_err==false)
            {
                Customer_plate.clear();
                Keypad_data_for_LCD.clear();
                CustomerID.clear();
                for(int jj=0; jj<16;jj++)
                {
                    TAGID[DCR_par.DCR_Nozzle_Number][jj]=0;
                }
                if(test)qDebug()<<"............33333333";
                qDebug()<<"cl11";
                vehiclekmtr.clear();
                sifre.clear();
                Cancel_flg=1;
                Enter_flg=0;
                usleep(300);
                LCD_refresh_flg=1;
                tag_read_count=4;
                Tag_read_count_flg=0;
                Tag_read_flg=0;
                NewPlateExist=0;
                if(LCD_status!=1 && LCD_status!=12 && LCD_status!=14 && LCD_status!=15 && LCD_status!=16 && LCD_status!=17){
                    delete_last_sale_flg=1;
                }
            }
            if(last_screen_flg==13){
                LCD_status=13;
                LCD_refresh_flg=1;
                last_screen_flg=0;
            }
            else{
                LCD_status=1;
                LCD_refresh_flg=1;
            }


        }
    }
    else
    {
        plateLine->clear();
        kmLine->clear();
        key->outputText.clear();
    }

}


void MainWindow::keyboardvalue(QString text)
{
    if(test) qDebug()<<"Keyboardvalue_Clicked"<<key->outputText<<LCD_status;
    if(LCD_status==2)
    {

        if(key->outputText.length()>=2)
        {
            if((key->outputText[key->outputText.length()-1].isNumber())&&(key->outputText[key->outputText.length()-2].isLetter()))
            {

                key->outputText.insert(key->outputText.length()-1," ");
            }

            if((key->outputText[key->outputText.length()-2].isNumber())&&(key->outputText[key->outputText.length()-1].isLetter()))
            {

                key->outputText.insert(key->outputText.length()-1," ");
            }
        }
        plateLine->setText(key->outputText);

    }


    if(test) qDebug()<<"KEYBOARDVALUE"<<key->outputText;
    if(LCD_status==1)
    {
        switch (keyboardcomingflag) {
        case 0:
            //plateLine->setText(plateLine->text().append(key->outputText));
            //key->outputText.clear();
            break;
        case 1://ip
            //ip_setup_lineedit->setText(ip_setup_lineedit->text().append(key->outputText));
            //key->outputText.clear();
            break;
        case 2://apn
            //apn_setup_lineedit->setText(apn_setup_lineedit->text().append(key->outputText));
            //key->outputText.clear();
            break;
        case 3://user
            //sifre_1_lineEdit->setText(key->outputText);
            //key->outputText.clear();
            break;
        case 4://password
            //receipt_2_lineEdit->setText(key->outputText);
            //key->outputText.clear();
            break;
        case 5:
            receipt_1_lineEdit->setText(key->outputText);
            //key->outputText.clear();
            break;
        case 6:
            receipt_2_lineEdit->setText(key->outputText);
            //key->outputText.clear();
            break;
        case 7:
            receipt_3_lineEdit->setText(key->outputText);
            //key->outputText.clear();
            break;
        case 8:
            receipt_4_lineEdit->setText(key->outputText);
            //key->outputText.clear();
            break;

        default:
            break;

        }
    }
}


void MainWindow::keyboardEntervalue()
{
    Enter_flg=1;
    if(test) qDebug()<<"Keyboardenter_Clicked"<<key->outputText<<LCD_status;

    if(LCD_status==1)
    {
        Enter_flg=0;
        givecopyflg=true;
        printing_header_data[0]=receipt_1_lineEdit->text();
        printing_header_data[1]=receipt_2_lineEdit->text();
        End_info[0]=receipt_3_lineEdit->text();
        End_info[1]=receipt_4_lineEdit->text();
    }
    if(LCD_status==2) //plaka giriniz enter
    {
        StatusLine.clear();
        LCD_status=1;//sonradan chn
        StoredScreenRFID_fromMaster=11;
        Customer_plate=plateLine->text().toLatin1();
        plateLine->clear();
        key->outputText.clear();
        Customer_plate.replace(" ","");
        customerPlate=QString::fromUtf8(Customer_plate);
        if((Customer_plate.length()>0) && (Customer_plate.length()-Customer_plate.count('\x0'))>0)
        {


            if(plate_entered_with_button==true)
            {

                qDebug()<<"plate entered true";
                pump[pumpNo].PLATE=Customer_plate;

                if(Nozzle_up_flg==1)
                {
                    for(int pl=0; pl<12;pl++)
                    {
                        pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                    }
                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=Customer_plate;
                }
            }
            else
            {

                qDebug()<<"plate entered false";
                for(int pl=0; pl<12;pl++)
                {
                    pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE[pl]=0x20;
                }
                pump[pumpNo].nozzle[DCR_par.DCR_Nozzle_Number].PLATE=Customer_plate;
            }
            qDebug()<<"plate entered";
            Plate_control_flg=1;  NewPlateExist=1;
            if(test)qDebug()<<"keypad-CUSTOMER_PLATE"<<Customer_plate;
            settings.setlog("SATIS_DEBUG keypad-CUSTOMER_PLATE "+QString::fromUtf8(Customer_plate.toHex()));
//            if(MWdatabase->kontrolSalesPlate(Customer_plate)==false){
//                qDebug()<<"pumpery";
//                StoredScreenRFID_fromMaster=2;
//            }
        }

    }

    if(LCD_status==17)  //birim fiyat giriniz ekranı enter
    {

        if(Unit_price_for_save[screen_UP_nozzle].length()>0 && Unit_price_for_save[screen_UP_nozzle].toFloat()!=0)
        {
            LCD_status=17;
            LCD_refresh_flg=1;
            if(test)qDebug()<<"KEYPAD-Unit_price_for_save "+QString::number(screen_UP_nozzle)<<Unit_price_for_save[screen_UP_nozzle];
            settings.setlog("SATIS_DEBUG KEYPAD-Unit_price_for_save "+QString::number(screen_UP_nozzle)+"  "+QString::fromUtf8(Unit_price_for_save[screen_UP_nozzle].toHex()));
            saveUnitPrice(screen_UP_nozzle);
            qDebug()<<"CAGIRRR22";
            Unit_price=convert_format(Unit_price_for_save[screen_UP_nozzle],unit_price_dot_index);
            qDebug()<<"CAGIRRR22----";
            dec_to_bcd(Unit_price.toInt()); // 4 byte 00 00 12 34
            pump[pumpNo].nozzle[screen_UP_nozzle].unitprice[0]=bcd[2];
            pump[pumpNo].nozzle[screen_UP_nozzle].unitprice[1]=bcd[1];
            pump[pumpNo].nozzle[screen_UP_nozzle].unitprice[2]=bcd[0];

        }

        if(screen_UP_nozzle<Number_of_Nozzle)
        {
            screen_UP_nozzle++;
            Lastprice[screen_UP_nozzle]=Unit_price_for_save[screen_UP_nozzle];
            Unit_price_for_save[screen_UP_nozzle].clear();
        }
        else
        {
            LCD_status=18;
            LCD_refresh_flg=1;
            Update_flg=true;
            //QThread::msleep(1500);
            LCD_status=1;
            LCD_refresh_flg=1;
            screen_UP_nozzle=1;
            sifre.clear();
        }

    }
    if(menu_frame->isVisible())
    {
        menu_tabWidget->setCurrentIndex(0);
    }

}

void MainWindow::keyboardClearvalue()
{
    Clear_flg=1;
    LCD_refresh_flg=1;
    if(LCD_status==2) //plaka giriniz clear
    {
        Customer_plate.remove(Customer_plate.length()-1,1);
    }
    if(LCD_status==11) //km giriniz clear
    {
        vehiclekmtr.remove(vehiclekmtr.length()-1,1);
    }
    if(LCD_status==15) //şifre giriniz clear
    {
        sifre.remove(sifre.length()-1,1);
    }
    if(LCD_status==17) //uprice clear
    {
        Unit_price_for_save[screen_UP_nozzle].remove(Unit_price_for_save[screen_UP_nozzle].length()-1,1);
    }
    LCD_refresh_flg=1;

}

void MainWindow::menu_button_Clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        if((((LCD_status==1) ||((LCD_status==13) && (system_lock==false))) && (config_error==false)) && (NewPlateExist==0))
        {
            cannotchange_frame=true;
            menutabwidgetindex=0;
            menu_tabWidget->setCurrentIndex(0);
            keyboardcomingflag=0;
            paswrd_label->setText(enterMenuPassword_String);
            paswrd_label->setStyleSheet(QLatin1String("background-color: rgb(142, 142, 142);\n"
                                                      "color: rgb(24, 24, 24);\n"
                                                      "border:1px solid;\n"
                                                      "border-radius:5px;\n"
                                                      "border-color: rgb(24, 24, 24);"));
            passwrdLine->setText("");
            Numpad_Output.clear();
            menu_passwrd=true;
            password_frame->setVisible(true);
            pmenter_frame->setVisible(true);
            numpadframe->setVisible(true);
            numpadframe->setGeometry(0,130,801,350);
            numpadframe->raise();

        }

        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}


void MainWindow::menu_enter_button_Clicked()
{

    if(Mwindowlock==false)
    {
        //QSettings settings(settingPath, QSettings::IniFormat);
        printing_header_data[0]=receipt_1_lineEdit->text();
        printing_header_data[1]=receipt_2_lineEdit->text();
        End_info[0]=receipt_3_lineEdit->text();
        End_info[1]=receipt_4_lineEdit->text();

        for(int ll=1; ll<=Nozzle_Num_spinBox->value();ll++)
        {
            MWdatabase->updateProductname(pump[pumpNo].nozzle[ll].nozzle_id,product_id[product_comboBox[ll-1]->currentIndex()+1],new_unit_price[product_comboBox[ll-1]->currentIndex()+1],tank_no[product_comboBox[ll-1]->currentIndex()+1]);
        }
        settings.setValue("ABU_address",adress_spinBox->value());

        MWdatabase->updateSettings(CardR_Address_spinBox->value(),Upricedotindex_spinBox->value()
                                   ,Amountdotindex_spinBox->value(),Volumedotindex_spinBox->value()
                                   ,receipt_1_lineEdit->text(),receipt_2_lineEdit->text()
                                   ,receipt_3_lineEdit->text(),receipt_4_lineEdit->text()
                                   ,kmsor_check->isChecked(),freeSaleCheck->isChecked()
                                   ,plateSaleCheck->isChecked());

        settings.setValue("commokpswrd",sifre_1_lineEdit->text());
        settings.setValue("otopswrd",sifre_2_lineEdit->text());
        settings.setValue("usc_ip",usc_ip_lineedit->text());
        settings.setValue("usc_port",usc_port_lineedit->text());
        settings.setValue("GateWay",subnetMax_lineedit->text());
        if(DinamikIP_check->checkState()==Qt::Checked){
            NetworkFile->writeDHCP();
        }
        else{
            NetworkFile->writeStatic(usc_ip_lineedit->text(),usc_port_lineedit->text(),subnetMax_lineedit->text());
        }
        sync();
        system("sync");
        key->outputText.clear();
        status_label->setText(settingsSaved_String);
        warning_label->setText(systemWillBeRestarted_String);
        menu_frame->close();
        password_frame->setVisible(false);
        numpadframe->setVisible(false);
        menutabwidgetindex=0;
        LCD_status=31;
        menu_tabWidget->setCurrentIndex(0);
        test_modu=false;
        write_configuration_active=false;
        menu_passwrd=false;
        BuzzerON();
        QThread::msleep(1000);
    }
}


void MainWindow::menu_cancel_button_Clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        MWdatabase->SettingsValues();
        cannotchange_frame=false;
        menu_passwrd=false;
        test_modu=false;
        write_configuration_active=false;
        testmodestart->setText(startTest_String);
        cardreader_test_status->setVisible(false);
        accesspoint_test_status->setVisible(false);
        printer_test_status->setVisible(false);
        key->outputText.clear();
        menu_frame->close();
        menu_tabWidget->setCurrentIndex(0);
        Mwindowtimer->start(locktimer);
        BuzzerON();
    }
}
void MainWindow::getTabNo()
{
    menu_tabWidget->raise();
    menutabwidgetindex=menu_tabWidget->currentIndex();
    //tabNo2->setText(menu_tabWidget->tabText(menu_tabWidget->currentIndex()));
    if(menutabwidgetindex==1)
    {
        menu_frame->setGeometry(QRect(0, 0, 800, 480));
        menu_tabWidget->setGeometry(QRect(5, 5, 800, 480));
        keyboard_frame->setGeometry(0,170,800,310);
        key->setGeometry(QRect(QPoint(0,0),QSize(800,310)));
        keyboard_frame->setVisible(true);
        keyboard_frame->raise();
        menu_enter->setVisible(false);
        menu_cancel->setVisible(false);

    }
//    else if(menutabwidgetindex==3)
//    {
//        menu_enter->setVisible(true);
//        menu_cancel->setVisible(true);
//        numpadframe->setVisible(false);
//        keyboard_frame->setVisible(false);
//        /*menu_tabWidget->setGeometry(QRect(10, 10, 780, 240));
//        menu_frame->setGeometry(QRect(0, 0, 800, 480));
//        numpadframe->setGeometry(0,200,801,285);
//        numpadframe->setVisible(false);
//        numpadframe->raise();
//        keyboard_frame->setVisible(false);
//        menu_enter->setVisible(false);
//        menu_cancel->setVisible(false);*/
//        numpadDateTimeEdit->setVisible(false);
//    }
    else if(menutabwidgetindex==4)
    {   numpadDateTimeEdit->setVisible(false);
        numpadframe->setVisible(false);
        keyboard_frame->setVisible(false);
        setDate_pushbutton->setVisible(true);
        saat_1_lineEdit->setVisible(true);
        tarih_1_lineEdit->setVisible(true);
        saat_sor->setVisible(true);
        tarih_sor->setVisible(true);
        menu_enter->setVisible(false);
        menu_cancel->setVisible(false);
//        menu_frame->setGeometry(QRect(0, 0, 800, 480));
//        menu_tabWidget->setGeometry(QRect(10, 10, 780, 480));
//        numpadframe->setGeometry(0,100,400,250);
//        numpadframe->setVisible(true);
//        numpadframe->raise();
//        menu_enter->setVisible(false);
//        menu_cancel->setVisible(false);

    }
    else if(menutabwidgetindex==5)
    {
        menu_frame->setGeometry(QRect(0, 0, 800, 480));
        menu_tabWidget->setGeometry(QRect(5, 5, 800, 480));
        access_point_info_label->setVisible(true);
        access_point_read_pushbutton->setVisible(true);
        access_point_write_pushbutton->setVisible(true);
        tts_active_pushbutton->setVisible(true);
        menu_enter->setVisible(true);
        menu_cancel->setVisible(true);
        menu_enter->raise();
        menu_cancel->raise();
        otomasyon_adres_label->setVisible(true);
        nozzle_adres_label->setVisible(true);
        channel_label->setVisible(true);
        pause_time_label->setVisible(true);
        otomasyon_adres_new_label->setVisible(false);
        nozzle_adres_new_label->setVisible(false);
        channel_new_label->setVisible(false);
        pause_time_new_label->setVisible(false);
        otomasyon_adres_lineEdit->setVisible(false);
        nozzle_adres_lineEdit->setVisible(false);
        channel_lineEdit->setVisible(false);
        pause_time_lineEdit->setVisible(false);
        otomasyon_adres_pushbutton->setVisible(false);
        nozzle_adres_pushbutton->setVisible(false);
        pause_time_pushbutton->setVisible(false);
        channel_pushbutton->setVisible(false);
        numpadframe->setVisible(false);
        write_configuration_active=false;
        numpadDateTimeEdit->setVisible(false);

    }
    else
    {
        menu_frame->setGeometry(QRect(0, 0, 800, 480));
        menu_tabWidget->setGeometry(QRect(5, 5, 800, 355));
        keyboard_frame->setGeometry(QRect(0, 150, 800, 330));
        key->setGeometry(QRect(QPoint(0,0),QSize(800,355)));
        keyboard_frame->setVisible(false);
        numpadframe->setGeometry(0,165,801,255);
        numpadframe->setVisible(false);
        menu_enter->setVisible(true);
        menu_cancel->setVisible(true);
        numpadDateTimeEdit->setVisible(false);
    }
    testmodestart->setText(startTest_String);
    test_modu=false;
    cardreader_test_status->setVisible(false);
    printer_test_status->setVisible(false);
    accesspoint_test_status->setVisible(false);
    keyboardcomingflag=31;
    usc_port_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    usc_ip_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    sifre_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    receipt_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    receipt_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    receipt_3_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    receipt_4_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    otomasyon_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    nozzle_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    channel_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    pause_time_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    //saat_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
    //tarih_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");

}

void MainWindow::sifre_1_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=sifre_1_lineEdit->text();
        keyboardcomingflag=3;
        sifre_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        sifre_1_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_port_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_ip_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowtimer->start(locktimer);
    }
}


void MainWindow::sifre_2_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=sifre_2_lineEdit->text();
        keyboardcomingflag=4;
        sifre_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        sifre_2_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_port_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        usc_ip_lineedit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::access_point_read_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        access_point_info_label->setText(accesspointSettingsReadingPleaseWait_String);
        read_configuration=true;
        read_configuration_success=0;
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::access_point_write_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        write_configuration_active=true;
        access_point_info_label->setVisible(false);
        access_point_read_pushbutton->setVisible(false);
        access_point_write_pushbutton->setVisible(false);
        tts_active_pushbutton->setVisible(false);
        menu_enter->setVisible(false);
        menu_cancel->setVisible(false);
        otomasyon_adres_label->setVisible(false);
        nozzle_adres_label->setVisible(false);
        channel_label->setVisible(false);
        pause_time_label->setVisible(false);
        otomasyon_adres_new_label->setVisible(true);
        nozzle_adres_new_label->setVisible(true);
        channel_new_label->setVisible(true);
        pause_time_new_label->setVisible(true);
        otomasyon_adres_lineEdit->setVisible(true);
        nozzle_adres_lineEdit->setVisible(true);
        channel_lineEdit->setVisible(true);
        pause_time_lineEdit->setVisible(true);
        otomasyon_adres_pushbutton->setVisible(true);
        nozzle_adres_pushbutton->setVisible(true);
        pause_time_pushbutton->setVisible(true);
        channel_pushbutton->setVisible(true);
        numpadframe->setGeometry(0,180,801,305);
        numpadframe->setVisible(true);
        numpadframe->raise();
        BuzzerON();
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::otomasyon_adres_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=otomasyon_adres_lineEdit->text();
        menu_cancel->setVisible(false);
        menu_enter->setVisible(false);
        keyboard_frame->setVisible(false);
        keyboardcomingflag=20;
        nozzle_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        otomasyon_adres_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        channel_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        pause_time_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");

        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::nozzle_adres_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=nozzle_adres_lineEdit->text();
        menu_cancel->setVisible(false);
        menu_enter->setVisible(false);
        keyboard_frame->setVisible(false);
        keyboardcomingflag=21;
        otomasyon_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        nozzle_adres_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        channel_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        pause_time_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");

        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::channel_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=channel_lineEdit->text();
        menu_cancel->setVisible(false);
        menu_enter->setVisible(false);
        keyboard_frame->setVisible(false);
        keyboardcomingflag=22;
        otomasyon_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        channel_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        nozzle_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        pause_time_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");

        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::pause_time_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        Numpad_Output.clear();
        Numpad_Output=pause_time_lineEdit->text();
        menu_cancel->setVisible(false);
        menu_enter->setVisible(false);
        keyboard_frame->setVisible(false);
        keyboardcomingflag=23;
        otomasyon_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        pause_time_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        channel_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        nozzle_adres_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");

        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::tts_active_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        if(tts_active_pushbutton->text().indexOf("KAPALI")>0)
        {
            tts_active_pushbutton->setText(serviceOpen_String);
            ttspause=true;
            settings.setValue("ttspause",ttspause);
        }
        else
        {
            tts_active_pushbutton->setText(serviceClosed_String);
            ttspause=false;
            settings.setValue("ttspause",ttspause);
        }
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::showModified(QString data)
{
    if(Nozzle_up_flg==0 && pump_sta==false)
    {
        for(int ll=1; ll<=Number_of_Nozzle;ll++)
        {
            QString price=MWdatabase->get_Unit_Price(ll);
            //qDebug()<<"price : "<<price;
            settings.setValue("Unit_price_"+QString::number(ll),price);
        }
    }
    else
    {
        new_configuration=true;
    }
}




void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //qDebug()<<"********************";

    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        Mwindowtimer->start(locktimer);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::hideAllNetworkComponents()
{
    sifre_1_label->setVisible(false);
    sifre_1_lineEdit->setVisible(false);
    sifre_1_pushbutton->setVisible(false);

    sifre_2_label->setVisible(false);
    sifre_2_lineEdit->setVisible(false);
    sifre_2_pushbutton->setVisible(false);
    SabitIP_check->setVisible(false);
    DinamikIP_check->setVisible(false);

    usc_ip_label->setVisible(false);
    usc_ip_lineedit->setVisible(false);
    usc_ip_pushbutton->setVisible(false);

    usc_port_label->setVisible(false);
    usc_port_lineedit->setVisible(false);
    usc_port_pushbutton->setVisible(false);


    //keyboard_frame->setVisible(false);
    subnetMask_pushbutton->setVisible(false);
    gateway_label->setVisible(false);

    menu_enter->setVisible(false);
    menu_cancel->setVisible(false);

}

void MainWindow::raiseAllNeworkComponents()
{
    sifre_1_label->setVisible(true);
//    sifre_1_lineEdit->setVisible(true);
//    sifre_1_pushbutton->setVisible(true);

    sifre_2_label->setVisible(true);
//    sifre_2_lineEdit->setVisible(true);
//    sifre_2_pushbutton->setVisible(true);

    usc_ip_label->setVisible(true);
    usc_ip_lineedit->setVisible(true);
    usc_ip_pushbutton->setVisible(true);

    usc_port_label->setVisible(true);
    usc_port_lineedit->setVisible(true);
    usc_port_pushbutton->setVisible(true);

    SabitIP_check->setVisible(true);
    DinamikIP_check->setVisible(true);

    subnetMask_pushbutton->setVisible(true);
    gateway_label->setVisible(true);

    menu_enter->setVisible(true);
    menu_cancel->setVisible(true);


}
//void MainWindow::nexttabPushbutton_clicked(){
//    menu_tabWidget->setCurrentIndex(menutabwidgetindex-1);
//    //qDebug()<<"girdi"<<menutabwidgetindex;

//}
//void MainWindow::previusTabPushbutton_clicked(){
//    menu_tabWidget->setCurrentIndex(menutabwidgetindex+1);
//    //qDebug()<<"girdi"<<menutabwidgetindex;

//}

void MainWindow::setDate_pushbutton_clicked(){

    numpadDateTimeEdit->setVisible(true);
    numpadDateTimeEdit->raise();

    numpadframe->show();
    numpadframe->raise();
    numpadframe->setVisible(true);
    keyboardcomingflag=24;
    LCD_status=40;
    tarih_1_lineEdit->setVisible(false);
    saat_1_lineEdit->setVisible(false);
    setDate_pushbutton->setVisible(false);
    numpadDateTimeEdit->setText(dateFormat_String);
    saat_sor->setVisible(false);
    tarih_sor->setVisible(false);

    //Numpad_Output.clear();
    //Numpad_Output=numpadDateTimeEdit->text();
    //numpadDateTimeEdit->setText(Numpad_Output);

        qDebug()<<"acilid"<<Numpad_Output;


//    if(Mwindowlock==false)
//    {
//        BuzzerON();
//        Numpad_Output.clear();
//        Numpad_Output=saat_1_lineEdit->text();
//        numpadDateTimeEdit->setVisible(true);
//        numpadDateTimeEdit->raise();
//       // hideAllNetworkComponents();
//        numpadDateTimeEdit->setDisplayFormat("hh:mm:ss");
//        numpadframe->show();
//        numpadframe->raise();
//        QDate date = QDate::fromString(Numpad_Output);
//        numpadDateTimeEdit->setDate(date);
//        numpadframe->setVisible(true);

//        //numpadDateTimeEdit->setText(saat_1_lineEdit->text());
//        sifre_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
//        sifre_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
//        saat_1_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
//        tarih_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
//        Mwindowlock=true;
//        Mwindowtimer->start(locktimer);
//    }
}


void MainWindow::receipt_1_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        key->outputText.clear();
        key->outputText=receipt_1_lineEdit->text();
        keyboardcomingflag=5;
        receipt_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_3_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_4_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_1_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::receipt_2_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        key->outputText.clear();
        key->outputText=receipt_2_lineEdit->text();
        keyboardcomingflag=6;
        receipt_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_4_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_3_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_2_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::receipt_3_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        key->outputText.clear();
        key->outputText=receipt_3_lineEdit->text();
        keyboardcomingflag=7;
        receipt_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_4_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_3_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowtimer->start(locktimer);
    }
}

void MainWindow::receipt_4_pushbutton_clicked()
{
    if(Mwindowlock==false)
    {
        Mwindowlock=true;
        BuzzerON();
        key->outputText.clear();
        key->outputText=receipt_4_lineEdit->text();
        keyboardcomingflag=8;
        receipt_1_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_2_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_3_lineEdit->setStyleSheet("color:rgb(255,255,255);background-color:rgb(0,0,0);border:1px solid #FFFFFF; border-radius:10px;");
        receipt_4_lineEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,155,0);border:1px solid #FFFFFF; border-radius:10px;");
        Mwindowtimer->start(locktimer);
    }
}

QString MainWindow::createPasswd()
{
    QString macaddr=getMacAddress();
    macaddr=macaddr.remove(":");
    QString tempdate=QDate::currentDate().toString("dd");
    macaddr.append(tempdate);
    QCryptographicHash calculate256(QCryptographicHash::Sha256);
    calculate256.addData(macaddr.toUtf8());
    macaddr=QString::fromUtf8(calculate256.result().toHex());
    macaddr=macaddr.mid(0,6);
    if(test) qDebug()<<"macaddr"<<macaddr.toUpper().toLatin1().toHex();
    return macaddr.toUpper().toLatin1().toHex();
}
