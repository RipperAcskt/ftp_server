#include "make_dir.h"
#include "ui_make_dir.h"

make_dir::make_dir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::make_dir)
{
    ui->setupUi(this);

    user = new User(0);
}

make_dir::~make_dir()
{
    delete ui;
}

void make_dir::set_user(User *user){
    this->user = user;
}
void make_dir::on_ok_clicked()
{
    QString name = ui->name->text();

    QDir().mkdir(user->get_current_dir()+"/"+name);

    close();
    emit mainWindow();
}

