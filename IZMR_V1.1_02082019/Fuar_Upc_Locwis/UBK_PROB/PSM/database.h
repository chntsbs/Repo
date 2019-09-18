#ifndef DATABASE_H
#define DATABASE_H
#include <QtWidgets>
#include <QSqlDatabase>

class Database
{
public:
    Database();
    void insertToStatus();

    void Open_Person_Database();
    void Close_Person_Database();
    void InsertPersonRfid(int id, QString rfid, QString name, int dgroupname);
    bool kontrolPersonRfid(QString id);
    void Open_Whitelist_Database();
    void Close_Whitelist_Database();
    void Delete_ServerId(QString server_id);
    void InsertWhitelist(int id, QString server_id, QString white_rfid, QString white_plate, QString customer_name, QString white_products, QString card_type, QString list_type,QString offline_limit, QString central_price, QString discount_group, QString promotion_group, QString authorization_type, QString card_password, QString event, QString event_time, QString balance_limit, QString vehicle_km);//30.05.2019 chn--vehicle km
    void OrderWhitelist();
    void updateBalanceLimit(QString id,QString balanceLmt);
    bool kontrol_ServerId(QString server_id);
    void UpdateWhitelist(QString server_id, QString white_rfid, QString white_plate, QString customer_name, QString white_products, QString card_type, QString list_type, QString offline_limit, QString central_price, QString discount_group, QString promotion_group, QString authorization_type, QString card_password, QString event, QString event_time, QString vehicle_km );
    bool kontrolSalesRfid(QString id);
    bool kontrolSales(QString id);
    bool kontrolSalesPlate(QString plate);
    bool ProbeValues();
    bool SettingsValues();
    void delete_first_sales();
    QString getProductname();
    void Open_Nozzles_Database();
    void Close_Nozzle_Database();
    QSqlQuery Show_Person();
    QSqlQuery Show_Whitelist();
    QSqlQuery Show_Nozzle();
    QSqlQuery Show_Sales();
    void InsertNozzle(int nozzle_id, int product_no, int tank_no, QString unit_price, QString max_unit_price, QString min_unit_price);
    void UpdateNozzle(int id, QString product_id, QString product_name, QString product_price, QString total_amount, QString total_volume, QString total_sales);
    QString NozzleProductId(int id);
    bool NozzleValues();
    QString NozzleProductPrice(int id);
    QString NozzleTotalAmount(int id);
    QString NozzleTotalVolume(int id);
    QString NozzleTotalSales(int id);
    void UpdateNozzleProduct(int id, QString product_id, QString product_name);
    void UpdateNozzlePrice(int id, QString product_price);
    void UpdateNozzleTotal(int id, QString total_amount, QString total_volume, QString total_sales);
    void InsertSales(QString id,int TSNO, int RNO, QString Tdate, QString Tclock, int PMPN, quint8 nozzle,quint8 prdctNo,quint8 tankNo, QByteArray unitprice, QByteArray liter, QByteArray amount, QByteArray bamount,QByteArray eamount, QByteArray bvolume, QByteArray evolume,quint8 Ftype, quint8 Ctype, quint8 Atype,quint8 Stype,quint8 Ptype, QString rfidCustomer, QString rfidDriver, QString rfidPerson, QByteArray plate, QString vehiclekm,QString vehicleruntime,QString NZLUT, QString NZLDT,QString RFRT,QString RFRET,QString AUTHT,QString FILLT,QString PCT, QString PFT, quint8 status);
    void delete_table(QString Tname);
    int CountId(QString tablo);
    void delete_zeros(QString Tname);
    int check_salesDate(QString tablo);
    QJsonObject kontrolSalesStatus();
    void UpdateStatus2(QString id);
    void UpdateStatus(QString id);
    void Updatedateclock(QString id,QString date, QString clock);
    bool UpdateSales(int id, QString date, QString clock, QString liter, QString price,QString amount);
    void UpdatePlate(int id, QString plate, QString vehiclekm, QString type);
    void UpdateProbe(QString adres, QString status, QString fuel_level, QString water_level, QString temperature);
    int totalsales(QString plate);
    void updateProductname(int nozzle_id, int product_id, QString price, int tank);
    void updateSettings(int card_adr, int unit_price_index, int amount_index, int volume_index, QString receipt_1, QString receipt_2, QString receipt_3, QString receipt_4, bool km, bool sell, bool plates);
    QString get_Unit_Price(int id);
    QString getBalanceLimit(QString rfid);
    int getKm(QString plate);//CHN
    void updateAllBalanceLimit();
    bool updateKm(QString kilometer);
    //void updateKmpomp(QString kilometer);


private:
 //   QSqlDatabase m_db;
};
#endif // DATABASE_H
