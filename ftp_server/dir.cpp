#include "dir.h"

Dir::Dir()
{
    num = 0;
}

void Dir::dirWalk(char* path){
    DIR *d;
    struct dirent *f;

    if((d = opendir(path))){
            while((f = readdir(d))){

                if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0 || strcmp(f->d_name, ".DS_Store") == 0 || strcmp(f->d_name, "") == 0)
                    continue;

                char *s = (char*)malloc((strlen(path)+2+strlen(f->d_name))*sizeof(char));
                strcpy(s, path);
                strcat(s, "/");
                strcat(s, f->d_name);
                strcat(s, "\0");

                switch(f->d_type){
                    case DT_DIR:
                        all_files.push_back(file{QString::fromUtf8(s), "dir"});
                        break;
                    case DT_REG:
                        all_files.push_back(file{QString::fromUtf8(s), "reg"});
                        break;
                    case DT_LNK:
                        all_files.push_back(file{QString::fromUtf8(s), "lnk"});
                        break;
                }
                num++;
                dirWalk(s);
            }
            closedir(d);
        }
    else return;
}

void Dir::current_dir(char* path){
    DIR *d;
    struct dirent *f;

    if((d = opendir(path))){
        while((f = readdir(d))){

            if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0 || strcmp(f->d_name, ".DS_Store") == 0 || strcmp(f->d_name, "") == 0)
                continue;

            char *s = (char*)malloc((strlen(path)+2+strlen(f->d_name))*sizeof(char));
            strcpy(s, path);
            strcat(s, "/");
            strcat(s, f->d_name);
            strcat(s, "\0");

            switch(f->d_type){
                case DT_DIR:
                    all_files.push_back(file{QString::fromUtf8(s), "dir"});
                    break;
                case DT_REG:
                    all_files.push_back(file{QString::fromUtf8(s), "reg"});
                    break;
                case DT_LNK:
                    all_files.push_back(file{QString::fromUtf8(s), "lnk"});
                    break;
            }
            num++;
        }
        closedir(d);
    }
    else return;
}

std::vector<struct file> Dir::get_all_files(){
    return all_files;
}

int Dir::get_num(){
    return num;
}

void Dir::set_empty(){
    all_files.resize(0);
    num = 0;
}
