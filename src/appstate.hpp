#pragma once
#include "board.hpp"
#include "solver.hpp"
#include <memory>
#include <thread>
#include <vector>

#define WIDTH 600
#define HEIGHT 800
#define PADDING 150
#define GAP 50

struct AppState {
    virtual ~AppState() = default;

    virtual std::unique_ptr<AppState> Run() { return nullptr; };
};

struct InitialState : public AppState {
    struct M {
        Board board;
        bool startPressed;
    } m;

    static std::unique_ptr<InitialState> Create();
    static std::unique_ptr<InitialState> Create(Board board);
    std::unique_ptr<AppState> Run() override;

  private:
    InitialState(M m) : m(std::move(m)) {};
};

struct LoadingState : public AppState {
    struct M {
        std::shared_ptr<Solver> solver;
        std::thread solverThread;
        Board finalBoard;
    } m;

    static std::unique_ptr<LoadingState> Create(Board finalBoard);
    std::unique_ptr<AppState> Run() override;
    ~LoadingState() override;

  private:
    LoadingState(M m) : m(std::move(m)) {};
};

struct ShowState : public AppState {
    struct M {
        std::vector<std::vector<Board>> solutions;
        float selected;
        float step;
        bool backPressed;
    } m;

    static std::unique_ptr<ShowState>
    Create(std::vector<std::vector<Board>> solutions);
    std::unique_ptr<AppState> Run() override;

  private:
    ShowState(M m) : m(std::move(m)) {};
};
