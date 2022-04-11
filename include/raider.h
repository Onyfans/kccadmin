#ifndef KCC_RAIDER_H
#define KCC_RAIDER_H

#include <string>
#include <mysql++/mysql++.h>

struct Raider {
    Raider(int, const char*, int, const char*, const char*);
    bool operator < (const Raider& o) const;

    int id;
    std::string name;
    int points;
    std::string cls;
    std::string spec;
};

void raider_inc(Raider*, mysqlpp::Connection&);
void raider_dec(Raider*, mysqlpp::Connection&);
void raider_zero(Raider*, mysqlpp::Connection&);

#endif //KCC_RAIDER_H
