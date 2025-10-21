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
    Board* gameBoard;
    int rows, cols, mines;
    QWidget* centralWidget;
    QGridLayout* gridLayout;
    QVector<QVector<QPushButton*>>buttons;

public:
    GameWindow(int rows, int cols, int mines, QWidget* parent = nullptr): QMainWindow(parent), rows(rows), cols(cols), mines(mines) {
        
        setWindowTitle("Minesweeper");

        gameBoard = new Board(mines, cols);
        gameBoard->initializeForMines();
        gameBoard->placeMines();
        gameBoard->placeNumbers();
        
        // Create central widget and layout
        centralWidget = new QWidget(this);
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(1);
        
        // Create the grid of buttons
        for(int i=0; i < rows; ++i){
                QVector<QPushButton*> rowButtons;
            for(int j=0; j < cols; ++j){
                QPushButton* button = new QPushButton();
                button->setFixedSize(30, 30);

                int cellValue = gameBoard->retrieveValue(i, j);
                
                if(cellValue == -1){
                    button->setText("💣");
                    button->setStyleSheet("background-color: red;");
                }else if(cellValue == 0){
                    button->setText("");
                    button->setStyleSheet("background-color: white;");
                }else {
                    button->setText(QString::number(cellValue));
                    button->setStyleSheet("background-color: lightblue;");
                }
                
                gridLayout->addWidget(button, i, j);
                rowButtons.append(button);
            }
            buttons.append(rowButtons);
        }
        
        setCentralWidget(centralWidget);
        
        // Adjust window size based on grid
        int width = cols * 32 + 20;
        int height = rows * 32 + 50;
        setFixedSize(width, height);
    }

    ~GameWindow(){
        delete gameBoard;
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
            rows = cols = 9;
            mines = 10;
        }
        else if(choice.contains("Intermediate")){ 
            rows = cols = 16; 
            mines = 40;
        }
        else{ 
            rows = cols = 24; 
            mines = 99;
        }

        GameWindow *gameWindow = new GameWindow(rows, cols, mines);
        gameWindow->show();
        startWindow.close();
    });

    startWindow.show();
    return app.exec();
}   