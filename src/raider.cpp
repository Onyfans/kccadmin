#include <algorithm>

#include "raider.h"

Raider::Raider(int id, const char *name, int points, const char *cls, const char *spec) :
        id(id), name(name), points(points), cls(cls), spec(spec) {}

bool Raider::operator<(const Raider &o) const {
    return name < o.name;
}

void raider_inc(Raider *r, mysqlpp::Connection &db) {
    char querystr[128];
    std::sprintf(querystr, "UPDATE raiders SET points = %d WHERE id = %d", r->points + 1, r->id);
    mysqlpp::Query query = db.query(querystr);
    query.exec();
}

void raider_dec(Raider *r, mysqlpp::Connection &db) {
    char querystr[128];
    std::sprintf(querystr, "UPDATE raiders SET points = %d WHERE id = %d", r->points - 1, r->id);
    mysqlpp::Query query = db.query(querystr);
    query.exec();
}

void raider_zero(Raider *r, mysqlpp::Connection &db) {
    char querystr[128];
    std::sprintf(querystr, "UPDATE raiders SET points = %d WHERE id = %d", 0, r->id);
    mysqlpp::Query query = db.query(querystr);
    query.exec();
}
