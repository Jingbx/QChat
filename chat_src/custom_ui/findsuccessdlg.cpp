#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QDir>
// #include "applyfriend.h"
#include <memory>
#include "../custom_ui_wid/clicked_btn.h"
#include "applyfriend.h"


FindSuccessDlg::FindSuccessDlg(QWidget *parent) : QDialog(parent),
    ui(new Ui::FindSuccessDlg), _parent(parent)
{
    ui->setupUi(this);
    // 设置对话框标题
    setWindowTitle("添加");
    qDebug() << "显示添加好友弹窗";
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("FindSuccessDlg");
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path +
                             QDir::separator() + "static"+QDir::separator()+"head_1.png");
    qDebug() << pix_path;
    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    ui->add_friend_btn->setState("normal","hover","press");
    ui->close_add_friend_btn->setState("normal","hover","press");
    this->setModal(true);
}

FindSuccessDlg::~FindSuccessDlg()
{
    qDebug() << "FindSuccessDlg析构";
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    _si = si;
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    qDebug() << "Clicked add_friend_btn!";
    this->hide();
   //弹出加好友界面
    auto applyFriend = new ApplyFriend(_parent);
    applyFriend->SetSearchInfo(_si);
    applyFriend->setModal(true);
    applyFriend->show();
}

void FindSuccessDlg::on_close_add_friend_btn_clicked() {
    qDebug() << "Clicked close_btn!";
    this->hide();
}

