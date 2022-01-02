#ifndef KCC_GUI_H
#define KCC_GUI_H

#include <SDL2/SDL.h>
#include <mysql++/mysql++.h>

#include "imgui.h"

bool gui_tick(SDL_Window*, ImGuiIO&, mysqlpp::Connection&);

#endif //KCC_GUI_H
