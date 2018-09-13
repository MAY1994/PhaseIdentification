#include "database.h"

DataBase::DataBase()
{

}

bool DataBase::AddElem(List PtrL,double elem){
    if (PtrL->Last >= MaxSize-1)
        return false;
    PtrL->Data[++PtrL->Last] = elem;
    return true;
}

List DataBase::MakeEmpty(){
    List PtrL;
    PtrL = (List)malloc(sizeof(struct DataBase_Array));
    PtrL->Last = -1;
    return PtrL;
}

int DataBase::Find(double elem,List PtrL){
    int i = 0;
    while( i <= PtrL->Last && PtrL->Data[i] != elem)
        i++;
    if(i > PtrL->Last) return -1;
    else return i;
}

void DataBase::Delete(int i,List PtrL){
    if( i < 1 || i >PtrL->Last)
        return;
    PtrL->Last--;
}
