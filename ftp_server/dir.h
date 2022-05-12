#ifndef DIR_H
#define DIR_H

#include <dirent.h>
#include <vector>
#include <QString>
#include <string.h>
#include <vector>

struct file{
    QString name;
    QString type;
};

class Dir
{
public:
    Dir();
    void dirWalk(char* path);
    void current_dir(char* path);
    int get_num();
    void set_empty();
    std::vector<struct file> get_all_files();

private:
    std::vector<struct file> all_files;
    int num;
//    std::vector<QString> all_files;
};

#endif // DIR_H
