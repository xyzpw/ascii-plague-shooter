#pragma once

struct Position {
    int column;
    int row;

    bool operator==(const Position& other) const {
        return column == other.column && row == other.row;
    }
    bool operator!=(const Position& other) const {
        return column != other.column || row != other.row;
    }
    bool operator<(const Position& other) const {
        if (column != other.column){
            return column < other.column;
        }
        return row < other.row;
    }
};
