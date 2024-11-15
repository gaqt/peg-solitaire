#pragma once
#include <cstdint>

#define BOARD_SIZE 7

enum MoveDirection : char {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT,
};

struct Board {
    uint64_t bytes;

    uint8_t Get(const int x, const int y) const;
    void Set(const int x, const int y);
    void Clear(const int x, const int y);
    void Flip(const int x, const int y);
    Board Move(const int x, const int y, const MoveDirection dir) const;
    Board MoveRev(const int x, const int y, const MoveDirection dir) const;
    void Show() const;
    void HandleClicks();
    int8_t Diff(const Board other);

    bool operator==(const Board &other) const;
};

struct BoardHasher {
    std::size_t operator()(const Board board) const;
};

struct BoardCompare {
    bool operator()(const Board a, const Board b) const;
};

Board InitialBoard();
Board FinalBoard();
