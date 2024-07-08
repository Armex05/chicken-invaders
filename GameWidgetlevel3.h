#ifndef AP_PROJECT_GAMEWIDGETLEVEL3_H
#define AP_PROJECT_GAMEWIDGETLEVEL3_H


#include <QPushButton>
#include "GameWidget.h"

class GameWidgetLevel3: public GameWidget {
    Q_OBJECT

public:
    GameWidgetLevel3(QWidget* parent = nullptr);

private slots:
            void gameLoop();
    void chickensShootEggs();
    void startMovingChickens();
    void startSecondMovingChickens();
    void startGiftMovement();
    void togglePauseContinue(); //pause option to manage the pause and continue...

private:
    void updateEggs();
    void updateMeats();
    void updateGift();
    void checkCollisions();
    void animateChickens();
    void updateChickens();
    void decreaseChickenHealth(QGraphicsPixmapItem* chicken);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    QList<QGraphicsPixmapItem*> eggs;
    QList<QGraphicsPixmapItem*> meats;
    QGraphicsPixmapItem* gift;
    QTimer *giftTimer;
    QTimer *eggShootTimer;
    QTimer * chickenMovementTimer;
    QTimer * secondChickenMovement;
    QTimer *animationTimer;
    int meatCount;
    bool wingUp;
    bool isPaused;
    bool isTowBullet;
    QGraphicsTextItem *meatText;
    QPushButton *pauseContinueButton;

};


#endif