#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    QLabel *label = new QLabel("Hello Qt!");
    QPushButton button ("Hello World");
    button.show();
    window.setCentralWidget(label);
    window.show();
    
    return app.exec();
}
