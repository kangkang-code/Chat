#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>

namespace Ui {
class DialogList;
}

class DialogList : public QWidget
{
    Q_OBJECT

public:
    explicit DialogList(QWidget *parent = nullptr);
    ~DialogList();

private:
    Ui::DialogList *ui;

    QVector<bool> isShow; //保存用户是否弹出,最好不要写进cpp中，那样就会变成局部变量
};

#endif // DIALOGLIST_H
