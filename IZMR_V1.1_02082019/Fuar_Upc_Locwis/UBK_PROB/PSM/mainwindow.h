#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include "keyboard.h"
#include "QHBoxLayout"
#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QTextEdit>
#include <customerscreen.h>
#include"database.h"
#include "QtSql"
#include"QTableView"
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void mysleep(unsigned int delay);
    void openChromium(QString goTo);

    int menutabwidgetindex=0;
    int keyboardcomingflag;


    CustomerScreen *customer;

    QMovie *locwiz_logo_movie;

    QSize *size1;



    //FRAMES
    QFrame *keyboard_frame;
    QFrame *top_frame;
    QFrame *info_top_frame;
    QFrame *main_frame;
    QFrame *info_frame;
    QFrame *password_frame;
    QFrame *pmenter_frame;
    QFrame *plateenter_frame;
    QFrame *plate_frame;
    QFrame *km_frame;
    QFrame *kmenter_frame;
    QFrame *numpadframe;
    QFrame *Records_Frame;
    QFrame *Person_Frame;
    QFrame *Whitelist_Frame;
    QFrame *Nozzle_Frame;
    QFrame *Sales_Frame;
    QFrame *devicesettingtabframe;
    Database *MWdatabase;
    Database *database;

    QFrame *menu_frame;
    //QFrame *message_frame;
    //LINE EDITS
    QLineEdit *saat_lineEdit;
    QLineEdit *tarih_lineEdit;
    QLineEdit *plateLine;
    QLineEdit *kmLine;
    QLineEdit *passwrdLine;
    QLineEdit *receipt_1_lineEdit;
    QLineEdit *receipt_2_lineEdit;
    QLineEdit *receipt_3_lineEdit;
    QLineEdit *receipt_4_lineEdit;
    QLineEdit *sifre_1_lineEdit;
    QLineEdit *sifre_2_lineEdit;
    QLineEdit *usc_ip_lineedit;
    QLineEdit *usc_port_lineedit;
    QLineEdit *subnetMax_lineedit;
    QLineEdit *otomasyon_adres_lineEdit;
    QLineEdit *nozzle_adres_lineEdit;
    QLineEdit *channel_lineEdit;
    QLineEdit *pause_time_lineEdit;
    QLineEdit *numpadDateTimeEdit;
    QLineEdit *numpadLineEdit;
    QDateEdit *tarih_1_lineEdit;
    QTimeEdit *saat_1_lineEdit;



    //QLineEdit *warning_lineEdit;
    //WIDGETS
    //LAYOUTS
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QGridLayout *gridLayoutnumpad;
    //TOOL BUTTONS

    //QToolButton *message_icon_button;
    QToolButton *warning_icon_button;
    QToolButton *kmWarningButton;
    //PUSH BUTTONS
    QPushButton *mepsan_logo;
    QPushButton *net_logo;
    QPushButton *records_button;
    QPushButton *enter_plate_button;
    QPushButton *menu_button;
    QPushButton *device_icon_button;
    QToolButton *whitelist_button;
    QToolButton *worker_button;
    QToolButton *sales_button;
    QToolButton *saved_records_button;
    QToolButton *back_from_records_button;
    QToolButton *back_from_Table_button;
    QToolButton *accesspoint_test_status;
    QToolButton *printer_test_status;
    QToolButton *cardreader_test_status;


    QPushButton *numpad_0;
    QPushButton *numpad_1;
    QPushButton *numpad_2;
    QPushButton *numpad_3;
    QPushButton *numpad_4;
    QPushButton *numpad_5;
    QPushButton *numpad_6;
    QPushButton *numpad_7;
    QPushButton *numpad_8;
    QPushButton *numpad_9;
    QPushButton *numpad_clear;
    QPushButton *numpad_point;
    QPushButton *numpad_cancel;
    QPushButton *numpad_enter;
    QPushButton *menu_cancel;
    QPushButton *menu_enter;
    QPushButton *receipt_2_pushbutton;
    QPushButton *receipt_1_pushbutton;
    QPushButton *receipt_3_pushbutton;
    QPushButton *receipt_4_pushbutton;
    QPushButton *sifre_1_pushbutton;
    QPushButton *sifre_2_pushbutton;
    QPushButton *usc_ip_pushbutton;
    QPushButton *usc_port_pushbutton;
    QPushButton *subnetMask_pushbutton;
    QPushButton *saha_modu_pushbutton;
    QPushButton *licance_pushbutton;
