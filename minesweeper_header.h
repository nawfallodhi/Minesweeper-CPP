#ifndef BOARD_H
#define BOARD_H

#include <vector>

class Board{
    private:
        int mines;
        int sides;
        int active_mines;
        std::vector<std::vector<int>> board;

    public:
        Board();
        Board(int mines, int sides);
        Board(const Board &b);
        void setdifficuly(); 
        void printBoard();
        void placeMines();
        void placeNumbers();

        bool playMove(int x, int y);

        int getMines();
        int getSides();
        int getActiveMines();

        void setActiveMines();

        int retrieveValue(int x, int y);
        void editValue(int x, int y, Board &b);

        void uncoverTiles(int x, int y, Board &real_board);

        void starterTile(int x, int y, Board &real_board);

        void initializeForMines();
};

#endif