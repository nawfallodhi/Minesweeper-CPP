#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "minesweeper_header.h"

Board:: Board(){
    mines = 0;
    sides = 0;
    active_mines =0;
}

Board:: Board(int mines, int sides){
    this->mines = mines;
    this->sides = sides;
    this->active_mines =0;

    for(int i=0;i< sides;i++){
        this->board.push_back(std::vector<int>());
        for(int j=0;j< sides;j++){
            this->board[i].push_back(-2);
        }
    }
}

Board:: Board(const Board &b){
    mines = b.mines;
    sides = b.sides;
    active_mines =b.active_mines;
    board = b.board;
}

void Board::printBoard(){

    std::cout<<"    ";
    for(int j=0; j<sides; j++){
        std::cout<<std::setw(2)<<j<<" ";
    }
    std::cout<<"\n";

    std::cout<<"   ";
    for(int j=0; j<sides; j++){
        std::cout<<"---";
    }
    std::cout<<"\n";

    // Print each row with row index
    for(int i=0; i<sides; i++){
        std::cout<<std::setw(2)<<i<<"| ";
        for(int j=0; j<sides; j++){
            if(board[i][j] == -2){
                std::cout<<" . ";
            } else if(board[i][j] == -1){
                std::cout<<" * ";
            } else if(board[i][j] == 0){
                std::cout<<" 0 ";
            } else{
                std::cout<<" "<<board[i][j]<<" ";
            }
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}


void Board:: placeMines(){
    std::srand(std::time(0));
    
    while(active_mines != mines){

        int rows = std::rand() % (sides);
        int column = std::rand() % (sides);
        if(board.at(rows).at(column) == 0){
            board.at(rows).at(column) =-1;
            active_mines++;
        }
    } 

}

void Board:: setdifficuly(){
    int level;
    bool flag = true;

    std::cout<<"For Beginner (9x9, 10mines) input 0 !\n";
    std::cout<<"For Intermediate (16x16, 40minies) input 1 !\n";
    std::cout<<"For Advanced (24x24, 99 mines) input 2 !\n";

    do{
        std::cout<<"Input desired level now:\n";
        std::cin>>level;

        if(level == 0){
            mines =10;
            sides = 9;
            flag = 0;
        } else if(level == 1){
            mines =40;
            sides =16;
            flag = 0;
        } else if(level == 2){
            mines =99;
            sides = 24;
            flag = 0;
        } else{
            std::cout<<"Invalid choice, please try again\n";
        }
    }while(flag);

    active_mines =0;
    board.clear();
    board.resize(sides, std::vector<int>(sides,0));
}

void Board:: placeNumbers(){

    int counter= 0;

    for(int i=0;i<sides;i++){
        for(int j=0;j<sides;j++){
            if(board.at(i).at(j) == 0){

                if((j+1 < sides) && board.at(i).at(j+1) == -1){               //EAST
                    counter++;
                }
                if((j-1 >= 0) && board.at(i).at(j-1) == -1 ){                //WEST
                    counter++;
                }
                if((i+1 < sides) && board.at(i+1).at(j) == -1){        //SOUTH
                    counter++;
                }
                if((i-1 >= 0) && board.at(i-1).at(j) == -1){        //NORTH
                    counter++;
                }
                if((i-1 >= 0) && (j+1 < sides) && board.at(i-1).at(j+1) == -1){    //NORTH EAST
                    counter++;
                }
                if((i-1 >= 0) && (j-1 >= 0) && board.at(i-1).at(j-1) == -1){    //NORTH WEST
                    counter++;
                }
                if((i+1 < sides) && (j+1 < sides) && board.at(i+1).at(j+1) == -1){    //SOUTH EAST              
                    counter++;
                }
                if((i+1 < sides) && (j-1 >= 0) && board.at(i+1).at(j-1) == -1){    //SOUTH WEST
                    counter++;
                }

                board.at(i).at(j) = counter;
                counter =0;                
            }
        }
    }
}

void Board:: uncoverTiles(int x, int y, Board &real_board){
    if(x< 0 || x >= sides || y<0 || y>= sides){
        return;
    }

    if(board[x][y] == real_board.retrieveValue(x,y)){
        return;
    }

    editValue(x,y, real_board);
    setActiveMines();

    if(board[x][y] != 0){
        return;
    }

    uncoverTiles(x+1, y, real_board);
    uncoverTiles(x-1, y, real_board);
    uncoverTiles(x, y+1, real_board);
    uncoverTiles(x, y-1, real_board);
    uncoverTiles(x+1, y+1, real_board);
    uncoverTiles(x+1, y-1, real_board);
    uncoverTiles(x-1, y+1, real_board);
    uncoverTiles(x-1, y-1, real_board);
}

void Board:: starterTile(int x, int y, Board &real_board){
    if(real_board.board[x][y] == -1){
        real_board.board[x][y] = 0;
        real_board.active_mines--; 
    }


    // Optionally, ensure no mines around the starter tile
    for(int i = x-1; i <= x+1; i++){
        for(int j = y-1; j <= y+1; j++){
            if(i >=0 && i < sides && j >=0 && j < sides){
                if(real_board.board[i][j] == -1){
                    real_board.board[i][j] = 0;
                    real_board.active_mines--; // remove any nearby mines
                }
            }
        }
    }


    std::srand(std::time(0));
    
    while(real_board.active_mines<real_board.mines){
        int rows = std::rand() % (sides);
        int column = std::rand() % (sides);
        if(real_board.board[rows][column] == 0 && !(rows >= x-1 && rows <= x+1 && column >= y-1 && column <= y+1)){
            real_board.board[rows][column] = -1;
            real_board.active_mines++;
        }
    }
    real_board.placeNumbers();
}

void Board::initializeForMines() {
    board.clear();
    board.resize(sides, std::vector<int>(sides, 0));
    active_mines = 0;
}

bool Board:: playMove(int x, int y){

    if(board.at(x).at(y) == -1){
        return false;
    } else {
        return true;
    }
}

int Board:: getMines(){
    return mines;
}

int Board:: getSides(){
    return sides;
}

int Board:: getActiveMines(){
    return active_mines;
}

int Board:: retrieveValue(int x, int y){
    return board.at(x).at(y);
}

void Board:: setActiveMines(){
    active_mines++;
}

void Board:: editValue(int x, int y, Board &b){
    board.at(x).at(y) = b.retrieveValue(x,y);
}

void playMinesweeper(Board &real_board){
    std::cout<<"Welcome to Minesweeper!!!\n"<<std::endl;

    bool first_move = true;
    bool game_over = false;

    int x,y;
    
    real_board.setdifficuly();
    real_board.placeMines();
    real_board.placeNumbers();

    Board player_board(real_board.getMines(),real_board.getSides()); //user board
    player_board.printBoard();

    while(!game_over){

        std::cout<<"Please enter the coordinates of the cell to uncover"<<std::endl;
        std::cin>>x>>y;

        if(x<0 || x>= real_board.getSides() || y<0 || y>= real_board.getSides()){
            std::cout<<"Invalid Coordinates. Try again\n";
            continue;
        }
        
        if(first_move) {
            player_board.starterTile(x, y, real_board);
            first_move = false;
        }

        
        if(real_board.retrieveValue(x,y) == -1){
            std::cout << "You hit a mine! Game over!\n";
            game_over = true;
            //reveal all mines
            for(int i=0;i < real_board.getSides(); i++){
                for(int j = 0; j < real_board.getSides(); j++){
                    if(real_board.retrieveValue(i, j) == -1){
                        player_board.editValue(i, j, real_board);
                    }
                }
            }
            player_board.printBoard();
            break;
        }

        player_board.uncoverTiles(x,y,real_board);
        player_board.printBoard();


        int total_safe_cells = real_board.getSides() * real_board.getSides() - real_board.getMines();
        int uncovered_cells =0;
        for (int i=0;i<player_board.getSides();i++){
            for(int j=0;j<player_board.getSides();j++){
                if(player_board.retrieveValue(i,j) !=-2){
                    uncovered_cells++;
                }
            }
        }

        if(uncovered_cells == total_safe_cells) {
            std::cout << "Congratulations! You cleared the board!\n";
            game_over = true;
        }
        
    }

}


// int main(){

//     Board real_board;
//     playMinesweeper(real_board);
    
//     return 0;
// }