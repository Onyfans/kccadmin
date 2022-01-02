#include "raider.h"

#include <utility>
#include <mysql++/mystring.h>

Raider::Raider(int id, const char* name, int points) :
    id(id), name(name), points(points) {}
