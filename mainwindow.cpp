#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置窗口标题
    setWindowTitle(tr("相位识别助手"));
    //窗口资源建立
    P_Com = NULL;
    P_Com = new ComDialog();
    P_Com->setMainUiPoint(ui);
    P_About = NULL;
    P_About = new AboutDialog();
    //菜单栏

    connect(ui->QuitBar,SIGNAL(triggered()),this,SLOT(MainWindow_Quit()));
    connect(ui->AboutBar,SIGNAL(triggered()),this,SLOT(MainWindow_About()));
    connect(ui->CommunicateBar,SIGNAL(triggered()),this,SLOT(MainWindow_Communicate()));
    connect(ui->DisconnectBar,SIGNAL(triggered(bool)),this,SLOT(MainWindow_Communicate()));
    connect(ui->Phase1AddressLineEdit,SIGNAL(textChanged(QString)),P_Com,SLOT(on_Phase1AddressLineEdit_textChanged()));
    connect(ui->Phase3AddressLineEdit,SIGNAL(textChanged(QString)),P_Com,SLOT(on_Phase3AddressLineEdit_textChanged()));
    connect(ui->StartButton,SIGNAL(clicked(bool)),P_Com,SLOT(on_StartButton_clicked()));
    connect(ui->DataTypeEdit_1,SIGNAL(textChanged(QString)),P_Com,SLOT(on_DataTypeEdit_1_textChanged()));
    connect(ui->EndButton,SIGNAL(clicked(bool)),P_Com,SLOT(on_EndButton_clicked()));
    connect(ui->ClearButton,SIGNAL(clicked(bool)),P_Com,SLOT(on_ClearButton_clicked()));

    ui->Phase1AddressLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->StartButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MainWindow_Quit()
{
    this->close();
}

void MainWindow::MainWindow_About()
{
    if(P_About != NULL)
    {
        P_About->exec();
    }
}

void MainWindow::MainWindow_Communicate()
{
    if(P_Com != NULL)
    {
        P_Com->exec();
    }
}
