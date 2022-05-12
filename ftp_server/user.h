#ifndef USER_H
#define USER_H

#include "dir.h"
#include <QString>

class User
{
public:
    User(int id);
    QString get_current_dir();
    Dir* get_all_files();
    int get_id();

    void set_id(int id);
    void set_current_dir(QString current_dir);

private:
    QString current_dir;
    Dir* all_files;
    int id;
};

#endif // USER_H
