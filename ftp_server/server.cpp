#include "server.h"

Server::Server(QString path)
{
    dir = path;

    for(int i = 0; i < QUEUE_SIZE; i++) indexes.push(9-i);

    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(SERVER_PORT20);

    sem = semget(2001, 1, 0666 | IPC_CREAT);
    semctl(sem, 0, SETVAL, 1);

    if((s20 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) qDebug() << "ошибка сокета";
    if(setsockopt(s20, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) qDebug() << "ошибка setsockopt REUSEADDR";
    if(setsockopt(s20, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) qDebug() << "ошибка setsockopt REUSEPORT";
    if((bind(s20, (struct sockaddr *) &channel, sizeof(channel))) < 0) qDebug() << "ошибка связывания";

    if(listen(s20, QUEUE_SIZE) < 0) qDebug() << "ошибка ожидания";

    //if(pthread_create(&tid, NULL, start_server20, (void *)this)) qDebug() << "ошибка создания потока ожидания";


    channel.sin_port = htons(SERVER_PORT21);

    if((s21 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) qDebug() << "ошибка сокета";
    if(setsockopt(s21, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) qDebug() << "ошибка setsockopt REUSEADDR";
    if(setsockopt(s21, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) qDebug() << "ошибка setsockopt REUSEPORT";
    if((bind(s21, (struct sockaddr *) &channel, sizeof(channel))) < 0) qDebug() << "ошибка связывания";

    if(listen(s21, QUEUE_SIZE) < 0) qDebug() << "ошибка ожидания";

    if(pthread_create(&tid, NULL, start_server21, (void *)this)) qDebug() << "ошибка создания потока ожидания";
}

void* Server::start_server21(void *serv){
    pthread_t tid;
    Server *server = (Server *)serv;
    int sa;
    while(1){

        sa = accept(((Server*)serv)->s21, 0, 0);
        while(!semctl(server->sem, 0, GETVAL));

        semctl(server->sem, 0, SETVAL, 0);
        server->sa21[server->indexes.top()] = sa;
        if(server->sa21[server->indexes.top()] < 0) qDebug() << "ошибка доступа";
        send(server->sa21[server->indexes.top()], "220 OK\n", 7, 0);

        semctl(server->sem, 0, SETVAL, 1);

        if(pthread_create(&tid, NULL, new_connection21, serv)) qDebug() << "ошибка создания потока подключения";

    }
    exit(0);
}

void* Server::new_connection21(void *serv){

    int bytes;

    Server *server = (Server *)serv;
    User *user = new User(server->indexes.pop());
    user->set_current_dir(server->dir);

    int descriptor;
    while(1){

        while(!semctl(server->sem, 0, GETVAL));

        semctl(server->sem, 0, SETVAL, 0);

        descriptor = server->sa21[user->get_id()];
        bytes = 0;
        semctl(server->sem, 0, SETVAL, 1);

        for(int i = 0; i < BUFFER_SIZE; i++) server->buffer[user->get_id()][i] = '\0';

        while((bytes = recv(descriptor, server->buffer[user->get_id()], BUFFER_SIZE, 0)) <= 0) continue;


        qDebug() << descriptor << ": " << server->buffer[user->get_id()] << " bytes: " << bytes;
        if(!server->menu(user)) break;

    }
    qDebug() << "exit";
    close(server->sa21[user->get_id()]);
    server->indexes.push(user->get_id());
    return(0);
}

int Server::menu(User *user){
    QStringList list;
    QString answer;
//    pthread_mutex_lock(&mutex);
    int descriptor20, descriptor21 = sa21[user->get_id()];

    if(strstr(buffer[user->get_id()], "USER")){
        send(descriptor21, "230 Ok\n", 7, 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "SYST")) {
        send(descriptor21, "215 MACOS Ripper's Server\n", 26, 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "FEAT")) {
        answer = "211 Extensions supported:\n\tPWD\n\tTYPE\n\tPASV\n\tLIST\n\tMKD\n\tCWD\n\tCDUP\n\tRETR\n\tQUIT\n211 End\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "PWD")) {
        list = user->get_current_dir().split("/");
        answer = "257 /";
        if(list.length() == len_of_path) answer += "\n";
        else {
            for(int i = len_of_path; i < list.length(); i++)
                answer += list[i] + "/";
            if(answer[answer.length()-1] == '/') answer.remove(answer.length()-1, answer.length());
            answer += "\n";
        }
        qDebug() << answer;
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "TYPE")) {
        send(descriptor21, "230 Ok\n", 7, 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "PASV")) {
        answer = "227 " + ip + ",0,20\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "LIST")) {
        sa20 = accept(s20, 0, 0);
        if(sa20 < 0) qDebug() << "ошибка доступа";
        descriptor20 = sa20;

        struct stat statbuf;
        qDebug() << user->get_current_dir();
        user->get_all_files()->current_dir(user->get_current_dir().toUtf8().data());

        answer = "150 Accepted data connection\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);

        while(!semctl(sem, 0, GETVAL));

        semctl(sem, 0, SETVAL, 0);
        for(int i = 0; i < user->get_all_files()->get_num(); i++){
            stat(user->get_all_files()->get_all_files()[i].name.toUtf8().data(), &statbuf);

            for(int i = 0; i < BUFFER_SIZE; i++) buffer[user->get_id()][i] = '\0';

            strmode(statbuf.st_mode, buffer[user->get_id()]);
            answer = QString::fromUtf8(buffer[user->get_id()]);
            answer += " " + QString::number(statbuf.st_nlink);
            answer += " ripper";
            answer += " staff";
            answer += " " + QString::number(statbuf.st_size);
            list = QString::fromUtf8(asctime(localtime(&statbuf.st_ctime))).split(" ");
            if(answer[0] == 'd'){
                answer += " " + list[1] + " " + list[3];
                list[list.length()-1][list[list.length()-1].length()-1] = ' ';
                answer += " " + list[list.length()-1];
            }
            else{
                answer += " " + list[1] + " " + list[2];
                list[4][list[4].length()-1] = ' ';
                answer += " " + list[4];
            }

            list = user->get_all_files()->get_all_files()[i].name.split("/");
            answer += " " + list[list.length()-1] + "\n";
            send(descriptor20, answer.toUtf8().data(), answer.length(), 0);
//            qDebug() << answer;

        }
        answer = "226 " + QString::number(user->get_all_files()->get_num()) + " matches total\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        close(descriptor20);
        user->get_all_files()->set_empty();
        semctl(sem, 0, SETVAL, 1);
        return 1;
    }

    if(strstr(buffer[user->get_id()], "MKD")) {
        list = QString::fromUtf8(buffer[user->get_id()]).split(" ");
        QString path = user->get_current_dir() + "/" + list[1].remove("\r\n");
        QDir().mkdir(path);

        list = user->get_current_dir().split("/");
        answer = "257 /";
        for(int i = len_of_path; i < list.length(); i++) answer += list[i] + "/";
        answer += "\n";

        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        return 1;
    }

    if(strstr(buffer[user->get_id()], "CWD")) {
        while(!semctl(sem, 0, GETVAL));

        semctl(sem, 0, SETVAL, 0);

        list = QString::fromUtf8(buffer[user->get_id()]).split(" ");
        QString path_request = list[1];
        int k = path_request.indexOf("\r");
        path_request.truncate(k);
        path_request.remove("\r");
//        qDebug() << path_request;
        if(path_request == ".."){
            strcpy(buffer[user->get_id()], "CDUP");
            menu(user);
            semctl(sem, 0, SETVAL, 1);
            return 1;
        }
        else{
            if(QDir(user->get_current_dir() + "/" + path_request).exists()){
                QString path = "/";

                answer = "257 ";

                QList path_list = path_request.split("/");
                path += path_list[path_list.length()-1];
                answer += path;
                user->set_current_dir(user->get_current_dir() + path);
                answer += "\n";
            }
            else{
                answer = "550 Requested action not taken\n";
            }

        }
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        semctl(sem, 0, SETVAL, 1);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "CDUP")) {

        QStringList path = user->get_current_dir().split("/");
        QString newPath = "";

        for(int i = 0; i < path.length()-1; i++) newPath += path[i] + "/";
        qDebug() << newPath;
        if(newPath[newPath.length()-1] == '/')newPath.remove(newPath.length()-1, newPath.length());
        if(user->get_current_dir() == QDir::currentPath()+"/files") {
            answer = "550 Requested action not taken\n";
        }
        else{
//            qDebug() << newPath;
            user->set_current_dir(newPath);
            answer = "250 OK\n";
        }

        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "RETR")) {
        while(!semctl(sem, 0, GETVAL));

        semctl(sem, 0, SETVAL, 0);

        QString current_dir = QDir::currentPath();
        QString p;
        struct stat statbuf;

        FILE *fd;
        list = QString::fromUtf8(buffer[user->get_id()]).split(" ");
        QString path_request = list[1];
        int k = path_request.indexOf("\r");
        path_request.truncate(k);
        path_request.remove("\r");


        QString path = "";
        if(path_request[0] != '/') path_request = "/" + path_request;
        path += user->get_current_dir() + path_request;


        for(int i = 0; i < BUFFER_SIZE; i++) buffer[user->get_id()][i] = '\0';
        stat(path.toUtf8().data(), &statbuf);
        strmode(statbuf.st_mode, buffer[user->get_id()]);
        p = QString::fromUtf8(buffer[user->get_id()]);
        if(p[0] == 'd'){

            if(!QDir::setCurrent(user->get_current_dir())) qDebug() << "error";
            answer = "tar -cvf " + path_request.remove(0,1) + ".tar " + path_request;
            system(answer.toUtf8().data());
            fd = fopen((path + ".tar").toUtf8().data(), "rb");

        }
        else fd = fopen(path.toUtf8().data(), "rb");

        sa20 = accept(s20, 0, 0);
        if(sa20 < 0) qDebug() << "ошибка доступа";
        descriptor20 = sa20;

        if(fd == NULL){
             answer = "450 No such file or directory.\n";
        }
        else{

            answer = "150 Accepted data connection\n";
            send(descriptor21, answer.toUtf8().data(), answer.length(), 0);

            while(1){
                for(int i = 0; i < BUFFER_SIZE; i++) buffer[user->get_id()][i] = '\0';

                int bytes = fread(buffer[user->get_id()], 1, BUFFER_SIZE, fd);
                if(bytes <= 0) break;
                send(sa20, buffer[user->get_id()], bytes, 0);
            }

            if(p[0] == 'd'){
                system(("rm " + path + ".tar").toUtf8().data());
                QDir::setCurrent(current_dir);
            }
            fclose(fd);
            answer = "226 File transfer OK\n";

        }

        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        close(descriptor20);
        semctl(sem, 0, SETVAL, 1);
        return 1;
    }
    if(strstr(buffer[user->get_id()], "STOR")) {
        list = QString::fromUtf8(buffer[user->get_id()]).split(" ");
        QString request = list[1];
        int k = request.indexOf("\r");
        request.truncate(k);
        request.remove("\r");
        QStringList l = request.split("/");

        FILE *fd;
        fd = fopen((user->get_current_dir() + "/" + l[l.length()-1]).toUtf8().data(), "wb");

        sa20 = accept(s20, 0, 0);
        if(sa20 < 0) qDebug() << "ошибка доступа";
        descriptor20 = sa20;

        answer = "150 Accepted data connection\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);

        while(1){
            for(int i = 0; i < BUFFER_SIZE; i++) buffer[user->get_id()][i] = '\0';
            int bytes = recv(sa20, buffer[user->get_id()], BUFFER_SIZE, 0);
            if(bytes <= 0) break;
            fwrite(buffer, bytes, 1, fd);
        }

        answer = "226 File getting OK\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        close(descriptor20);

        return 1;
    }
    if(strstr(buffer[user->get_id()], "QUIT")) {
        answer = "221 OK\n";
        send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
        return -1;
    }
    answer = "502 Command not implemented.\n";
    send(descriptor21, answer.toUtf8().data(), answer.length(), 0);
    return 1;
}

void Server::set_ip(QString ip){
    this->ip = ip;
    this->ip.replace(".", ",");
}

void Server::set_length_of_path(int len){
    len_of_path = len;
}
