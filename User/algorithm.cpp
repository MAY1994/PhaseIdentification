#include "algorithm.h"


Algorithm::Algorithm()
{

}

double Algorithm::CorrelationCal(double * x,double * y,int count)
{
    double d1 = 0;
    double d2 = 0;
    double d3 = 0;
    double mx = 0;
    double my = 0;
    int i;

    for (i = 0;i<count;i++)
    {
        mx += x[i];
        my += y[i];
    }
    mx = mx/count;		//Ex
    my = my/count;		//Ey

    for (i=0;i<count;i++)
    {
        d1 += (x[i]-mx)*(y[i]-my);
        d2 += (x[i]-mx)*(x[i]-mx);
        d3 += (y[i]-my)*(y[i]-my);
    }
    d2 = sqrt(d2*d3);
    if (d2 == 0)
    {
        d1 = -1;
    }
    else
    {
        d1 = d1/d2;
    }
    return d1;
}

QString Algorithm::Sum_Calculate(QString Msg, int usDataLen)
{
    QString CSValue = 0;
    bool isUnit = false;   //个位标识
    bool ok;
    int sum = 0;

    for (int i=0;i < usDataLen;i++)   //将QString转化为10进制，计算过后再转化为UINT8
    {
        QString str = Msg.mid(i,1);\
        if(isUnit){
            sum += str.toInt(&ok,10);
            isUnit = false;
        }
        else{
            sum += str.toInt(&ok,10) * 16;
            isUnit = true;
        }
    }
    CSValue = QString::number((sum % 256),16).toUpper();
    return CSValue;
}

QString Algorithm::Add33H(QString Msg)
{
    QString ret = "";
    bool ok;
    int temp = 0;
    int carry = 0;
    bool isUnit = true;   //个位标识

    for(int i = Msg.length()-1;i>=0;i--){
        QString str = Msg.mid(i,1);
        if(isUnit){
            carry = (str.toInt(&ok,16)+ 3)/16;
            temp = (str.toInt(&ok,16) + 3)%16;
            isUnit = false;
        }
        else{
            temp = (str.toInt(&ok,16) + 3 + carry) % 16;
            isUnit = true;
            carry = 0;
        }
        ret += QString::number(temp,16).toUpper();
    }
    std::reverse(ret.begin(),ret.end());

    return ret;
}
