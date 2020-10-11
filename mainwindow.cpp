#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QPushButton>
#include<QMessageBox>
#include<map>
#include<stack>
#include<cmath>
#include<QClipboard>
#include<QApplication>
#include<QDebug>
#include<QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText("");
    waitForOperand=true;
    connectSlots();
    setShortcutKeys();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::abortOperation()
{

    ui->lineEdit->setText("0");
    waitForOperand=true;
    QMessageBox::warning(this,"运算错误","错误！");
}

void MainWindow::connectSlots()
{
    QPushButton *digitaBtns[16]={
        ui->digtalBtn0,ui->digtalBtn1,ui->digtalBtn2,ui->digtalBtn3,ui->digtalBtn4,ui->digtalBtn5,ui->digtalBtn6,ui->digtalBtn7,ui->digtalBtn8,ui->digtalBtn9,
        ui->BtnA,ui->BtnB,ui->BtnC,ui->BtnD,ui->BtnE,ui->BtnF,
    };

    for(auto btn:digitaBtns)
        connect(btn,&QPushButton::clicked,this,&MainWindow::digitClicked);

    QPushButton *operatorBtns[9]={
        ui->addBtn,ui->subtractionBtn,ui->mulBtn,ui->divBtn,ui->leftBracketBtn,ui->rightBracketBtn,ui->powBtn,ui->GCDBtn,ui->LCMBtn

        };
    for(auto btn:operatorBtns)
        connect(btn,&QPushButton::clicked,this,&MainWindow::operatorClicked);
    QPushButton *specialOperatorBtns[5]={
        ui->sinBtn,ui->cosBtn,ui->tanBtn,ui->lnBtn,ui->lgBtn
    };

    for(auto btn:specialOperatorBtns)
        connect(btn,&QPushButton::clicked,this,&MainWindow::specialOperatorClicked);
}
 void MainWindow::setShortcutKeys()
 {
     QPushButton *btn[34]={
         ui->digtalBtn0,ui->digtalBtn1,ui->digtalBtn2,ui->digtalBtn3,ui->digtalBtn4,ui->digtalBtn5,ui->digtalBtn6,ui->digtalBtn7,ui->digtalBtn8,ui->digtalBtn9,
         ui->BtnA,ui->BtnB,ui->BtnC,ui->BtnD,ui->BtnE,ui->BtnF,
         ui->addBtn,ui->subtractionBtn,ui->mulBtn,ui->divBtn,ui->equalBtn,ui->pointBtn,ui->clearAllBtn,ui->signBtn,ui->leftBracketBtn,ui->rightBracketBtn,ui->powBtn,ui->GCDBtn,ui->LCMBtn,
         ui->sinBtn,ui->cosBtn,ui->tanBtn,ui->lnBtn,ui->lgBtn
     };
 }

 void MainWindow::digitClicked()
 {
     QPushButton *digitBtn=static_cast<QPushButton*>(sender());

     QString value=digitBtn->text();

     if(ui->lineEdit->text()=="0"&&value=="0")
         return;

     if(waitForOperand)
     {
         ui->lineEdit->setText(value);
         waitForOperand=false;
     }
     else
     {
         ui->lineEdit->setText(ui->lineEdit->text()+value);
     }
 }

 void MainWindow::operatorClicked()
 {
     QPushButton *clickedBtn=qobject_cast<QPushButton*>(sender());
     QString clickedOperator=clickedBtn->text();
     if(waitForOperand==true&&clickedOperator=='(')//首符号为‘（’的到时候
         {
             ui->lineEdit->setText(clickedOperator);
             waitForOperand=false;
         }
     else if(waitForOperand==false)
         ui->lineEdit->setText(ui->lineEdit->text()+clickedOperator);
 }

 void MainWindow::specialOperatorClicked()
 {
     QPushButton *specialOperatorBtn=static_cast<QPushButton*>(sender());
     QString value=specialOperatorBtn->text();
     if(waitForOperand)
     {
         ui->lineEdit->setText(value);
         waitForOperand=false;
     }
     else
     {
         ui->lineEdit->setText(ui->lineEdit->text()+value);
     }
 }


