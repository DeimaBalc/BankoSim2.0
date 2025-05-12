#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "admin.h"

#include <fstream>

Admin::Admin(const std::string& id, const std::string& slaptazodis) : id(id), slaptazodis(slaptazodis) {};

std::string Admin::getId() const {
    return id;
}

std::string Admin::getSlapt() const {
    return slaptazodis;
}

void Admin::setSlapt(const std::string& naujasSlaptazodis) {
    slaptazodis = naujasSlaptazodis;
}

