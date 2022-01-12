#include <algorithm>
#include <cstdlib>
#include <map>
#include <vector>
#include <cstring>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "gui.h"
#include "raider.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static std::map<std::string, int> raids{
        {"Molten Core",    10 * 3 + 1},
        {"Blackwing Lair", 9 * 3 + 1},
};

std::vector<Raider> get_raiders(mysqlpp::Connection &db) {
    mysqlpp::Query query = db.query("select * from raiders");
    mysqlpp::StoreQueryResult res = query.store();
    std::vector<Raider> raiders;

    for (const auto &raider: res) {
        raiders.emplace_back(
                std::atoi(raider["Id"]),
                raider["Name"].c_str(),
                std::atoi(raider["Points"]),
                raider["Class"].c_str(),
                raider["Spec"].c_str()
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
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                ImGuiTableFlags_NoBordersInBody;
        if (ImGui::BeginTable("KCC Loot Table", 6, flags, ImVec2(0.0f, ImGui::CalcTextSize("A").x * 15), 0.0f)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, 1);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f,
                                    2);
            ImGui::TableSetupColumn("Class",
                                    ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed,
                                    0.0f, 3);
            ImGui::TableSetupColumn("Spec",
                                    ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed,
                                    0.0f, 4);
            ImGui::TableSetupColumn("Points",
                                    ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch,
                                    0.0f, 5);
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
                    ImGui::TextUnformatted(r->cls.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(r->spec.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", r->points);
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }
    }

    if (ImGui::Button("Add Raider")) {
        ImGui::OpenPopup("Create");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Create", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char name_buf[32], class_buf[32], spec_buf[32];
        ImGui::InputText("Name", name_buf, 32);
        ImGui::InputText("Class", class_buf, 32);
        ImGui::InputText("Spec", spec_buf, 32);

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            if (std::strlen(name_buf) == 0 || std::strlen(class_buf) == 0 || std::strlen(spec_buf) == 0) {
                // TODO: ERROR
            } else {
                char querystr[256];
                std::sprintf(querystr, "INSERT into raiders (name, points, Class, Spec) values ('%s', 0, '%s', '%s')", name_buf, class_buf, spec_buf);
                mysqlpp::Query query = db.query(querystr);
                query.exec();
                name_buf[0] = '\0';
                class_buf[0] = '\0';
                spec_buf[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Delete Raider")) {
        ImGui::OpenPopup("Delete");
    }
    ImGui::SameLine();
    ImGui::Text("Total Raiders: %lu", raiders.size());

    center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static size_t cmb_idx = 0;
        if (ImGui::BeginCombo("Raider", raiders[cmb_idx].name.c_str())) {
            for (size_t i = 0; i < raiders.size(); ++i) {
                const bool is_selected = (cmb_idx == i);
                if (ImGui::Selectable(raiders[i].name.c_str(), is_selected)) {
                    cmb_idx = i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            char querystr[128];
            std::sprintf(querystr, "DELETE FROM raiders WHERE name='%s'", raiders[cmb_idx].name.c_str());
            mysqlpp::Query query = db.query(querystr);
            query.exec();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    if (ImGui::Button("+1 All")) {
        for (auto &raider: raiders) {
            raider_inc(&raider, db, raidmaxpoints);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("-1 All")) {
        for (auto &raider: raiders) {
            raider_dec(&raider, db, raidmaxpoints);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Zero All")) {
        for (auto &raider: raiders) {
            raider_zero(&raider, db, raidmaxpoints);
        }
    }

    ImGui::End();

    ImGui::Render();
    glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    return true;
}
