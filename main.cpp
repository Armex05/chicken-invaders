#include <QApplication>
#include "gamewidget.h"
//#include "GameWidgetlevel2.h"
//#include "GameWidgetLevel3.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GameWidget game;
    game.setWindowTitle("Chicken Invaders");
    game.resize(800, 600);
    game.show();
    return app.exec();
}