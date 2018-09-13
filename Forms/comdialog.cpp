#include "comdialog.h"
#include "ui_comdialog.h"
#include <QMessageBox>

ComDialog::ComDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("串口配置"));

    myCom = NULL;
    myAlgorithm = new Algorithm();
    myTime = new QTimer();
    RecvTimer = new QTimer();
    EndTimer = new QTimer();
    dataBase = new DataBase();
    frame_t = (Frame_t *)malloc(sizeof(Frame_t));
    connect(myTime,SIGNAL(timeout()),this,SLOT(AutoSend()));
    isFirst = true;
    Recv_buf = NULL;    //缓冲区
    SendCount = 0;
    RecvCount = 0;
    //读取连接的串口,QextSerialPort类版本太低，getPorts()无法正常使用，使用QT5自带的QSerialPort可解决

    foreach(const QSerialPortInfo &port, QSerialPortInfo::availablePorts()){
        ui->PortNameComboBox->addItem(port.portName());
    }

    connect(RecvTimer,SIGNAL(timeout()),this,SLOT(Recv_Timeout()));
    connect(EndTimer,SIGNAL(timeout()),this,SLOT(EndTimeout()));

}

ComDialog::~ComDialog()
{
    delete ui;
}

/*
 * 打开串口函数
 */

bool ComDialog::WorkOpen()
{
    //选择串口号
    QString PortName = ui->PortNameComboBox->currentText();
    myCom = new QSerialPort(PortName);
    //串口数据读取连接
    connect(myCom,SIGNAL(readyRead()),this,SLOT(ReadMyCom()));

    //设置波特率
    myCom->setBaudRate(ui->BaudRateComboBox->currentText().toInt());

    //设置数据位
    myCom->setDataBits((QSerialPort::DataBits)ui->DataBitComboBox->currentText().toInt());
    //设置校验位
    switch(ui->ParityComboBox->currentIndex())
    {
    case 0:
        myCom->setParity(QSerialPort::NoParity);
        break;
    case 1:
        myCom->setParity(QSerialPort::OddParity);
        break;
    case 2:
        myCom->setParity(QSerialPort::EvenParity);
        break;
    default:
        myCom->setParity(QSerialPort::NoParity);
        //QDebug("set to default : PAR_NONE");
        break;
    }

    //设置停止位
    switch(ui->StopBitComboBox->currentIndex())
    {
    case 0:
        myCom->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        myCom->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        myCom->setStopBits(QSerialPort::TwoStop);
        break;
    default:
        myCom->setStopBits(QSerialPort::OneStop);
        //QDebug("set to default : STOP_1");
        break;
    }

    //设置数据流控制
    myCom->setFlowControl(QSerialPort::NoFlowControl);

    //设置延时

    //以可读写方式打开串口
    bool flag = myCom->open(QIODevice::ReadWrite);

    if (false == flag)
    {
        QMessageBox::warning(this,tr("Warning"),tr("串口不存在或已被占用！"),QMessageBox::Ok);
        return false;
    }
    ui->ComStatusLabel->setText("串口已开启！");
    main_ui->MainWindowComStatus->setText("串口已开启！");
    main_ui->StartButton->setEnabled(true);
    return true;
}

void ComDialog::WorkClose()
{
    myCom->close();
    ui->ComStatusLabel->setText("串口已关闭！");
    main_ui->MainWindowComStatus->setText("串口已关闭！");
    setComEditable(true);
    main_ui->StartButton->setEnabled(false);
}

/*
 * 串口接收数据处理
 */
void ComDialog::ReadMyCom()
{
    /*读取数据持续100ms，将串口数据放入Recv_buf缓冲区，100ms到了之后
     * 调用Recv_Timeout()函数对缓冲区数据进行处理、储存并显示。
     */
    RecvTimer->start(100);
    Recv_buf.append(myCom->readAll());
}

/*
 * 串口数据接收超时函数
 */
