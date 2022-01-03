#ifndef KCC_RAIDER_H
#define KCC_RAIDER_H

#include <string>
#include <mysql++/mysql++.h>

struct Raider {
    Raider(int, const char*, int);
    bool operator < (const Raider& o) const;

    int id;
    std::string name;
    int points;
    bool here;
};

void raider_inc(Raider*, mysqlpp::Connection&, int);
void raider_dec(Raider*, mysqlpp::Connection&, int);
void raider_zero(Raider*, mysqlpp::Connection&, int);

#endif //KCC_RAIDER_H
