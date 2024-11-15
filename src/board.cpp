#include "board.hpp"
#include "appstate.hpp"
#include <cstdint>
#include <cstdio>
#include <functional>
#include <raylib.h>

using namespace std;

const uint8_t posTable[BOARD_SIZE + 4][BOARD_SIZE + 4] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0},
    {0, 0, 0, 0, 4, 5, 6, 0, 0, 0, 0},
    {0, 0, 7, 8, 9, 10, 11, 12, 13, 0, 0},
    {0, 0, 14, 15, 16, 17, 18, 19, 20, 0, 0},
    {0, 0, 21, 22, 23, 24, 25, 26, 27, 0, 0},
    {0, 0, 0, 0, 28, 29, 30, 0, 0, 0, 0},
    {0, 0, 0, 0, 31, 32, 33, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

uint8_t Board::Get(const int x, const int y) const {
    uint8_t i = posTable[y + 2][x + 2];
    return (!!i) * ((bytes >> i) & 1) + (!i) * 9;
}

void Board::Set(const int x, const int y) {
    uint8_t i = posTable[y + 2][x + 2];
    if (i == 0 || i > 45)
        return;

    bytes |= (1ull << i);
}

void Board::Clear(const int x, const int y) {
    uint8_t i = posTable[y + 2][x + 2];
    if (i == 0 || i > 45)
        return;

    bytes &= ~(1ull << i);
}

void Board::Flip(const int x, const int y) {
    uint8_t i = posTable[y + 2][x + 2];
    if (i == 0 || i > 45)
        return;

    bytes ^= (1ull << i);
}

Board Board::Move(const int x, const int y, const MoveDirection dir) const {
    int dx = 0, dy = 0;
    switch (dir) {
    case UP:
        dy = -1;
        break;
    case RIGHT:
        dx = 1;
        break;
    case DOWN:
        dy = 1;
        break;
    case LEFT:
        dx = -1;
        break;
    }

    uint8_t i0 = posTable[y + 2][x + 2];
    uint8_t i1 = posTable[y + 2 + dy][x + 2 + dx];
    uint8_t i2 = posTable[y + 2 + 2 * dy][x + 2 + 2 * dx];

    if (!i0 || !i1 || !i2)
        return {0};

    uint8_t b0 = (bytes >> i0) & 1;
    uint8_t b1 = (bytes >> i1) & 1;
    uint8_t b2 = (bytes >> i2) & 1;

    if (!b0 || !b1 || b2)
        return {0};

    Board cloned = *this;
    cloned.bytes &= ~(1ull << i0);
    cloned.bytes &= ~(1ull << i1);
    cloned.bytes |= (1ull << i2);

    return cloned;
}

Board Board::MoveRev(int x, int y, MoveDirection dir) const {
    int dx = 0, dy = 0;
    switch (dir) {
    case UP:
        dy = -1;
        break;
    case RIGHT:
        dx = 1;
        break;
    case DOWN:
        dy = 1;
        break;
    case LEFT:
        dx = -1;
        break;
    }

    uint8_t i0 = posTable[y + 2][x + 2];
    uint8_t i1 = posTable[y + 2 + dy][x + 2 + dx];
    uint8_t i2 = posTable[y + 2 + 2 * dy][x + 2 + 2 * dx];

    if (!i0 || !i1 || !i2)
        return {0};

    uint8_t b0 = (bytes >> i0) & 1;
    uint8_t b1 = (bytes >> i1) & 1;
    uint8_t b2 = (bytes >> i2) & 1;

    if (!b0 || b1 || b2)
        return {0};

    Board cloned = *this;
    cloned.bytes &= ~(1ull << i0);
    cloned.bytes |= (1ull << i1);
    cloned.bytes |= (1ull << i2);

    return cloned;
}

constexpr inline Vector2 CirclePos(float x, float y) {
    return {PADDING + GAP * x, PADDING + GAP * y};
}

#define CELL_RADIUS 20

void Board::Show() const {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Get(x, y) == 9) {
#ifdef DEBUG
                DrawText("9", CirclePos(x, y).x, CirclePos(x, y).y, 20, GREEN);
#endif
                continue;
            }

            if (Get(x, y)) {
                DrawCircleV(CirclePos(x, y), CELL_RADIUS, GREEN);
            } else {
                DrawCircleLinesV(CirclePos(x, y), CELL_RADIUS, GREEN);
            }

#ifdef DEBUG
            if (Get(x, y)) {
                DrawText("1", CirclePos(x, y).x, CirclePos(x, y).y, 20, BLACK);
            } else {
                DrawText("0", CirclePos(x, y).x, CirclePos(x, y).y, 20, GREEN);
            }
#endif
        }
    }
}

void Board::HandleClicks() {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Get(x, y) == 9)
                continue;

            Vector2 mousePos = GetMousePosition();
            Vector2 cellPos = CirclePos(x, y);

            if (CheckCollisionPointCircle(mousePos, cellPos, CELL_RADIUS)) {
                Flip(x, y);
                return;
            }
        }
    }
}

bool Board::operator==(const Board &other) const {
    return this->bytes == other.bytes;
}

Board InitialBoard() { return {(~((1ull) << 17)) & ((1ull << 34) - 1ull)}; }

Board FinalBoard() { return {((1ull) << 17) ^ 1ull}; }

int8_t Board::Diff(const Board other) {
    return __builtin_popcountll(this->bytes) -
           __builtin_popcountll(other.bytes);
}

size_t BoardHasher::operator()(const Board board) const {
    return hash<uint64_t>()(board.bytes);
}

bool BoardCompare::operator()(const Board a, const Board b) const {
    return a.bytes < b.bytes;
}