void MainWindow::on_equalBtn_clicked()
{
    double result=0.0;
        try
        {
            result=compute(inToPost(ui->lineEdit->text()));
        }
        catch(const char *er)
        {
            error=er;
            abortOperation();
            return;
        }

        ui->lineEdit->setText(ui->lineEdit->text()+'='+QString::number(result));
        log=ui->lineEdit->text();
        waitForOperand=true;
}

void MainWindow::on_clearBtn_clicked()
{
    //将显示字符串退格
        QString str=ui->lineEdit->text();
        if(str!='0'&&waitForOperand==false)
        {
            ui->lineEdit->setText(str.left(str.count()-1));
            waitForOperand=false;
        }
        else
        {
            ui->lineEdit->setText("0");
            waitForOperand=true;
        }
}

void MainWindow::on_signBtn_clicked()
{
    QString text=ui->lineEdit->text();
       QChar sign=text[text.size()-1];
       if(sign=='-')
       {
           text.remove(text.size()-1,1);
       }
       else {
           text.append('-');
       }

       ui->lineEdit->setText(text);
}


void MainWindow::on_clearAllBtn_clicked()
{
    //将当前显示全部归零
        ui->lineEdit->setText("");
        waitForOperand=true;
}

void MainWindow::on_pointBtn_clicked()
{
    if(waitForOperand==false&&ui->lineEdit->text().data()[ui->lineEdit->text().size()-1].isDigit()==true)
             ui->lineEdit->setText(ui->lineEdit->text()+".");
}

QString MainWindow::inToPost(QString infix) throw(const char*)//将中缀表达式转换为后缀表达式
{
    std::stack<char> stack;
    char current=0;
    QString postfix;
    std::map<char,int> priority;
    priority['+']=0;
    priority['-']=0;
    priority['*']=1;
    priority['/']=1;
    priority['^']=2;
    priority['|']=2;
    priority['~']=2;
    priority['s']=3;
    priority['c']=3;
    priority['t']=3;
    priority['n']=3;
    priority['g']=3;

    for(int i=0;i<infix.length();i++)
    {
        current=infix[i].toLatin1();
        if(isdigit(current))
        {
            postfix.push_back(current);
            continue;
        }
        switch (current)
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '|':
        case '~':

            if(infix[i-1]!=')')
            {
                if(infix[i-1].isDigit())
                    postfix.push_back('#');
                else if(infix[i-1]=='%')
                {}
//                    infix.begin();
                else
                    throw "expression is illegality";
            }
            if(stack.empty()==false)
            {
                char tempTop=stack.top();
                while(tempTop!='('&&priority[current]<=priority[tempTop])
                {
                    stack.pop();
                    postfix.push_back(tempTop);
                    if(stack.empty())
                        break;
                    tempTop=stack.top();
                }
            }
            stack.push(current);

            break;

        case's':
        case'c':
        case't':
            if(i>0&&infix[i-1].isDigit())
                throw"表达式非法";
            if(!stack.empty())
            {
                char temTop=stack.top();
                while(temTop!='('&&priority[current]<priority[temTop])
                {
                    stack.pop();
                    postfix.push_back(temTop);
                    if(stack.empty())
                        break;
                    temTop=stack.top();
                }
            }
            stack.push(current);
            i=i+2;
            break;

        case'l':
            if(i>0&&infix[i-1].isDigit())
                throw"表达式非法";
            if(!stack.empty())
            {
                char temTop=stack.top();
                while(temTop!='('&&priority[infix[i+1].toLatin1()]<priority[temTop])
                {
                    stack.pop();
                    postfix.push_back(temTop);
                    if(stack.empty())
                        break;
                    temTop=stack.top();
                }
            }
            stack.push(infix[i+1].toLatin1());
            i=i+1;
            break;

        case'.':
            postfix.push_back(current);
            break;
        case'(':
            stack.push(current);
            break;
        case')':
             if(infix[i-1]!='%')
                 postfix.push_back('#');//右括号说明前方数字输入完成 标识一下
            char temTop;
            temTop=stack.top();
            while (temTop!='(') {
                stack.pop();
                postfix.push_back(temTop);
                temTop=stack.top();
            }
            stack.pop();
            break;

        default:
            throw "expression has illegality charactor";
            break;
        }
    }
    if(infix[infix.size()-1]!=')')
    {
        if(infix[infix.size()-1].isDigit())
            postfix.push_back('#');
        else if(infix[infix.size()-1]=='%')
        {}
        else
            throw "expression is illegality";
    }
    while(!stack.empty())
    {
        char tempOut=stack.top();
        stack.pop();
        postfix.push_back(tempOut);
    }
    return postfix;
}