void ComDialog::Recv_Timeout(){
    RecvTimer->stop();
    DataStorage(Recv_buf);
    ComDataDisplay(Recv_buf);
    Recv_buf.clear();    //清空缓冲区
    main_ui->RecvCount_Text->setText(QString::number(++RecvCount));
}

/*
 * 串口数据存储函数
 * 输入为缓冲区
 * 返回值 -1 代表不是完整帧
 *        0 代表异常应答
 *        2 代表写数据正常应答
 */
int ComDialog::DataStorage(QByteArray byte_data){
    double RetV = 0;
    double RetV_A = 0;
    double RetV_B = 0;
    double RetV_C = 0;
    QString str;
    //帧头不为68，放弃存储
    if(!memicmp(Frame_Head,(unsigned char *)byte_data.data(),1))
        return -1;

    memcpy(&(frame_t->uHead),(unsigned char *)byte_data.data(),1);
    memcpy(&(frame_t->resAddr),(unsigned char *)byte_data.data()+1,6);
    memcpy(&(frame_t->uStart),(unsigned char *)byte_data.data()+7,1);
    memcpy(&(frame_t->control),(unsigned char *)byte_data.data()+8,1);
    memcpy(&(frame_t->len),(unsigned char *)byte_data.data()+9,1);

    switch(frame_t->len){
    case 0:
        return 2;
    case 1:
        return 0;
    case 6:  //数据域长度为6
        memcpy(&(frame_t->DataSign),(unsigned char *)byte_data.data()+10,4);
        memcpy(&(frame_t->Data),(unsigned char *)byte_data.data()+14,2);
        memcpy(&(frame_t->CS),(unsigned char *)byte_data.data()+16,1);
        memcpy(&(frame_t->uEnd),(unsigned char *)byte_data.data()+17,1);
        RetV = Del51ToHex(frame_t->Data[0]) * 0.1 + Del51ToHex(frame_t->Data[1]) * 10;
        dataBase->AddElem(Array_V_P_1,RetV);
        str = " " + QString::number(RetV) + "V";
        main_ui->MsgTextArea_2->insertPlainText(str);
        main_ui->MsgTextArea_2->insertPlainText("\n");

        return 1;
    case 10:
        memcpy(&(frame_t->DataSign),(unsigned char *)byte_data.data()+10,4);
        memcpy(&(frame_t->Data),(unsigned char *)byte_data.data()+14,6);
        memcpy(&(frame_t->CS),(unsigned char *)byte_data.data()+20,1);
        memcpy(&(frame_t->uEnd),(unsigned char *)byte_data.data()+21,1);
        RetV_A = Del51ToHex(frame_t->Data[0]) * 0.1 + Del51ToHex(frame_t->Data[1]) * 10;
        dataBase->AddElem(Array_V_P_A,RetV_A);
        RetV_B = Del51ToHex(frame_t->Data[2]) * 0.1 + Del51ToHex(frame_t->Data[3]) * 10;
        dataBase->AddElem(Array_V_P_B,RetV_B);
        RetV_C = Del51ToHex(frame_t->Data[4]) * 0.1 + Del51ToHex(frame_t->Data[5]) * 10;
        dataBase->AddElem(Array_V_P_C,RetV_C);
        str = "             " + QString::number(RetV_A) + "V        " + QString::number(RetV_B)
                + "V       " + QString::number(RetV_C) + "V";
        main_ui->MsgTextArea_2->insertPlainText(str);
        main_ui->MsgTextArea_2->insertPlainText("\n");
        return 1;
    }
    return 0;
}
/*
 *
 */
int ComDialog::Del51ToHex(quint8 data){
    int decade = 0;
    int unit = 0;
    int ret = 0;
    data = data - 51;
    decade = data / 16;
    unit = data % 16;
    ret = decade * 10 + unit;
    return ret;
}

/*
 * 串口接收数据显示函数,将读取到的数据进行转化然后显示
 */
