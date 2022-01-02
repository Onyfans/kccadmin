cmake_minimum_required(VERSION 3.21)
project(kcc)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

set(SRC
    src/main.cpp
    src/gui.cpp
    src/raider.cpp

    src/imgui.cpp
    src/imgui_impl_opengl3.cpp
    src/imgui_impl_sdl.cpp
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp

    src/imgui_demo.cpp
)

add_executable(kcc ${SRC})
target_include_directories(kcc PRIVATE
    include
    /usr/include/mysql
    ${SDL2_INCLUDE_DIRS}
    ${SDL2main_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)
target_link_libraries(kcc PRIVATE
    mysqlpp
    mysqlclient
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)