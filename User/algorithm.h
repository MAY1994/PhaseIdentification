#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <BaseTsd.h>
#include <QString>
#include <math.h>
class Algorithm
{
public:
    Algorithm();
    QString Sum_Calculate(QString Msg, int usDataLen);
    QString Add33H(QString Msg);
    double CorrelationCal(double * x,double * y,int count);
};

#endif // ALGORITHM_H
