#include "appstate.hpp"
#include "../lib/raygui/raygui.h"
#include "../lib/raylib/src/raylib.h"
#include "board.hpp"
#include "solver.hpp"
#include <cstdio>
#include <memory>

using namespace std;

#pragma GCC diagnostic ignored "-Wc++20-extensions"

unique_ptr<InitialState> InitialState::Create() {
    return unique_ptr<InitialState>(
        new InitialState(M{.board = InitialBoard(),
                           .startPressed = false,
                           .resetPressed = false,
                           .solvedPressed = false}));
}

unique_ptr<InitialState> InitialState::Create(Board board) {
    return unique_ptr<InitialState>(
        new InitialState(M{.board = board, .startPressed = false}));
}

unique_ptr<AppState> InitialState::Run() {
    if (m.startPressed)
        return LoadingState::Create(m.board);

    if (m.resetPressed)
        m.board = InitialBoard();

    if (m.solvedPressed)
        m.board = FinalBoard();

    m.board.HandleClicks();

    BeginDrawing();
    ClearBackground(BLACK);
    m.board.Show();
    m.startPressed = GuiButton(
        {(float)WIDTH / 2 - 50, (float)HEIGHT - 100, 100, 40}, "Start");
    m.resetPressed = GuiButton(
        {(float)WIDTH / 2 - 250, (float)HEIGHT - 100, 100, 40}, "Reset");
    m.solvedPressed = GuiButton(
        {(float)WIDTH / 2 + 150, (float)HEIGHT - 100, 100, 40}, "Solved");
    EndDrawing();

    return nullptr;
}

unique_ptr<LoadingState> LoadingState::Create(Board finalBoard) {
    auto solver =
        shared_ptr<Solver>(Solver::Create(InitialBoard(), finalBoard, 100));
    thread solverThread = thread([=]() { solver->Solve(false); });

    return unique_ptr<LoadingState>(new LoadingState(M{
        .solver = solver,
        .solverThread = std::move(solverThread),
    }));
}

unique_ptr<AppState> LoadingState::Run() {
    if (m.solver->m.finished->load())
        return ShowState::Create(std::move(m.solver->m.solutions));

    char middleStatesBuf[80];
    sprintf(middleStatesBuf, "MiddleStates: %.2fK",
            m.solver->m.middleStates.size() / 1'000.0f);
    char solutionsBuf[60];
    sprintf(solutionsBuf, "Solutions: %ld", m.solver->m.solutions.size());
    char verifiedBuf[60];
    sprintf(verifiedBuf, "Verified: %.2fK",
            m.solver->m.verified.size() / 1'000.0f);

    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Loading", WIDTH / 2 - 90, HEIGHT / 2 - 10, 20, GREEN);
    DrawText(middleStatesBuf, WIDTH / 2 - 90, HEIGHT / 2 + 15, 20, GREEN);
    DrawText(solutionsBuf, WIDTH / 2 - 90, HEIGHT / 2 + 40, 20, GREEN);
    DrawText(verifiedBuf, WIDTH / 2 - 90, HEIGHT / 2 + 65, 20, GREEN);
    EndDrawing();

    return nullptr;
}

LoadingState::~LoadingState() {
    if (m.solverThread.joinable()) {
        m.solverThread.detach();
    }
}

unique_ptr<ShowState> ShowState::Create(vector<vector<Board>> solutions) {
    return unique_ptr<ShowState>(new ShowState(M{
        .solutions = std::move(solutions),
        .selected = 0,
        .step = 0,
        .backPressed = false,
    }));
}

unique_ptr<AppState> ShowState::Run() {
    if (m.backPressed) {
        if (m.solutions.size() == 0)
            return InitialState::Create();
        else
            return InitialState::Create(m.solutions[0].back());
    }

    if (m.solutions.size() == 0) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("No Solutions Found", WIDTH / 2 - 90, HEIGHT / 2 - 10, 20,
                 GREEN);
        m.backPressed = GuiButton({50, HEIGHT - 100, 100, 40}, "Back");
        EndDrawing();
        return nullptr;
    }

    char textSelected[80];
    sprintf(textSelected, "Selected: %d", (int)m.selected + 1);
    char textStep[80];
    sprintf(textStep, "Step: %d", (int)m.step + 1);
    char textSolutions[20];
    sprintf(textSolutions, "Solutions: %ld", m.solutions.size());
    char textMaxSelected[20];
    sprintf(textMaxSelected, "%ld", m.solutions.size());
    char textMaxStep[20];
    sprintf(textMaxStep, "%ld", m.solutions[(int)m.selected].size());

    BeginDrawing();
    ClearBackground(BLACK);
    m.solutions[(int)m.selected][(int)m.step].Show();
    DrawText(textSolutions, 50, HEIGHT - 240, 20, GREEN);
    DrawText(textSelected, 50, HEIGHT - 200, 20, GREEN);
    DrawText(textStep, 350, HEIGHT - 200, 20, GREEN);
    GuiSlider({50, HEIGHT - 170, 200, 20}, "1", textMaxSelected, &m.selected, 0,
              m.solutions.size() - 0.9f);
    GuiSlider({350, HEIGHT - 170, 200, 20}, "1", textMaxStep, &m.step, 0,
              m.solutions[(int)m.selected].size() - 0.9f);
    m.backPressed = GuiButton({50, HEIGHT - 100, 100, 40}, "Back");
    EndDrawing();

    return nullptr;
}
