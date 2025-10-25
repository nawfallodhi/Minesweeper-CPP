#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QWidget>
#include <QListView>
#include <QVector>
#include "minesweeper_header.h"

class GameWindow : public QMainWindow {
private:
    Board* realBoard;    // The actual board with mines
    Board* playerBoard;  // What the player sees (-2 = covered)
    int rows, cols, mines;
    QWidget* centralWidget;
    QGridLayout* gridLayout;
    QLabel* mineCounter;
    QVector<QVector<QPushButton*>> buttons;
    QVector<QVector<bool>> flags;
    bool firstMove;
    bool gameOver;

public:
    GameWindow(int rows, int cols, int mines, QWidget* parent = nullptr)
        : QMainWindow(parent), rows(rows), cols(cols), mines(mines), firstMove(true), gameOver(false) {
        
        setWindowTitle("Minesweeper");

        flags.resize(rows);
        for(int i=0;i<rows;i++){
            flags[i].resize(cols, false);
        }
        
        realBoard = new Board(mines, cols);
        realBoard->initializeForMines();
        
        realBoard->placeMines();
        realBoard->placeNumbers();

        playerBoard = new Board(mines, cols);  

        // Create central widget and layout
        centralWidget = new QWidget(this);

        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(2);
        mainLayout->setContentsMargins(10,10,10,10);
        
        mineCounter = new QLabel(QString("Mines: %1").arg(mines));
        mainLayout->addWidget(mineCounter);
        
        QWidget* gridContainer = new QWidget();
        gridContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout = new QGridLayout(gridContainer);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0,0,0,0);

        mainLayout->addWidget(gridContainer);
        mainLayout->setAlignment(gridContainer, Qt::AlignCenter);
        
        // Create the grid of buttons - all start covered
        for (int i = 0; i < rows; ++i) {
            QVector<QPushButton*> rowButtons;
            for(int j = 0; j < cols; ++j){
                QPushButton* button = new QPushButton();
                //button->setFixedSize(30, 30);
                
                button->setMinimumSize(25, 25);  // Minimum size
                button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                
                button->setProperty("row", i);
                button->setProperty("col", j);
                
                // All buttons start as covered (blank)
                button->setText("");
                button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; margin: 0px; padding: 0px; }");
                
                // Connect click handler
                connect(button, &QPushButton::clicked, this, &GameWindow::onCellClicked);
                
                button->setContextMenuPolicy(Qt::CustomContextMenu);
                connect(button, &QPushButton::customContextMenuRequested,this, &GameWindow::onRightClick);

                gridLayout->addWidget(button, i, j);
                rowButtons.append(button);
            }
            buttons.append(rowButtons);
        }
        
        setCentralWidget(centralWidget);
        
        int width = cols * 30 + 50;
        int height = rows * 30 + 100;
        //setFixedSize(width, height);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(width, height);
    }

    ~GameWindow(){
        delete realBoard;
        delete playerBoard;
    }
protected:
    void resizeEvent(QResizeEvent* event) override {
        QMainWindow::resizeEvent(event);
        
        // Calculate new tile size based on window size
        int availableWidth = centralWidget->width() - 20;  // Account for margins
        int availableHeight = centralWidget->height() - 50; // Account for counter and margins
        
        int tileWidth = availableWidth / cols;
        int tileHeight = availableHeight / rows;
        int tileSize = qMin(tileWidth, tileHeight);
        
        // Limit tile size
        tileSize = qMax(25, qMin(50, tileSize));  // Between 25px and 50px
        
        // Apply to all buttons
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < cols; ++j){
                buttons[i][j]->setFixedSize(tileSize, tileSize);
            }
        }
    }
