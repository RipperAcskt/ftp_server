#ifndef MAKE_DIR_H
#define MAKE_DIR_H

#include <QDialog>
#include <QDir>
#include "user.h"

namespace Ui {
class make_dir;
}

class make_dir : public QDialog
{
    Q_OBJECT

public:
    explicit make_dir(QWidget *parent = nullptr);
    ~make_dir();

    void set_user(User *user);
signals:
    void mainWindow();

private slots:
    void on_ok_clicked();

private:
    Ui::make_dir *ui;
    User *user;
};

#endif // MAKE_DIR_H
