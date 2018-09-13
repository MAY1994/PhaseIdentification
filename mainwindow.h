#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Forms/aboutdialog.h>
#include <Forms/comdialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ComDialog *P_Com;               //串口配置窗口声明
    AboutDialog *P_About;           //关于窗口声明


private slots:
    void MainWindow_Quit();         //关闭主界面
    void MainWindow_About();        //打开关于界面
    void MainWindow_Communicate();  //打开串口配置界面
};

#endif // MAINWINDOW_H