//    QPushButton *nextTabPushbutton;
 //   QPushButton *previusTabPushbutton;
    QPushButton *sd_kart_yedek_pushbutton;
    QPushButton *sd_kart_al_pushbutton;
    QPushButton *testmodestart;
    QPushButton *update_pushbutton;
    QPushButton *access_point_read_pushbutton;
    QPushButton *access_point_write_pushbutton;
    QPushButton *otomasyon_adres_pushbutton;
    QPushButton *nozzle_adres_pushbutton;
    QPushButton *channel_pushbutton;
    QPushButton *pause_time_pushbutton;
    QPushButton *tts_active_pushbutton;
    QPushButton *top_button;
    QPushButton *setDate_pushbutton;
    QPushButton *yesButton;


    //LABELS
    QLabel *status_label;
    QLabel *warning_label;
    QLabel *plate_label;
    QLabel *km_label;
    QLabel *paswrd_label;  
    QLabel *adress_label;
    QLabel *Volumedotindex_label;
    QLabel *Amountdotindex_label;
    QLabel *Upricedotindex_label;
    QLabel *kmsor_label;
    QLabel *freeSaleLabel;
    QLabel *plateSaleLabel;
    QLabel *CardR_Address_label;
    QLabel *Nozzle_Num_label;
    QLabel *receipt_1_label;
    QLabel *receipt_2_label;
    QLabel *receipt_3_label;
    QLabel *receipt_4_label;
    QLabel *sifre_1_label;
    QLabel *sifre_2_label;
    QLabel *usc_ip_label;
    QLabel *usc_port_label;
    QLabel *gateway_label;
    QLabel *product_comboBox_label[7];
    QLabel *access_test_label;
    QLabel *printer_test_label;
    QLabel *cardreader_test_label;
    QLabel *otomasyon_adres_label;
    QLabel *access_point_info_label;
    QLabel *nozzle_adres_label;
    QLabel *channel_label;
    QLabel *pause_time_label;
    QLabel *otomasyon_adres_new_label;
    QLabel *nozzle_adres_new_label;
    QLabel *channel_new_label;
    QLabel *pause_time_new_label;
    QLabel *locwiz_logo;
    QLabel *tarih_sor;
    QLabel *saat_sor;


    QTabWidget *menu_tabWidget;

    QWidget *devicesettingtab;
    QWidget *datesettingtab;
    QWidget *saleSettingTab;
    QWidget *receipttab;
    QWidget *producttab;
    QWidget *kmtab;
    QWidget *sifretab;
    QWidget *testmodetab;
    QWidget *generalset;


    QSpinBox *adress_spinBox;
    QSpinBox *Nozzle_Num_spinBox;
    QSpinBox *CardR_Address_spinBox;
    QSpinBox *Volumedotindex_spinBox;
    QSpinBox *Amountdotindex_spinBox;
    QSpinBox *Upricedotindex_spinBox;
    QComboBox *product_comboBox[7];

    QCheckBox *kmsor_check;
    QCheckBox *freeSaleCheck;
    QCheckBox *plateSaleCheck;
    QCheckBox *DinamikIP_check;
    QCheckBox *SabitIP_check;
    QMessageBox *reply;
    QMessageBox *reply2;

    Keyboard *key;
    QSqlQueryModel *Personmodel;
    QTableView *Persontable;
    QSqlQueryModel *Whitelistmodel;
    QTableView *Whitelisttable;
    QSqlQueryModel *Nozzlemodel;
    QTableView *Nozzletable;
    QSqlQueryModel *Salesmodel;
    QTableView *Salestable;
    QRect screen;
    QTimer *Mwindowtimer;
    QProcess *pingProcess;
    QTimer *CLRCtimer2;
    int screen_width;
    int screen_heigth;
    bool Mwindowlock;

    void BuzzerON();

    void BuzzerOFF();
    ~MainWindow();
private slots:

    QString createPasswd();
    void Numpad_Enter_Clicked();
    void Numpad_Sender_Clicked();
    void Numpad_Cancel_Clicked();
    void Numpad_Clear_Clicked();
    void Enter_Plate_Clicked();
    void Enter_Record_Clicked();
    void records_button_clicked();
    void whitelist_button_clicked();
    void sales_button_clicked();    
    void worker_button_clicked();
    void back_from_records_button_Clicked();
    void menu_button_Clicked();
    void menu_enter_button_Clicked();
    void menu_cancel_button_Clicked();
    void Nozzle_Num_Changed(int line);
    void net_logo_clicked();
    void locwiz_Clicked();
    void DinamikIP_check_clicked();
    void SabitIP_check_clicked();
    void usc_ip_pushbutton_clicked();
    void usc_port_pushbutton_clicked();
    void subnetMask_pushbutton_clicked();
    void licance_pushbutton_Clicked();
    void saha_modu_pushbutton_Clicked();
    void testmodu_clicked();
    void Mwindowaction();
    void update_from_flash();
    void clrcBuzzerON();
//    void nexttabPushbutton_clicked();
 //   void previusTabPushbutton_clicked();

//    void on_records_button_clicked();

//    void on_back_from_password_button_3_clicked();

//    void on_password_enter_button_clicked();

//    void on_back_from_records_button_clicked();

//    void on_whitelist_button_clicked();

//    void on_back_from_tableview_button_clicked();

//    void on_worker_button_clicked();

//    void on_sales_button_clicked();

//    void on_saved_records_button_clicked();

//    void on_logs_button_clicked();

//    void on_back_from_logs_button_clicked();

    void keyboardback();

    void keyboardvalue(QString text);

    void keyboardEntervalue();

    void keyboardClearvalue();
//    void on_menu_button_clicked();

//    void getTabNo();

//    void on_back_from_records_button_2_clicked();

//    void on_testmodestart_clicked();

//    void messageShow();

      void show_time();

      void screen_lines(QString,QString);

      void frameChange(int);

      void getTabNo();

      void sd_kart_yedek_pushbutton_Clicked();

      void sd_kart_al_pushbutton_Clicked();

      void setDate_pushbutton_clicked();

      void receipt_1_pushbutton_clicked();

      void receipt_2_pushbutton_clicked();

      void receipt_3_pushbutton_clicked();

      void receipt_4_pushbutton_clicked();

      void sifre_1_pushbutton_clicked();

      void sifre_2_pushbutton_clicked();

      void access_point_read_clicked();

      void access_point_write_clicked();

      void otomasyon_adres_pushbutton_clicked();

      void nozzle_adres_pushbutton_clicked();

      void channel_pushbutton_clicked();

      void pause_time_pushbutton_clicked();

      void tts_active_pushbutton_clicked();

      void showModified(QString data);

protected:
      void mousePressEvent(QMouseEvent *event);

private:
      Ui::MainWindow *ui;
      QSqlDatabase tankLevel;
      void hideAllNetworkComponents();
      void raiseAllNeworkComponents();

};

#endif // MAINWINDOW_H
