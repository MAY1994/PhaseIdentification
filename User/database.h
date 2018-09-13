#ifndef DATABASE_H
#define DATABASE_H
#define MaxSize 610
#include <stdlib.h>
typedef struct DataBase_Array *List;
struct DataBase_Array{
    double Data[MaxSize];  //MaxSize 数组大小
    int Last;  //当前位置
};

class DataBase
{
public:
    DataBase();
    bool AddElem(List PtrL, double elem);
    List MakeEmpty();
    int Find(double elem, List PtrL);
    void Delete(int i,List PtrL);
};

#endif // DATABASE_H
