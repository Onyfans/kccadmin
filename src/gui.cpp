#include <algorithm>
#include <cstdlib>
#include <map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "gui.h"
#include "raider.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static std::map<std::string, int> raids {
        { "Molten Core", 10 * 3 + 1},
        { "Blackwing Lair", 9 * 3 + 1 },
};

std::vector<Raider> get_raiders(mysqlpp::Connection &db) {
    mysqlpp::Query query = db.query("select * from raiders");
    mysqlpp::StoreQueryResult res = query.store();
    std::vector<Raider> raiders;

    for (const auto &raider: res) {
        raiders.emplace_back(
                std::atoi(raider["Id"]),
                raider["Name"].c_str(),
                std::atoi(raider["Points"])
        );
    }
    return raiders;
}

bool gui_tick(SDL_Window *window, ImGuiIO &io, mysqlpp::Connection &db) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            return false;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(window)) {
            return false;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    bool demo = false;
    if (demo) {
        ImGui::ShowDemoWindow();
    } else {
        auto raiders = get_raiders(db);
        static ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        bool p_open;

        // TODO: Create Raid Chooser
        auto raidmaxpoints = raids["Molten Core"];

        // Loot Table
        if (ImGui::Begin("KCC Loot Admin", &p_open, flags)) {
            static ImGuiTableFlags flags =
                    ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                    ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
                    | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                    ImGuiTableFlags_NoBordersInBody
                    | ImGuiTableFlags_ScrollY;
            if (ImGui::BeginTable("KCC Loot Table", 4, flags, ImVec2(0.0f, ImGui::CalcTextSize("A").x * 15), 0.0f)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed,
                                        0.0f,
                                        0);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, 1);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f,
                                        2);
                ImGui::TableSetupColumn("Points",
                                        ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch,
                                        0.0f, 3);
                ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
                ImGui::TableHeadersRow();

                std::sort(raiders.begin(), raiders.end());

                // Demonstrate using clipper for large vertical lists
                ImGuiListClipper clipper;
                clipper.Begin(raiders.size());
                while (clipper.Step()) {
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++) {
                        // Display a data item
                        Raider *r = &raiders[row_n];
                        ImGui::PushID(r->id);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%04d", r->id);
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(r->name.c_str());
                        ImGui::TableNextColumn();
                        if (ImGui::SmallButton("Inc")) {
                            raider_inc(r, db, raidmaxpoints);
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton("Dec")) {
                            raider_dec(r, db, raidmaxpoints);
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton("Zero")) {
                            raider_zero(r, db, raidmaxpoints);
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", r->points);
                        ImGui::PopID();
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    return true;
}
