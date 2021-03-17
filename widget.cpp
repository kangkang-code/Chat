#include "widget.h"
#include "ui_widget.h"
#include<QDataStream>
#include<QMessageBox>
#include<QDateTime>
#include<QColorDialog>
#include<QFileDialog>
#include<QFile>
#include<QTextStream> //用文本流保存数据
#include<QComboBox>
Widget::Widget(QWidget *parent,QString name) //构造的实现，这里也要添加QString name
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //初始化
    udpSocket = new QUdpSocket(this);

    uName = name;  //用户名获取

    this->port = 9999; //端口号

    udpSocket->bind(this->port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint); //绑定端口号，参数分别为绑定地址，绑定模式，断线重连

    //发送新用户进入
    sendMsg(UsrEnter);

    //点击发送按钮发消息
    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        sendMsg(Msg);
    });

    //监听别人发送的数据
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::ReceiveMessage);

    //点击退出按钮 实现关闭窗口
    connect(ui->exitBtn,&QPushButton::clicked,[=](){
        this->close();
    });

    ////////////辅助功能////////

    //字体
    connect(ui->fontCbx,&QFontComboBox::currentFontChanged,[=](const QFont &font){
        ui->msgTxEdit->setCurrentFont(font);
        ui->msgTxEdit->setFocus();
    });

    // 设置字号
    void(QComboBox:: *cbxsignal)(int) = &QComboBox::QComboBox::currentIndexChanged;
    connect(ui->sizeCbx,cbxsignal,[=](int text){
        ui->msgTxEdit->setFontPointSize(text);
        ui->msgTxEdit->setFocus();

    });

    //加粗
    connect(ui->boldTBtn,&QToolButton::clicked,[=](bool isCheck){
        if(isCheck)
        {
        ui->msgTxEdit->setFontWeight(QFont::Bold); //加粗
        }
        else
        {
            ui->msgTxEdit->setFontWeight(QFont::Normal);
        }
    });

    //倾斜
    connect(ui->italicTBtn,&QToolButton::clicked,[=](bool check){
       ui->msgTxEdit->setFontItalic(check);
    });

    //下划线
    connect(ui->underlineTBtn,&QToolButton::clicked,[=](bool check){
        ui->msgTxEdit->setFontUnderline(check);
    });

    //字体颜色
    connect(ui->colorTBtn,&QToolButton::clicked,[=](){
       QColor color =  QColorDialog::getColor(Qt::red);
       ui->msgTxEdit->setTextColor(color);
    });

    //清空聊天记录
    connect(ui->clearTBtn,&QToolButton::clicked,[=](){
        ui->msgBrowser->clear();
    });

    //保存聊天记录
    connect(ui->savaTBtn,&QToolButton::clicked,this,[=](){
                if(ui->msgBrowser->document()->isEmpty())
                {
                    QMessageBox::warning(this,"警告","聊天记录为空，无法保存！");
                            return ;
                }
                else
                {
                    QString fName = QFileDialog::getSaveFileName(this,"保存聊天记录","聊天记录","(*.txt)");
                    if(!fName.isEmpty())
                    {
                        //保存名称不为空再做保存操作
                        QFile file(fName);
                        file.open(QIODevice::WriteOnly | QFile::Text);

                        QTextStream stream(&file);
                        stream << ui->msgBrowser->toPlainText();
                        file.close();
                    }
                }
            });

}

void Widget::ReceiveMessage()
{
    //获取数据报文
    //获取长度
    qint64 size = udpSocket->pendingDatagramSize();
    QByteArray array = QByteArray(size,0);
    udpSocket->readDatagram(array.data(),size);

    //解析数据
    //第一段 类型 第二段 用户名 第三段 内容
    QDataStream stream (&array,QIODevice::ReadOnly); //往array里流，还要读数据

    int msgType; //读取到类型
    QString usrName; //用户名
    QString msg; //内容

    //获取当前时间
   QString time =  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    stream>>msgType;

    switch (msgType) {
    case Msg: //普通聊天
        stream>>usrName>>msg;

        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::blue); //设置颜色
        ui->msgBrowser->append("["+usrName+"]"+time); //获取当前时间
        ui->msgBrowser->append(msg); //具体发送内容
        break;
    case UsrEnter:
        stream>>usrName; //先获取名
         usrEnter(usrName);

        break;
    case UsrLeft:
        stream>>usrName; //获取名
        usrLeft(usrName,time); //获取离开的时间
        break;
    default:
        break;
    }


} //接受UDP消息