double MainWindow::compute(QString s) throw(const char*)//计算后缀表达式
{
    std::stack<double> stack;
    QString str;
    double curr;
    double temNum1;
    double temNum2;
    int a;
    int b;
    for(auto i=s.begin();i!=s.end();i++)
    {
        if((*i).isDigit())
        {
            str.push_back(*i);
            continue;
        }
        switch ((*(i)).toLatin1())
        {
        case '+':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2+temNum1);
            break;
        case '-':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2-temNum1);
            break;
        case '*':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2*temNum1);
            break;
        case '/':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2/temNum1);
            break;
        case '^':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(pow(temNum2,temNum1));
            break;
        case '|':
            a=stack.top();
            stack.pop();
            b=stack.top();
            stack.pop();
            int c,d,t,max,min;
            if(b>a)             //如果b>a，交换双方的值，使大者a为被除数
            { t=a;a=b;b=t;}
            d=a*b;              //待会计算最小公倍数
            while((c=a%b)!=0)   //余数不为零，继续执行循环
            {
                a=b;              //使除数b变为被除数a
                b=c;              //使余数c变为除数b
            }
            max=b;            //最大公约数
            stack.push(max);
            break;
        case'~':
            a=stack.top();
            stack.pop();
            b=stack.top();
            stack.pop();
            int c1,d1,t1,max1,min1;
            if(b>a)             //如果b>a，交换双方的值，使大者a为被除数
            { t1=a;a=b;b=t1;}
            d1=a*b;              //待会计算最小公倍数
            while((c1=a%b)!=0)   //余数不为零，继续执行循环
            {
                a=b;              //使除数b变为被除数a
                b=c1;              //使余数c变为除数b
            }
            max1=b;            //最大公约数
            min1=d1/max1;        //计算最小公倍数
            stack.push(min1);
            break;
        case '.':
            str.push_back(*i);
            break;
        case'#':
            curr=str.toDouble();
            str.clear();
            stack.push(curr);
            break;
        case's':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::sin(temNum1/180.0*M_PI));
            break;
        case'c':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::cos(temNum1/180.0*M_PI));
            break;
        case't':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::tan(temNum1/180.0*M_PI));
            break;
        case'g':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::log10(temNum1));
            break;
        case'n':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::log(temNum1));
            break;

        default:
            throw "illeaglity";
            break;
        }
    }
    curr=stack.top();
    return curr;
}

QString MainWindow::fanbilihanshu(QString x){
    double y=x.toDouble();
    double m=1/y;
    return QString::number(m);
}

void MainWindow::on_fanbiliBtn_clicked()
{
    QString into = ui->lineEdit->text();
    QString result =fanbilihanshu(into);
    ui->lineEdit->setText(result);
}

QString MainWindow::decTobin(QString strDec){
    int nDec = strDec.toInt();
        int nYushu, nShang;
        QString strBin, strTemp;
        //TCHAR buf[2];
        bool bContinue = true;
        while ( bContinue )
        {
            nYushu = nDec % 2;
            nShang = nDec / 2;
            strBin=QString::number(nYushu)+strBin; //qDebug()<<strBin;
            strTemp = strBin;
            nDec = nShang;
            if ( nShang == 0 )
                bContinue = false;
        }
        int nTemp = strBin.length() % 4;
        switch(nTemp)
        {
        case 1:
            strTemp = "000"+strBin;
            strBin = strTemp;
            break;
        case 2:
            strTemp = "00"+strBin;
            strBin = strTemp;
            break;
        case 3:
            strTemp = "0"+strBin;
            strBin = strTemp;
            break;
        default:
            break;
        }
        return strBin;

}

