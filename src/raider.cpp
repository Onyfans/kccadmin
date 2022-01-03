#include <algorithm>

#include "raider.h"

Raider::Raider(int id, const char* name, int points) :
    id(id), name(name), points(points) {}

bool Raider::operator<(const Raider &o) const {
    return name < o.name;
}

void raider_inc(Raider* r, mysqlpp::Connection& db, int max) {
    int p = std::min(r->points + 1, max);
    char querystr[128];
    std::sprintf(querystr, "UPDATE raiders SET points = %d WHERE id = %d", p, r->id);
    mysqlpp::Query query = db.query(querystr);
    query.exec();
}

void raider_dec(Raider* r, mysqlpp::Connection& db, int max) {
    int p = std::max(r->points - 1, 0);
    char querystr[128];
    std::sprintf(querystr, "UPDATE raiders SET points = %d WHERE id = %d", p, r->id);
    mysqlpp::Query query = db.query(querystr);
    query.exec();
}

void raider_zero(Raider* r, mysqlpp::Connection& db, int max) {
    char querystr[128];
    std::sprintf(querystr, "UPDATE raiders SET points = %d WHERE id = %d", 0, r->id);
    mysqlpp::Query query = db.query(querystr);
    query.exec();
}
