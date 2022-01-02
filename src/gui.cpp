#include <vector>
#include <cstdlib>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "gui.h"
#include "raider.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

std::vector<Raider> get_raiders(mysqlpp::Connection& db) {
    mysqlpp::Query query = db.query("select * from raiders");
    mysqlpp::StoreQueryResult res = query.store();
    std::vector<Raider> raiders;

    for (const auto &raider : res) {
        raiders.emplace_back(
            std::atoi(raider["Id"]),
            raider["Name"].c_str(),
            std::atoi(raider["Points"])
        );
    }
    return raiders;
}

bool gui_tick(SDL_Window *window, ImGuiIO& io, mysqlpp::Connection& db) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            return false;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
            return false;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // TODO: literally everything
    get_raiders(db);
    ImGui::ShowDemoWindow();


    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    return true;
}