void Widget::usrLeft(QString usrname, QString time)
{
    //更新右侧tableWidget
   bool isEmpty =  ui->usrTb1Widget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(!isEmpty)
    {
        int row = ui->usrTb1Widget->findItems(usrname,Qt::MatchExactly).first()->row();
        ui->usrTb1Widget->removeRow(row);
    }

    //追加聊天记录
    ui->msgBrowser->setTextColor(Qt::gray); //设置颜色
    ui->msgBrowser->append(QString("%1 于 %2离开").arg(usrname).arg(time));
    //在线人数更新
    ui->usrNumLbl->setText(QString("在线用户：%1人").arg(ui->usrTb1Widget->rowCount()));
}

void Widget::usrEnter(QString username)
{
    //更新右侧TableWidget

   bool isEmpty =  ui->usrTb1Widget->findItems(username,Qt::MatchExactly).isEmpty();
   if(isEmpty)
   {
       QTableWidgetItem * usr = new QTableWidgetItem(username);

       //插入行
       ui->usrTb1Widget->insertRow(0);
       ui->usrTb1Widget->setItem(0,0,usr);

       //追加聊天记录
       ui->msgBrowser->setTextColor(Qt::gray);
       ui->msgBrowser->append(QString("%1 进入了聊天室").arg(username));
       //在线人数更新
       ui->usrNumLbl->setText(QString("在线用户：%1人").arg(ui->usrTb1Widget->rowCount()));

       //把自身信息广播出去，即上面的步骤是别人广播给自己的信息，我们自身的信息也要广播给别人
       sendMsg(UsrEnter); //这里是不会进入死循环的，因为前面加了限定条件，只有当empty的时候才会执行这段代码
   }
} //新用户进入

void Widget::sendMsg(MsgType type)
{
    //发送的消息分为三种类型
    //发送的数据要做分段处理，第一段：类型，第二段：用户名，第三段：具体的内容

    QByteArray array;

    QDataStream stream(&array,QIODevice::WriteOnly); //使用QDataStream数据流往array里面进行写数据操作

    //开始拼接
    stream<<type<<getUsr(); //第一段内容，添加到流中，流又把内容添加到了array中，第二段：用户名

    switch (type) {
    case Msg:  //普通消息发送
        if(ui->msgTxEdit->toPlainText()=="") //如果用户没有输入内容，不发任何消息
        {
            QMessageBox::warning(this,"警告","发送内容不能为空");
            return;
        }
        //追加第三段数据，具体说的话
        stream<<getMsg();

        break;
    case UsrEnter:  //新用户进入消息

        break;
     case UsrLeft:  //用户离开消息
        break;
    default:
        break;
    }

    //书写报文 ,广播发送
    udpSocket->writeDatagram(array,QHostAddress::Broadcast,port);

} //广播UDP消息

QString Widget::getUsr()
{
    return this->uName;
}

QString Widget::getMsg()
{
    QString str = ui->msgTxEdit->toHtml();

    //清空输入框
    ui->msgTxEdit->clear(); //清空聊天记录
    ui->msgTxEdit->setFocus(); //让光标回到起始点
    return str;
}//获取聊天信息
void Widget::closeEvent(QCloseEvent *e)
{

    emit this->closeWidget();
    sendMsg(UsrLeft); //发送用户离开的消息
    //断开套接字
    udpSocket->close();
    udpSocket->destroyed();

    QWidget::closeEvent(e);
}

Widget::~Widget()
{
    delete ui;
}

