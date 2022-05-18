#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_add_file_clicked()
{

    QStringList files = QFileDialog::getOpenFileNames(0, "Выберите файлы", QDir::homePath(), "");
    QStringList file_name;
    QString path = "/Users/ripper/Desktop/bsuir/spovm/ForthSemCourseWork/files/";
    std::string line;

    for(int i = 0; i < files.length(); i++){
        if(QFile::exists(files[i])) {
            file_name = files[i].split("/");
            std::ifstream in(files[i].toStdString());
            std::ofstream out((path + file_name[file_name.length()-1]).toStdString());
            while(std::getline(in, line)) out << line << '\n';
        }
    }
}


