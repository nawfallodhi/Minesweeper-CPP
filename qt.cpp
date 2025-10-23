#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QWidget>
#include <QVector>
#include "minesweeper_header.h"

class GameWindow : public QMainWindow {
private:
    Board* realBoard;    // The actual board with mines
    Board* playerBoard;  // What the player sees (-2 = covered)
    int rows, cols, mines;
    QWidget* centralWidget;
    QGridLayout* gridLayout;
    QVector<QVector<QPushButton*>> buttons;
    bool firstMove;
    bool gameOver;

public:
    GameWindow(int rows, int cols, int mines, QWidget* parent = nullptr)
        : QMainWindow(parent), rows(rows), cols(cols), mines(mines), firstMove(true), gameOver(false) {
        
        setWindowTitle("Minesweeper");

        realBoard = new Board(mines, cols);
        realBoard->initializeForMines();
        
        realBoard->placeMines();
        realBoard->placeNumbers();

        playerBoard = new Board(mines, cols);  // This initializes with -2

        // Create central widget and layout
        centralWidget = new QWidget(this);
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(0);
        
        // Create the grid of buttons - all start covered
        for (int i = 0; i < rows; ++i) {
            QVector<QPushButton*> rowButtons;
            for(int j = 0; j < cols; ++j){
                QPushButton* button = new QPushButton();
                button->setFixedSize(30, 30);
                button->setProperty("row", i);
                button->setProperty("col", j);
                
                // All buttons start as covered (blank)
                button->setText("");
                button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; }");
                
                // Connect click handler
                connect(button, &QPushButton::clicked, this, &GameWindow::onCellClicked);
                
                gridLayout->addWidget(button, i, j);
                rowButtons.append(button);
            }
            buttons.append(rowButtons);
        }
        
        setCentralWidget(centralWidget);
        
        int width = cols * 32 + 20;
        int height = rows * 32 + 50;
        setFixedSize(width, height);
    }

    ~GameWindow(){
        delete realBoard;
        delete playerBoard;
    }

private slots:
    void onCellClicked() {
        if (gameOver) return;
        
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (button) {
            int row = button->property("row").toInt();
            int col = button->property("col").toInt();
            
            // Handle first move (safe start)
            if (firstMove) {
                playerBoard->starterTile(row, col, *realBoard);
                firstMove = false;
            }

            // Check if mine was hit
            if (realBoard->retrieveValue(row, col) == -1) {
                // Game over - reveal all mines
                revealAllMines();
                QMessageBox::information(this, "Game Over", "You hit a mine! Game over!");
                gameOver = true;
                return;
            }
            
            // Uncover this cell and potentially others
            playerBoard->uncoverTiles(row, col, *realBoard);
            updateBoardDisplay();
            
            // Check win condition
            if (checkWinCondition()) {
                QMessageBox::information(this, "Congratulations", "You cleared the board!");
                gameOver = true;
            }
        }
    }
    
    void updateBoardDisplay() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int playerValue = playerBoard->retrieveValue(i, j);
                QPushButton* button = buttons[i][j];
                
                if (playerValue == -2) {
                    // Cell is still covered
                    button->setText("");
                    button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; }");
                } else {
                    // Cell is uncovered - show real value
                    button->setEnabled(false);
                    int realValue = realBoard->retrieveValue(i, j);
                    
                    if (realValue == -1) {
                        button->setText("💣");
                        button->setStyleSheet("QPushButton { background-color: red; border: 1px solid black; }");
                    } else if (realValue == 0) {
                        button->setText("");
                        button->setStyleSheet("QPushButton { background-color: white; border: 1px solid darkgray; }");
                    } else {
                        button->setText(QString::number(realValue));
                        button->setStyleSheet("QPushButton { background-color: white; border: 1px solid darkgray; color: blue; }");
                    }
                }
            }
        }
    }
    
    void revealAllMines() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                    playerBoard->editValue(i, j, *realBoard);
            }
        }
        updateBoardDisplay();
    }
    
    bool checkWinCondition() {
        int totalSafeCells = rows * cols - mines;
        int uncoveredCells = 0;
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (playerBoard->retrieveValue(i, j) != -2) {
                    uncoveredCells++;
                }
            }
        }
        
        return uncoveredCells == totalSafeCells;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget startWindow;
    startWindow.setWindowTitle("Select Difficulty");
    startWindow.setFixedSize(300, 150);

    QVBoxLayout *layout = new QVBoxLayout(&startWindow);

    QComboBox *combo = new QComboBox();
    combo->addItem("Beginner (9x9)");
    combo->addItem("Intermediate (16x16)");
    combo->addItem("Advanced (24x24)");

    QPushButton *startButton = new QPushButton("Start Game");

    layout->addWidget(combo);
    layout->addWidget(startButton);
    startWindow.setLayout(layout);

    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        int rows, cols, mines;
        QString choice = combo->currentText();

        if(choice.contains("Beginner")){ 
            rows = 9;
            cols = 9;
            mines = 10;
        }
        else if(choice.contains("Intermediate")){ 
            rows = 16; 
            cols = 16;
            mines = 40;
        }
        else{ 
            rows = 24; 
            cols = 24;
            mines = 99;
        }

        GameWindow *gameWindow = new GameWindow(rows, cols, mines);
        gameWindow->show();
        startWindow.close();
    });

    startWindow.show();
    return app.exec();
}   