/*
 * Author: LT Thomas <ltjr@ti.com>
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


#include "keyboard.h"
#include "ui_keyboard.h"
#include <QtGui>
#include <QDebug>
#include <QLineEdit>
#include "global.h"
#include "mainwindow.h"
QString ClickinputText;
bool clearclicked_flg;
bool enterclickedflg;
bool backclickedflg;
Keyboard::Keyboard(QWidget *parent) :
    QWidget(parent,Qt::FramelessWindowHint),
    ui(new Ui::Keyboard)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    keylocktimer=171;
    ui->Buttoni->setText(QString::fromUtf8("\u0130"));
    keyboardtimer=new QTimer(this);
    connect(keyboardtimer,SIGNAL(timeout()),this,SLOT(keyboardaction()));
    connect ( ui->Buttonu_2, SIGNAL( clicked()), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttoni_2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttono_2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttons_2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttong_2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonc_2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    connect ( ui->Buttonq, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonw, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttone, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonr, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttont, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttony, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonu, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttoni, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttono, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonp, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    connect ( ui->Buttona, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttons, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttond, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonf, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttong, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonh, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonj, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonk, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonl, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    connect ( ui->Buttonz, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonx, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonc, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonv, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonb, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonn, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonm, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );



    //connect ( ui->Button00, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button0, SIGNAL(clicked()), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button1, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button3, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button4, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button5, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button6, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button7, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button8, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button9, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->space, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    //--------------------------------------------------------------------------------------------------------
    connect ( ui->Buttonu_2, SIGNAL(pressed()), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttoni_2, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttono_2, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttons_2, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttong_2, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonc_2, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );

    connect ( ui->Buttonq, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonw, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttone, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonr, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttont, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttony, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonu, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttoni, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttono, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonp, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );

    connect ( ui->Buttona, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttons, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttond, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonf, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttong, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonh, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonj, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonk, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonl, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );

    connect ( ui->Buttonz, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonx, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonc, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonv, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonb, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonn, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Buttonm, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );



    //connect ( ui->Button00, SIGNAL( clicked() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button0, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button1, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button2, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button3, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button4, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button5, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button6, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button7, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button8, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->Button9, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    connect ( ui->space, SIGNAL( pressed() ), this, SLOT( keyboardClickHandler() ) );
    //--------------------------------------------------------------------------------------------------------
    connect ( ui->ClearButton, SIGNAL( clicked() ), this, SLOT( ClearButton_clicked() ) ); //sil
    connect ( ui->BackButton, SIGNAL( clicked() ), this, SLOT( Back_clicked() ) );
    connect ( ui->ButtonEnter, SIGNAL( clicked() ), this, SLOT( enterButton_clicked() ) );
    //--------------------------------------------------------------------------------------------------------
    connect ( ui->ClearButton, SIGNAL( pressed()), this, SLOT( ClearButton_clicked1() ) ); //sil
    connect ( ui->BackButton, SIGNAL ( pressed()), this, SLOT( Back_clicked1() ) );
    connect ( ui->ButtonEnter, SIGNAL( pressed()), this, SLOT( enterButton_clicked1() ) );

    //ui->Buttonc_2->setText(trUtf8("\u00C7"));

    outputText = "";
    shift = false;
    // ui->lineEdit->setFocus();

}

void Keyboard::keyboardaction()
{
    keyboardtimer->stop();
    BuzzerOFF();
    keyboardlock=false;
}

void Keyboard::keyboardClickHandler()
{

    if(keyboardlock==false)
    {
        QPushButton *button = (QPushButton *)sender();
        ClickinputText= button->text();
        if(test)qDebug()<<"clicked"<<ClickinputText;
    }

}

void Keyboard::keyboardHandler()
{
    if(keyboardlock==false)
    {
        QPushButton *button = (QPushButton *)sender();
        //QChar s=button->text().data();

        QString inputText = button->text();
        if(test)qDebug()<<"keyboardHandler"<<inputText<<ClickinputText;
        if(ClickinputText==inputText)
        {
            ClickinputText.clear();
            BuzzerON();
            if(inputText=="İ")
            {
                if(test) qDebug()<<QString::fromUtf8("\u0130");
                outputText+="I";

            }
            else if(inputText=="Ç")
            {
                outputText+="C";
            }
            else if(inputText=="Ş")
            {
                outputText+="S";
            }
            else if(inputText=="Ö")
            {
                outputText+="O";
            }
            else if(inputText=="Ü")
            {
                outputText+="U";
            }
            else if(inputText=="Ğ")
            {
                outputText+="G";
            }
            else if (inputText == "Space")
            {
                outputText += " ";
            }
            else if(inputText == "&&")
            {
                outputText += "&";
            }
            else if(inputText == "\\")
            {
                outputText += ui->Buttona->text() ;
            }
            else
            {
                if(shift)
                {
                    shift = false;
                    outputText += inputText.toUpper();
                }
                else
                {
                    //ui->Buttoni->setText(trUtf8("İ"));
                    outputText += inputText;
                }
            }


            //ui->lineEdit->setText(outputText);
            //qDebug()<<ui->lineEdit->text();
            keyboardlock=true;
            keyboardtimer->start(keylocktimer);
            if(test) qDebug() << "TEXT:" <<outputText;
            emit sendKBvalue(outputText);
        }
        else
        {
            inputText=ClickinputText;
            ClickinputText.clear();
            BuzzerON();
            if(inputText=="İ")
            {
                if(test) qDebug()<<QString::fromUtf8("\u0130");
                outputText+="I";

            }
            else if(inputText=="Ç")
            {
                outputText+="C";
            }
            else if(inputText=="Ş")
            {
                outputText+="S";
            }
            else if(inputText=="Ö")
            {
                outputText+="O";
            }
            else if(inputText=="Ü")
            {
                outputText+="U";
            }
            else if(inputText=="Ğ")
            {
                outputText+="G";
            }
            else if (inputText == "Space")
            {
                outputText += " ";
            }
            else if(inputText == "&&")
            {
                outputText += "&";
            }
            else if(inputText == "\\")
            {
                outputText += ui->Buttona->text() ;
            }
            else
            {
                if(shift)
                {
                    shift = false;
                    outputText += inputText.toUpper();
                }
                else
                {
                    //ui->Buttoni->setText(trUtf8("İ"));
                    outputText += inputText;
                }
            }


            //ui->lineEdit->setText(outputText);
            //qDebug()<<ui->lineEdit->text();
            keyboardlock=true;
            keyboardtimer->start(keylocktimer);
            if(test) qDebug() << "TEXT:" <<outputText;
            emit sendKBvalue(outputText);
        }

    }
}

void Keyboard::setLineEdit(QLineEdit * line)
{
    outputLineEdit = line;
}

Keyboard::~Keyboard()
{
    delete ui;
}

void Keyboard::BuzzerON()
{
    system("echo 1 > /sys/class/gpio/gpio141/value");
}

void Keyboard::BuzzerOFF()
{
    system("echo 0 > /sys/class/gpio/gpio141/value");
}
void Keyboard::Back_clicked1()
{
    if(keyboardlock==false)
    {
        backclickedflg=true;
    }
}
void Keyboard::Back_clicked()
{
    if(keyboardlock==false)
    {
        if(backclickedflg==true)
        {
            backclickedflg=false;
            BuzzerON();
            outputText.clear();
            keyboardlock=true;
            keyboardtimer->start(keylocktimer);
            emit KBback();
        }
    }
}


void Keyboard::char_2_toggled(bool checked)
{
    if(checked)
    {
        ui->Button0->setText("`");
        ui->Button1->setText("~");
        ui->Button2->setText("!");
        ui->Button3->setText("@");
        ui->Button4->setText("#");
        ui->Button5->setText("$");
        ui->Button6->setText("%");
        ui->Button7->setText("^");
        ui->Button8->setText("&&");// trUtf8("\u0040")
        ui->Button9->setText("*");

        ui->Buttonq->setText("(");
        ui->Buttonw->setText(")");
        ui->Buttone->setText("-");
        ui->Buttonr->setText("_");
        ui->Buttont->setText("=");
        ui->Buttony->setText("+");
        ui->Buttonu->setText("[");
        ui->Buttoni->setText("]");
        ui->Buttono->setText("{");
        ui->Buttonp->setText("}");

        ui->Buttona->setText(trUtf8("\\"));
        ui->Buttons->setText("|");
        ui->Buttond->setText(";");
        ui->Buttonf->setText(":");
        ui->Buttong->setText("'");
        ui->Buttonh->setText("\"");
        ui->Buttonj->setText("/");
        ui->Buttonk->setText("?");
        ui->Buttonl->setText("");

        ui->Buttonz->setText("<");
        ui->Buttonx->setText(">");
        ui->Buttonc->setText(",");
        ui->Buttonv->setText(".");
        ui->Buttonb->setText("");
        ui->Buttonn->setText("");
        ui->Buttonm->setText("");

    }
    else
    {
        ui->Button0->setText("0");
        ui->Button1->setText("1");
        ui->Button2->setText("2");
        ui->Button3->setText("3");
        ui->Button4->setText("4");
        ui->Button5->setText("5");
        ui->Button6->setText("6");
        ui->Button7->setText("7");
        ui->Button8->setText("8");
        ui->Button9->setText("9");

        ui->Buttonq->setText("q");
        ui->Buttonw->setText("w");
        ui->Buttone->setText("e");
        ui->Buttonr->setText("r");
        ui->Buttont->setText("t");
        ui->Buttony->setText("y");
        ui->Buttonu->setText("u");
        //ui->Buttoni->setText(trUtf8("İ"));
        ui->Buttono->setText("o");
        ui->Buttonp->setText("p");

        ui->Buttona->setText("a");
        ui->Buttons->setText("s");
        ui->Buttond->setText("d");
        ui->Buttonf->setText("f");
        ui->Buttong->setText("g");
        ui->Buttonh->setText("h");
        ui->Buttonj->setText("j");
        ui->Buttonk->setText("k");
        ui->Buttonl->setText("l");

        ui->Buttonz->setText("z");
        ui->Buttonx->setText("x");
        ui->Buttonc->setText("c");
        ui->Buttonv->setText("v");
        ui->Buttonb->setText("b");
        ui->Buttonn->setText("n");
        ui->Buttonm->setText("m");
    }
}

void Keyboard::enterButton_clicked1()
{
    if(keyboardlock==false)
    {
        enterclickedflg=true;
    }
}

void Keyboard::enterButton_clicked()
{
    if(keyboardlock==false)
    {
        if(enterclickedflg==true)
        {
            enterclickedflg=false;
            BuzzerON();
            keyboardlock=true;
            keyboardtimer->start(keylocktimer);
            emit KBsendok();
        }
    }
}

void Keyboard::lineEdit_textChanged(const QString &arg1)
{
    outputText = arg1;
    //ui->lineEdit->setText(outputText);

}

void Keyboard::ClearButton_clicked1()
{
    if(keyboardlock==false)
    {
        clearclicked_flg=true;
    }
}

void Keyboard::ClearButton_clicked()
{
    if(keyboardlock==false)
    {
        if(clearclicked_flg==true)
        {
            clearclicked_flg=false;
            BuzzerON();
            outputText.remove(outputText.length()-1,outputText.length());
            if(test) qDebug()<<"TEXT:"<<outputText;
            //ui->lineEdit->setText(outputText);
            keyboardclear=1;
            keyboardlock=true;
            keyboardtimer->start(keylocktimer);
            emit KBsendclear();
            emit sendKBvalue(outputText);
        }
    }
}
