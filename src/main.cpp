#include "../lib/raylib/src/raylib.h"
#include "appstate.hpp"
#include "board.hpp"
#include <memory>
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/raygui.h"

using namespace std;

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Only One Left");
    SetTargetFPS(60);
    GuiLoadStyle("assets/style_terminal.rgs");

    // printf("%lb\n%lb\n", InitialBoard().bytes, FinalBoard().bytes);
    // printf("%d\n", InitialBoard().Diff(FinalBoard()));
    // printf("%d\n", InitialBoard().Move(3, 1, DOWN).Diff(InitialBoard()));
    // return 0;

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
