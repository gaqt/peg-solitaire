#include "solver.hpp"
#include "board.hpp"
#include <cstdio>
#include <cstdlib>
#include <thread>

#pragma GCC diagnostic ignored "-Wc++20-extensions"

using namespace std;

void Solver::FirstPass(const Board board, vector<Board> &path) {

    if (!board.bytes || m.verified.count(board))
        return;

#ifdef DEBUG
    printf("1 pass: %ld\n", board.bytes);
#endif

    m.verified.insert(board);
    path.push_back(board);

    if (path.size() == 1u + m.pathCnt / 2u) {
        m.middleStates.insert(board);
        goto cleanup;
    }

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board.Get(x, y) != 1)
                continue;

            FirstPass(board.Move(x, y, UP), path);
            FirstPass(board.Move(x, y, RIGHT), path);
            FirstPass(board.Move(x, y, DOWN), path);
            FirstPass(board.Move(x, y, LEFT), path);
        }
    }

cleanup:
    path.pop_back();
}

void Solver::SecondPass(const Board board, vector<Board> &path) {

    if (!board.bytes || m.verified.count(board))
        return;

    if (m.solutions.size() >= m.maxSolutions)
        return;

#ifdef DEBUG
    printf("2 pass: %ld\n", board.bytes);
#endif

    m.verified.insert(board);
    path.push_back(board);

    if (path.size() == (m.pathCnt + 1u) / 2u) {
        if (m.middleStates.count(board) == 0)
            goto cleanup;

        auto halfSolver = Solver::Create(m.initialBoard, board,
                                         m.solutions.size() - m.maxSolutions);
        halfSolver->Solve(false);

        for (auto &v : halfSolver->m.solutions) {
            v.pop_back();
            for (auto p = path.rbegin(); p != path.rend(); p++) {
                v.push_back(*p);
            }

            m.solutions.push_back(v);
        }

        goto cleanup;
    }

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board.Get(x, y) != 1)
                continue;

            SecondPass(board.MoveRev(x, y, UP), path);
            SecondPass(board.MoveRev(x, y, RIGHT), path);
            SecondPass(board.MoveRev(x, y, DOWN), path);
            SecondPass(board.MoveRev(x, y, LEFT), path);
        }
    }

cleanup:
    path.pop_back();
}

Solver *Solver::Create(Board initialBoard, Board finalBoard,
                       uint32_t maxSolutions) {
    uint8_t pathCnt = 1u + __builtin_popcountll(initialBoard.bytes) -
                      __builtin_popcountll(finalBoard.bytes);
    return new Solver(M{
        .middleStates = {},
        .verified = {},
        .solutions = {},
        .initialBoard = initialBoard,
        .finalBoard = finalBoard,
        .pathCnt = pathCnt,
        .maxSolutions = maxSolutions,
        .finished = make_unique<atomic<bool>>(false),
    });
}

void Solver::Solve(bool multithreaded) {
    const int threadCnt = thread::hardware_concurrency();

#ifdef DEBUG
    printf("initial: %ld\n", m.initialBoard.bytes);
    printf("final: %ld\n", m.finalBoard.bytes);
    printf("pathCnt: %d\n", m.pathCnt);
#endif

    if (m.initialBoard.bytes == 0 || m.finalBoard.bytes == 0)
        goto cleanup;

    if (m.pathCnt == 1) {
        if (m.initialBoard == m.finalBoard)
            m.solutions.push_back({m.initialBoard});

        goto cleanup;
    }

    if (m.pathCnt == 2) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                if (m.initialBoard.Move(x, y, UP) == m.finalBoard ||
                    m.initialBoard.Move(x, y, RIGHT) == m.finalBoard ||
                    m.initialBoard.Move(x, y, DOWN) == m.finalBoard ||
                    m.initialBoard.Move(x, y, LEFT) == m.finalBoard) {

                    m.solutions.push_back({m.initialBoard, m.finalBoard});
                    goto cleanup;
                }
            }
        }

        goto cleanup;
    }

    m.middleStates.insert(m.initialBoard);

    if (!multithreaded || threadCnt <= 1) {
        vector<Board> path;
        path.reserve((m.pathCnt + 1) / 2);
        FirstPass(m.initialBoard, path);
        path.clear();
        m.verified.clear();
        SecondPass(m.finalBoard, path);
        goto cleanup;
    }

cleanup:
    if (m.solutions.size() > m.maxSolutions)
        m.solutions.resize(m.maxSolutions);
    m.finished->store(true);
}

