#include "../lib/raylib/src/raylib.h"
#include "appstate.hpp"
#include <memory>
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/raygui.h"

using namespace std;

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Only One Left");
    SetTargetFPS(60);
    GuiLoadStyle("assets/style_terminal.rgs");

    unique_ptr<AppState> state = InitialState::Create();

    while (!WindowShouldClose()) {
        auto newState = state->Run();

        if (newState) {
            state = std::move(newState);
        }
    }

    CloseWindow();

    return 0;
}
