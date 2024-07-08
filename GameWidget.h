#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

class GameWidget : public QGraphicsView {
Q_OBJECT




public:
    GameWidget(QWidget * parent = nullptr);
    GameWidget(QWidget *parent, int n);
    ~GameWidget();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

protected slots:
    void gameLoop();
    void startMovingChickens();
    void startSecondMovingChickens();
    void endInvincibility();
    void nextSeason();

protected:
    void updateShipPosition();
    void updateBullets();
    void updateChickens();
    void checkCollisions();
    void handleCollision();

    QGraphicsScene *scene;
    QGraphicsPixmapItem *ship;
    QGraphicsPixmapItem *bg;
    QList<QGraphicsPixmapItem *> bullets;
    QList<QGraphicsPixmapItem *> chickens;
    QTimer *timer;
    QTimer *nextSeasonTimer;
    QTimer * chickenMovementTimer;
    QTimer * secondChickenMovement;
    QTimer * invincibilityTimer;
    QGraphicsTextItem *scoreText;
    QGraphicsTextItem *livesText;

    int score;
    int lives;
    bool leftPressed, rightPressed, upPressed, downPressed;
    bool chickensMoving;
    bool shipInvincible;
};

#endif