private slots:
    void onCellClicked() {
        if(gameOver) return;
        
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if(button) {
            int row = button->property("row").toInt();
            int col = button->property("col").toInt();
            
            if(firstMove) {
                playerBoard->starterTile(row, col, *realBoard);
                firstMove = false;
            }

            if(flags[row][col]){
                return;
            }

            if(realBoard->retrieveValue(row, col) == -1){
                revealAllMines();

                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Game Over", "You hit a mine! Game over!\nWould you like to restart?",QMessageBox::Yes | QMessageBox::No);
                if(reply == QMessageBox::Yes){
                    restartGame();
                } else{
                    gameOver = true;
                    return;
                }
            }
            
            // Uncover this cell and potentially others
            playerBoard->uncoverTiles(row, col, *realBoard);
            updateBoardDisplay();
            
            // Check win condition
            if(checkWinCondition()){
                
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Congradulations", "You cleared the board!\nWould you like to restart?",QMessageBox::Yes | QMessageBox::No);
                if(reply == QMessageBox::Yes){
                    restartGame();
                } else{
                    gameOver = true;
                    return;
                }
            }
        }
    }
    
    void updateBoardDisplay(){
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < cols; ++j){
                int playerValue = playerBoard->retrieveValue(i, j);
                QPushButton* button = buttons[i][j];
                
                if(flags[i][j] && playerValue == -2){
                    button->setText("🚩");
                    button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; color: red; }");
                    continue; 
                }
                
                if(playerValue == -2) {
                    // Cell is still covered
                    button->setText("");
                    button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; }");
                } else{
                    // Cell is uncovered - show real value
                    button->setEnabled(false);
                    int realValue = realBoard->retrieveValue(i, j);
                    
                    if(realValue == -1) {
                        button->setText("💣");
                        button->setStyleSheet("QPushButton { background-color: red; border: 1px solid black; }");
                    } else if(realValue == 0) {
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

    void updateMineCounter(){
        int count = 0;
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                if(flags[i][j]){
                    count++;
                }
            }
        }
        mineCounter->setText(QString("Mines: %1").arg(mines-count));
    }

    void onRightClick(){
        if(gameOver){
            return;
        }

        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if(button){
            int row = button->property("row").toInt();
            int col = button->property("col").toInt();

            if(row < 0 || row >= rows || col < 0 || col >= cols){
                return;
            } 
            
            if(playerBoard->retrieveValue(row,col) == -2){
                flags[row][col] = !flags[row][col];


                if(flags[row][col]){
                    button->setText("🚩");
                    button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; color: red; }");
                } else{
                    button->setText("");
                    button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; }");
                }


                updateMineCounter();
            }
        }
    }

    void restartGame(){
        // firstMove = true;
        // gameOver = false;

        // for(int i=0;i<rows;i++){
        //     for(int j=0;j<cols;j++){
        //         flags[i][j] = false;
        //     }
        // }

        // delete realBoard;
        // delete playerBoard;

        // realBoard = new Board(mines, cols);
        // realBoard->initializeForMines();
        // realBoard->placeMines();
        // realBoard->placeNumbers();

        // playerBoard = new Board(mines, cols);

        // for(int i=0;i<rows;i++){
        //     for(int j=0;j<cols;j++){
        //         QPushButton* button = buttons[i][j];
        //         button->setEnabled(true);
        //         button->setText("");
        //         button->setStyleSheet("QPushButton { background-color: lightgray; border: 1px solid darkgray; margin: 0px; padding: 0px; }");
        //     }
        // }

        // updateMineCounter();

            this->close();
    
    // Recreate and show the start window (copy from your main())
    QWidget* startWindow = new QWidget();
    startWindow->setWindowTitle("Select Difficulty");
    startWindow->setFixedSize(300, 150);

    QVBoxLayout* layout = new QVBoxLayout(startWindow);

    QComboBox* combo = new QComboBox();
    combo->addItem("Beginner (9x9)");
    combo->addItem("Intermediate (16x16)");
    combo->addItem("Advanced (24x24)");

    QPushButton* startButton = new QPushButton("Start Game");
    layout->addWidget(combo);
    layout->addWidget(startButton);
    startWindow->setLayout(layout);

    QObject::connect(startButton, &QPushButton::clicked, [=]() {
        int newRows, newCols, newMines;
        QString choice = combo->currentText();

        if(choice.contains("Beginner")){ 
            newRows = 9; newCols = 9; newMines = 10;
        }
        else if(choice.contains("Intermediate")){ 
            newRows = 16; newCols = 16; newMines = 40;
        }
        else{ 
            newRows = 24; newCols = 24; newMines = 99;
        }

        GameWindow* newGame = new GameWindow(newRows, newCols, newMines);
        newGame->show();
        startWindow->close();
    });

    startWindow->show();
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