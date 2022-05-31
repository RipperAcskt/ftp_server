#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!QDir("files").exists()) QDir().mkdir("files");

    QString path = QDir::currentPath() + "/" + "files";

    server = new Server(path);
    server->set_length_of_path(path.split("/").length());
    user->set_current_dir(path);
    qDebug() << QDir::currentPath();
    showAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_add_file_clicked()
{

    QStringList files = QFileDialog::getOpenFileNames(0, "Выберите файлы", QDir::homePath(), "");
    QStringList file_name;
    QString path = user->get_current_dir()+"/";
    std::string line;

    for(int i = 0; i < files.length(); i++){
        if(QFile::exists(files[i])) {
            file_name = files[i].split("/");
            std::ifstream in(files[i].toStdString());
            std::ofstream out((path + file_name[file_name.length()-1]).toStdString());
            while(std::getline(in, line)) out << line << '\n';
        }
    }
    ui->tableWidget->setRowCount(0);
    showAll();
}

void MainWindow::showAll(){
    struct stat statbuf;
    char buffer[1024];


    if(user->get_current_dir() == (QDir::currentPath() + "/" + "files")) ui->back->hide();
    else ui->back->show();

    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->hideColumn(4);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    user->get_all_files()->current_dir(user->get_current_dir().toUtf8().data());

    for(int i = 0; i < user->get_all_files()->get_num(); i++){
        stat(user->get_all_files()->get_all_files()[i].name.toUtf8().data(), &statbuf);

        for(int j = 0; j < BUFFER_SIZE; j++) buffer[i] = '\0';
        strmode(statbuf.st_mode, buffer);
        QString answer = QString::fromUtf8(buffer);

        ui->tableWidget->removeRow(i);
        ui->tableWidget->insertRow(i);

        QRadioButton *radioButton = new QRadioButton();
        radioButton->setChecked(false);
        radioButton->setCursor(Qt::PointingHandCursor);

        ui->tableWidget->setCellWidget(i, 0, radioButton);
        QStringList list = user->get_all_files()->get_all_files()[i].name.split("/");
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem (list[list.length()-1]));
        if(answer[0] == 'd'){
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem ("папка"));
            QPushButton *button = new QPushButton();
            button->setText("Перейти");
            button->setObjectName(list[list.length()-1]);
            ui->tableWidget->setCellWidget(i, 3, button);
            QObject::connect(button, SIGNAL(clicked()), this, SLOT(clicked_button()));
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem ("1"));
        }
        else{
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem ("файл"));
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem ("0"));
        }
    }
    ui->tableWidget->resizeColumnsToContents();
    user->get_all_files()->set_empty();
}


void MainWindow::on_create_folder_clicked()
{
    bool ok;
    QString name = QInputDialog().getText(0, "Создать папку", "Имя:", QLineEdit::Normal, "", &ok);
    if(!ok) return;
    QDir().mkdir(user->get_current_dir()+"/"+name);
    showAll();
}

void MainWindow::clicked_button(){
    user->set_current_dir(user->get_current_dir() + "/" + QObject::sender()->objectName());
    ui->tableWidget->setRowCount(0);
    showAll();
}

void MainWindow::on_back_clicked()
{
    QStringList list = user->get_current_dir().split("/");
    QString path;
    for(int i = 0; i < list.length()-1; i++) path += "/" + list[i];
    path.remove(0,1);
    user->set_current_dir(path);
    ui->tableWidget->setRowCount(0);
    showAll();
}


void MainWindow::on_delete_file_clicked()
{
    user->get_all_files()->current_dir(user->get_current_dir().toUtf8().data());

    QRadioButton *item = new QRadioButton();
    for(int i = 0; i < user->get_all_files()->get_num(); i++){
        item = (QRadioButton*)ui->tableWidget->cellWidget(i, 0);
        if(item->isChecked()){
            QString name = ui->tableWidget->item(i, 1)->text();
            if(ui->tableWidget->item(i, 4)->text() == "1"){
                QDir dir(user->get_current_dir() + "/" + name);
                dir.removeRecursively();
                break;
            }

            QFile::remove(user->get_current_dir() + "/" + name);
            break;
        }
    }
    user->get_all_files()->set_empty();
    ui->tableWidget->setRowCount(0);
    showAll();

}


void MainWindow::on_action_ip_triggered()
{
    bool ok;
    QString ip = QInputDialog().getText(0, "Задать IP", "IP:", QLineEdit::Normal, "", &ok);
    if(!ok) return;
    server->set_ip(ip);

}

