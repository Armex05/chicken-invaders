#ifndef GAMEWIDGETLEVEL2_H
#define GAMEWIDGETLEVEL2_H

#include "gamewidget.h"
#include <QList>
#include <QTimer>
#include <QPushButton>

class GameWidgetLevel2 : public GameWidget {
Q_OBJECT

public:
    GameWidgetLevel2(QWidget* parent = nullptr);

private slots:
    void gameLoop();
    void chickensShootEggs();
    void startMovingChickens();
    void nextSeason();
    void startSecondMovingChickens();
    void togglePauseContinue(); //pause option to manage the pause and continue...

private:
    void updateEggs();
    void updateMeats();
    void updateShipPosition();
    void checkCollisions();
    void animateChickens();
    void updateChickens();
    void decreaseChickenHealth(QGraphicsPixmapItem* chicken);

    QList<QGraphicsPixmapItem*> eggs;
    QList<QGraphicsPixmapItem*> meats;
    QTimer *eggShootTimer;
    QTimer * chickenMovementTimer;
    QTimer * secondChickenMovement;
    QTimer *animationTimer;
    int meatCount;
    bool wingUp;
    bool isPaused;
    QGraphicsTextItem *meatText;
    QPushButton *pauseContinueButton;

};

#endif