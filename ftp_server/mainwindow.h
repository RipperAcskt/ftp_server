#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <fstream>
#include <QRadioButton>
#include <QPushButton>
#include "make_dir.h"
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void showAll();
    ~MainWindow();

private slots:
    void on_add_file_clicked();

    void on_create_folder_clicked();
    void clicked_button();

    void on_back_clicked();

    void on_delete_file_clicked();

private:
    Ui::MainWindow *ui;
    Server server;
    User *user = new User(0);
    make_dir *make;

};
#endif // MAINWINDOW_H