void ComDialog::ComDataDisplay(QByteArray Recv){
    QString strDisplay = NULL;
    QString str = Recv.toHex().data();
    str = str.toUpper();
    for(int i = 0;i<str.length();i+=2)
    {
        QString st = str.mid(i,2);
        strDisplay += st;
        strDisplay += " ";
    }

    main_ui->MsgTextArea_1->insertPlainText(strDisplay);    //显示接收帧
    main_ui->MsgTextArea_1->insertPlainText("\n");
}

/*
 *构造发送帧，单相表帧：68 A0-A5 68 11(C) 04(L) 33 34 34 35 CS 16
 * 三相表帧：68 A0-A5 68 11(C) 04(L) 33 32 34 35 CS 16
 */
QString ComDialog::frame_Constructor(bool isSinglePhase)
{
    QString destAddr = NULL;
    QString DataSign = NULL;
    QString Data = NULL;
    QString frame = Frame_Head;
    QString ControlNum = "11";
    QString Len = "04";        //直接已知是4，如果还需要其他命令，再另加一个函数

    QString CS = NULL;

    if (isSinglePhase){
        destAddr = main_ui->Phase1AddressLineEdit->text();
        DataSign = main_ui->DataTypeEdit_1->text();
    }
    else{
        destAddr = main_ui->Phase3AddressLineEdit->text();
        DataSign = main_ui->DataTypeEdit_2->text();
    }
    for(int i = destAddr.length()-2;i>=0;i-=2)
    {
        QString st = destAddr.mid(i,2);
        frame +=st;
    }
    frame = frame + Frame_Head + ControlNum + Len;
    DataSign = myAlgorithm->Add33H(DataSign);
    for(int i = DataSign.length()-2;i>=0;i-=2)
    {
        QString st = DataSign.mid(i,2);
        frame +=st;
    }
    CS = myAlgorithm->Sum_Calculate(frame,frame.length());
    frame = frame+CS+Frame_Tail;   //未加前导符

    return frame;
}

/*
 * 发送帧显示函数
 */
void ComDialog::Msg_Send_Show(bool isSinglePhase){
    QString Display = NULL;
    QString frame = frame_Constructor(isSinglePhase);

    for (int i = 0 ; i <= frame.length() - 2;i+=2){
        QString st = frame.mid(i,2);
        Display += st + " ";
    }
    if (isSinglePhase)
        main_ui->lineEdit_Frame_1->setText(Display);
    else
        main_ui->lineEdit_Frame_2->setText(Display);
}

/*
 * 串口数据发送函数
 */
void ComDialog::WorkSendMsg(bool isSinglePhase)
{
    if (!(myCom->isOpen())) return;

    QString frame = frame_Constructor(isSinglePhase);
    QByteArray sendBuf = QByteArray::fromHex(frame.toLatin1().data());
    myCom->write(sendBuf);
}

/*
 * 开始函数
 */
void ComDialog::on_StartButton_clicked()
{
    QTime StartTime = QTime::currentTime();

    int frequency = main_ui->CollectFrequency_Button->currentText().toInt();  //采集频率：秒
    int collectTime = main_ui->CollectTimeButton->currentText().toInt();  //采集总时间,小时
    main_ui->StartTimeLabel->setText(StartTime.toString());
    main_ui->EndTimeLabel->setText((StartTime.addSecs(collectTime * 3600)).toString());
    main_ui->EndButton->setEnabled(true);
    Array_V_P_1 = dataBase->MakeEmpty();    //存储单相表电压序列
    Array_V_P_A = dataBase->MakeEmpty();    //存储三相表A相电压序列
    Array_V_P_B = dataBase->MakeEmpty();    //存储三相表B相电压序列
    Array_V_P_C = dataBase->MakeEmpty();    //存储三相表C相电压序列

    myTime->start(frequency * 1000);    //采集频率定时器
    EndTimer->start(collectTime * 3600 *1000);    //采集总时间定时器，到时就停止计时，并计算电压相关性
    AutoSend();
}

