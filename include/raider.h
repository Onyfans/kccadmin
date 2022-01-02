#ifndef KCC_RAIDER_H
#define KCC_RAIDER_H

#include <string>

struct Raider {
    Raider(int, const char*, int);

    int id;
    std::string name;
    int points;
};

#endif //KCC_RAIDER_H
