#pragma once

struct Position {
    int column;
    int row;

    bool operator==(const Position& other) {
        return column == other.column && row == other.row;
    }
    bool operator!=(const Position& other) {
        return column != other.column || row != other.row;
    }
};
