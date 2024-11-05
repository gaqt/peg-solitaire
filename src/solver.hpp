#pragma once
#include "board.hpp"
#include <atomic>
#include <memory>
#include <unordered_set>
#include <vector>

struct Solver {
    struct M {
        std::unordered_set<Board, BoardHasher> middleStates;
        std::unordered_set<Board, BoardHasher> verified;
        std::vector<std::vector<Board>> solutions;
        Board initialBoard;
        Board finalBoard;
        uint8_t pathCnt;
        uint32_t maxSolutions;
        std::unique_ptr<std::atomic<bool>> finished;
    } m;

    static Solver *Create(Board initialBoard, Board finalBoard, uint32_t maxSolutions);
    void Solve(bool multithreaded);
    bool IsFinished();

  private:
    explicit Solver(M m) : m(std::move(m)) {}
    void FirstPass(Board board, std::vector<Board> &path);
    void SecondPass(Board board, std::vector<Board> &path);
};
