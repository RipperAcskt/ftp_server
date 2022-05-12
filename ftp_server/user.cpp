#include "user.h"

User::User(int id)
{
    all_files = new Dir();
    this->id = id;
}

QString User::get_current_dir(){
    return this->current_dir;
}

Dir* User::get_all_files(){
    return this->all_files;
}

int User::get_id(){
    return id;
}

void User::set_id(int id){
    this->id = id;
}

void User::set_current_dir(QString current_dir){
    this->current_dir = current_dir;
}