QString MainWindow::decToHex(QString strDec){  //十进制转十六进制
    int hex = strDec.toInt();
    QString hex1 = QString("%1").arg(hex,8,16,QLatin1Char('0'));
    return hex1;
}

int MainWindow::hex2(unsigned char ch){          //十六进制转换工具
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    return 0;
}

QString MainWindow::binToDec(QString strBin){  //二进制转十进制
    QString decimal;
    int nDec = 0,nLen;
    int i,j,k;
    nLen = strBin.length();
    for(i=0;i<nLen;i++){
        if(strBin[nLen-i-1]=="0")
            continue;
        else{
            k=1;
            for(j=0;j<i;j++)
                k=k*2;
            nDec+=k;
        }
    }
    decimal = QString::number(nDec);
    return decimal;
}

QString MainWindow::hexToDec(QString strHex){  //十六进制转十进制
    int i;
    int v = 0;
    for(i=0;i<strHex.length();i++)
    {
        v*=16;
        v+=hex2(strHex[i].toLatin1());
    }
    return QString::number(v);
}

QString MainWindow::octToDec(QString m)//八进制转十进制
{
    int index=0;
    int result=0;
    int length=m.length();
    QString temp;
    int num=0;
    while(length!=0){
        temp=m.right(1);
        num=temp.toInt();
        result=result+num*pow(8,index);
        index+=1;
        length-=1;
        m=m.left(length);
    }
    return QString::number(result);
}

QString MainWindow::DecToOct(QString m)//十进制转八进制
{
    int nDec = m.toInt();
    int index=1;
    int result=0;
    while(nDec!=0){
        result=result+(nDec%8)*index;
        nDec=nDec/8;
        index*=10;
    }
    return QString::number(result);
}



void MainWindow::on_TentoTwoBtn_clicked()
{
    QString Ten = ui->lineEdit->text();
        QString TentoTwo = decTobin(Ten);
        ui->lineEdit->setText(TentoTwo);
}

void MainWindow::on_TentoSixteenBrn_clicked()
{
    QString Ten = ui->lineEdit->text();
        QString TentoSixteen = decToHex(Ten);
        ui->lineEdit->setText(TentoSixteen);
}

void MainWindow::on_TwotoTenBrn_clicked()
{
    QString Two = ui->lineEdit->text();
    QString TwotoTen = binToDec(Two);
    ui->lineEdit->setText(TwotoTen);
}

void MainWindow::on_SinteentoTenBrn_clicked()
{
    QString Sinteen = ui->lineEdit->text();
    QString SinteentoTen = hexToDec(Sinteen);
    ui->lineEdit->setText(SinteentoTen);
}

void MainWindow::on_EighttoTenBrn_clicked()
{
    QString Eight = ui->lineEdit->text();
    QString EighttoTen = octToDec(Eight);
    ui->lineEdit->setText(EighttoTen);
}

void MainWindow::on_TentoEightBrn_clicked()
{
    QString Ten = ui->lineEdit->text();
    QString TentoEight = DecToOct(Ten);
    ui->lineEdit->setText(TentoEight);
}

void MainWindow::on_SixteentoTwoBtn_clicked()
{
    QString hexadecimal = ui->lineEdit->text();
    QString binary = decTobin(hexToDec(hexadecimal));
    ui->lineEdit->setText(binary);
}

void MainWindow::on_TwotoSixteenBtn_clicked()
{
    QString binary = ui->lineEdit->text();
    QString hexadecimal = decToHex(binToDec(binary));
    ui->lineEdit->setText(hexadecimal);
}

void MainWindow::on_yiyuanyici_clicked()
{
    new_yiyuanyicihanshu=new yiyuanyicihanshu;
    new_yiyuanyicihanshu->show();
}

void MainWindow::on_yiyuanerci_clicked()
{
    new_yiyuanercihanshu=new yiyuanercihanshu;
    new_yiyuanercihanshu->show();
}

void MainWindow::on_bijiaodaxiaoBtn_clicked()
{
    new_bijiaodaxiao=new bijiaodaxiao;
    new_bijiaodaxiao->show();
}
