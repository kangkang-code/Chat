#include "dialoglist.h"
#include "ui_dialoglist.h"
#include<QToolButton>
#include"widget.h"
#include<QMessageBox>
DialogList::DialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);


    //设置标题
    setWindowTitle("MySelfQQ 2021");
    //设置图标
    setWindowIcon(QPixmap(":/images/qq.png"));

    //准备图标
    QList<QString>nameList;
     nameList << "Frustré" << "NSFW专员" <<"刘里个威"<<"玄不改非 氪不改命"<<"不想再感觉"
              <<"不欢迎"<<"不欢迎②号";


     QStringList iconNameList; //图标资源列表
     iconNameList << "Frustr"<< "cellphonecao" <<"duguwei" <<"saobao"<< "thebin"
                  <<"kangkang"<<"kangkangtwo";

     //通过一个容器维护住这个循环
     QVector <QToolButton *>vToolBtn;


    for (int i=0;i<7 ;i++ ) {

    //设置头像
    QToolButton * btn = new QToolButton;
    //设置文字
    btn->setText(nameList[i]);
    //设置头像
    QString str = QString(":/images/%1.png").arg(iconNameList.at(i)); //拼接字符串，通过循环设置头像
    btn->setIcon(QPixmap(str));
    //设置头像大小
    btn->setIconSize(QPixmap(str).size()); //设置成图片的大小
    //设置按钮风格 透明
    btn->setAutoRaise(true);
    //设置文字和图片一起显示
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //加到垂直布局中
    ui->vlayout->addWidget(btn);
    vToolBtn.push_back(btn); //容器维护循环，即保存住这7个按钮，方便以后操作
    //9个标示默认初始化
    isShow.push_back(false);
    }

    //对7个按钮进行添加信号槽
    for (int i=0;i<vToolBtn.size() ;i++ ) {
        connect(vToolBtn[i],&QToolButton::clicked,[=](){      //这里一定要加mutable，值传递想要修改属性必须加

            //如果被打开了，就不要再次打开
            if(isShow[i])
            {
                QString str = QString("%1的窗口已经被打开了").arg(vToolBtn[i]->text());
                QMessageBox::warning(this,"警告",str);
                return;
            }

            isShow[i]=true;

            //弹出聊天对话框
            //构造聊天窗口的时候，告诉这个窗口它的名字 参数1：顶层方式弹出，参数2：窗口名字
            //注意，widget构造函数并没有这两个参数
            Widget* widget = new Widget(0,vToolBtn[i]->text());
            //设置窗口标题
            widget->setWindowTitle(vToolBtn[i]->text());
            //设置窗口图标，即是每个人的头像
            widget->setWindowIcon(vToolBtn[i]->icon());
            widget->show(); //展示窗口

            //监听窗口关闭的信号
            connect(widget,&Widget::closeWidget,[=](){
                isShow[i]=false;
            });
        });
    }
}

DialogList::~DialogList()
{
    delete ui;
}
