#include <iostream>
#include <cstdlib>

#include <mysql++/mysql++.h>
#include "gui.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>

SDL_Window* window = nullptr;
SDL_GLContext gl_context;
const char* glsl_version = "#version 130";

static int setup_SDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "Failed to init SDL2" << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    return 0;
}

static void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    int r = setup_SDL();
    if (r != 0) {
        return r;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (gui_tick(window, io)) {}

    /* TODO: Remove temp mysql++ test */
    char* dbpass = std::getenv("KCC_DB_PASS");
    if (dbpass == nullptr) {
        std::cout << "KCC_DB_PASS does not exist" << std::endl;
    } else {
        mysqlpp::Connection db;
        if (!db.connect("kcc", "fputs.com", "kcc", dbpass, 0)) {
            std::cout << "Failed to connect to database!" << std::endl;
            return 1;
        }

        mysqlpp::Query query = db.query("select * from raiders");
        mysqlpp::StoreQueryResult res = query.store();

        for (const auto &raider : res) {
            std::cout << raider["Name"] << "\t" << raider["Points"] << std::endl;
        }
    }

    cleanup();
    return 0;
}
