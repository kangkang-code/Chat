#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    enum MsgType{Msg,UsrEnter,UsrLeft};

public:
    Widget(QWidget *parent,QString name); //构造的声明，添加多一个参数
    ~Widget();

private:
    Ui::Widget *ui;

signals:
    //关闭窗口发送信息
    void closeWidget();

public:
    //关闭事件,这不是信号，不能写在signals下
    void closeEvent(QCloseEvent *);

public:
    void sendMsg(MsgType type); //广播udp信息
    void usrEnter(QString username); //处理新用户加入
    void usrLeft(QString usrname,QString time); //处理用户离开
    QString getUsr(); //获取用户名
    QString getMsg(); //获取聊天信息

private:
    QUdpSocket * udpSocket; //udp套接字
    qint16 port; //端口
    QString uName; //用户名

    void ReceiveMessage();
};
#endif // WIDGET_H
