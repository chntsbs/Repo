#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QtGui>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>


namespace Ui {
class Keyboard;
}

class Keyboard : public QWidget
{
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = 0);
    void setLineEdit(QLineEdit * );
    ~Keyboard();
    QPushButton *enterButton;
    QString outputText;
    QString appendoutputText;
    QLineEdit *keylinedit;
    QTimer *keyboardtimer;
    bool keyboardlock;
    void BuzzerON();
    void BuzzerOFF();
public slots:
    void keyboardaction();
signals:
    void sendKBvalue(QString);
    void KBback();
    void KBsendok();
    void KBsendclear();

private slots:
    void keyboardHandler();
    void keyboardClickHandler();
    void Back_clicked();
    void Back_clicked1();

    void char_2_toggled(bool checked);
    void enterButton_clicked();
    void enterButton_clicked1();
    void lineEdit_textChanged(const QString &arg1);
    void ClearButton_clicked();
    void ClearButton_clicked1();


private:
    Ui::Keyboard *ui;

    QLineEdit *outputLineEdit;
    bool shift;
};
extern Keyboard *key;
#endif // KEYBOARD_H
