#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include "user.h"
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <QDir>
#include <QDebug>
#include <QStack>

#define SERVER_PORT20 20
#define SERVER_PORT21 21
#define QUEUE_SIZE 10
#define BUFFER_SIZE 4096


class Server
{
public:
    Server(QString path);
    void set_ip(QString ip);
    void set_length_of_path(int len);
private:

    int s20, s21, on = 1, num_of_users21 = 0;
    int sa20;
    int sa21[QUEUE_SIZE];
    char buffer[10][BUFFER_SIZE];
    int sem;
    int len_of_path;
    QStack<int> indexes;
    QString dir, ip;

    struct sockaddr_in channel;
    pthread_t tid;

    static void* start_server21(void *s);
    static void* new_connection21(void *s);
    int menu(User *user);
};

#endif // SERVER_H
