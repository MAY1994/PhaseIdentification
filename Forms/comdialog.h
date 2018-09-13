#ifndef COMDIALOG_H
#define COMDIALOG_H
#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include "User/combase.h"
#include "ui_MainWindow.h"
#include "User/algorithm.h"
#include <QTimer>
#include <QTime>
#define Frame_Head "68"
#define Frame_Tail "16"
#define Leading_Byte "FEFEFEFE"
#define TIME_OUT 200 //串口读写的延时
#define SinglePhase true
#define ThreePhase false
#include "User/database.h"
#include <BaseTsd.h>

typedef struct _FRAME //帧结构
{
    quint8 uHead;	//帧起始符
    quint8 resAddr[6];//源地址域
    quint8 uStart;
    quint8 control;
    quint8 len;
    quint8 DataSign[4];
    quint8 Data[6];
    quint8 CS;
    quint8 uEnd;
}Frame_t;//


namespace Ui {
class ComDialog;
}

class ComDialog : public QDialog,public Combase
{
    Q_OBJECT

public:
    explicit ComDialog(QWidget *parent = 0);
    ~ComDialog();
    QSerialPort *myCom;       //公共的，在mainwindow里会用到
    void setMainUiPoint(Ui::MainWindow *ui){main_ui = ui;}
    bool WorkOpen();             //打开串口
    void WorkClose();            //关闭串口
    void setComEditable(bool editable);
    void WorkSendMsg(bool isSinglePhase);          //发送数据
    QString frame_Constructor(bool isSinglePhase);
    void Msg_Send_Show(bool isSinglePhase);
    void sleep(unsigned int msec);
    int DataStorage(QByteArray byte_data);
    void ComDataDisplay(QByteArray Recv);
    int Del51ToHex(quint8 data);
    double PhaseIdentification();
    Algorithm *myAlgorithm;
    QTimer *myTime;
    QTimer *RecvTimer;
    QTimer *EndTimer;
    DataBase *dataBase;
    Frame_t *frame_t;
    bool isFirst;
    List Array_V_P_1;
    List Array_V_P_A;
    List Array_V_P_B;
    List Array_V_P_C;
    QByteArray Recv_buf;
    int SendCount;
    int RecvCount;
    //QList ports;
private slots:
    void ReadMyCom();

    void on_OpenComButton_clicked();

    void on_ComButtonY_clicked();

    void on_ComButtonN_clicked();

    void on_CloseComButton_clicked();

    void on_StartButton_clicked();

    void on_Phase1AddressLineEdit_textChanged();

    void on_Phase3AddressLineEdit_textChanged();

    void AutoSend();

    void Recv_Timeout();

    void on_DataTypeEdit_1_textChanged();

    void EndTimeout();

    void on_EndButton_clicked();

    void on_ClearButton_clicked();

private:
    Ui::ComDialog *ui;
    Ui::MainWindow *main_ui;
};

#endif // COMDIALOG_H