void ComDialog::EndTimeout(){
    myTime->stop();
    EndTimer->stop();
    double ans = PhaseIdentification();
    QString display = "\nThe biggist correlation is " + QString::number(ans);
    main_ui->Result_textBrowser->insertPlainText(display);
}

double ComDialog::PhaseIdentification(){
    double corr_Single_A = 0;
    double corr_Single_B = 0;
    double corr_Single_C = 0;
    double maxcorr = 0;

    corr_Single_A = myAlgorithm->CorrelationCal(Array_V_P_1->Data,Array_V_P_A->Data,Array_V_P_1->Last + 1);
    corr_Single_B = myAlgorithm->CorrelationCal(Array_V_P_1->Data,Array_V_P_B->Data,Array_V_P_1->Last + 1);
    corr_Single_C = myAlgorithm->CorrelationCal(Array_V_P_1->Data,Array_V_P_C->Data,Array_V_P_1->Last + 1);
    maxcorr = corr_Single_A > corr_Single_B ? corr_Single_A > corr_Single_C ? corr_Single_A
           :corr_Single_C :corr_Single_B > corr_Single_C ? corr_Single_B : corr_Single_C;
    main_ui->Result_textBrowser->insertPlainText("A相相关系数：" + QString::number(corr_Single_A));
    main_ui->Result_textBrowser->insertPlainText(" B相相关系数：" + QString::number(corr_Single_B));
    main_ui->Result_textBrowser->insertPlainText(" C相相关系数：" + QString::number(corr_Single_C));

    return maxcorr;
}

void ComDialog::AutoSend()
{
    WorkSendMsg(SinglePhase);
    main_ui->SendCount_Text->setText(QString::number(++SendCount));
    sleep(500);
    WorkSendMsg(ThreePhase);
    main_ui->SendCount_Text->setText(QString::number(++SendCount));
}

/*
 * 延时函数： 延时msec毫秒
 */
void ComDialog::sleep(unsigned int msec){
    QTime reachTime = QTime::currentTime().addMSecs(msec);

    while(QTime::currentTime() < reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}

void ComDialog::on_OpenComButton_clicked()
{
    setComEditable(true);
    if (myCom != NULL)
        delete myCom;
    if(WorkOpen())
    {
        setComEditable(false);
        this->close();
    }
}

void ComDialog::on_CloseComButton_clicked()
{
    WorkClose();
}

void ComDialog::on_ComButtonY_clicked()
{
    this->close();
}

void ComDialog::on_ComButtonN_clicked()
{
    this->close();
}

void ComDialog::setComEditable(bool editable)
{
    ui->groupBox->setEnabled(editable);
    ui->OpenComButton->setEnabled(editable);
    ui->CloseComButton->setEnabled(!editable);
}

void ComDialog::on_Phase1AddressLineEdit_textChanged()
{
    if(main_ui->Phase1AddressLineEdit->text().length() != 12)
    {
        main_ui->lineEdit_Frame_1->setText("");
        return;
    }
    else
    {
        Msg_Send_Show(SinglePhase);
    }
}

void ComDialog::on_Phase3AddressLineEdit_textChanged()
{
    if(main_ui->Phase3AddressLineEdit->text().length() != 12)
    {
        main_ui->lineEdit_Frame_2->setText("");
        return;
    }
    else
    {
        Msg_Send_Show(ThreePhase);
        return;
    }
}

void ComDialog::on_DataTypeEdit_1_textChanged()
{
    if(main_ui->DataTypeEdit_1->text().length() == 12)
        Msg_Send_Show((SinglePhase));
}

void ComDialog::on_EndButton_clicked()
{
    EndTimeout();
    main_ui->EndButton->setEnabled(false);
}

void ComDialog::on_ClearButton_clicked()
{
    main_ui->MsgTextArea_1->clear();
    main_ui->MsgTextArea_2->clear();
    SendCount = 0;
    RecvCount = 0;
}
