#include "gamewidget.h"
#include "GameWidgetlevel2.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QFont>

GameWidget::GameWidget(QWidget *parent)
        : QGraphicsView(parent), score(0), lives(3), leftPressed(false), rightPressed(false), upPressed(false), downPressed(false), chickensMoving(false),isPaused(false) {
    setMouseTracking(true);

    scene = new QGraphicsScene(this);
    setScene(scene);
    setSceneRect(0, 0, 800, 600);
    scene->setBackgroundBrush(QBrush("#0D0944"));

    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800,600);  //setting fix size to the scene to not get bigger

    ship = new QGraphicsPixmapItem(QPixmap(":/images/ship.png"));
    ship->setScale(0.15);
    ship->setPos(375, 500);
    scene->addItem(ship); // adding the ship to the scene

    chickenMovementTimer = new QTimer(this);
    connect(chickenMovementTimer, &QTimer::timeout, this, &GameWidget::startMovingChickens);
    chickenMovementTimer->setSingleShot(true);
    chickenMovementTimer->start(4000); // 4 secs to start generating chickens
    // seconds delay
    secondChickenMovement = new QTimer(this);
    connect(secondChickenMovement,&QTimer::timeout, this, &GameWidget::startSecondMovingChickens);
    secondChickenMovement->setSingleShot(true);
    secondChickenMovement->start(14000);

    scoreText = new QGraphicsTextItem();
    scoreText->setPlainText(QString("Score: %1").arg(score));
    scoreText->setDefaultTextColor(Qt::green);
    scoreText->setFont(QFont("Arial", 16));
    scoreText->setPos(10, 10);
    scene->addItem(scoreText);  // adding the text for scores to the top left corner

    livesText = new QGraphicsTextItem();
    livesText->setPlainText(QString("Lives: %1").arg(lives));
    livesText->setDefaultTextColor(Qt::white);
    livesText->setFont(QFont("Arial", 16));
    livesText->setPos(10, 550);
    scene->addItem(livesText);  // adding the text for lives(hearts) to the top left corner

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    timer->start(16);

    nextSeasonTimer =  new QTimer(this);
    connect(nextSeasonTimer,&QTimer::timeout, this,&GameWidget::nextSeason);
    nextSeasonTimer->setSingleShot(true);
    nextSeasonTimer->start(28000);   // to go to the nest season(GameWidgetLevel2) after 28 secs of the game

    pauseContinueButton = new QPushButton("Pause", this); // setting up the pause button
    connect(pauseContinueButton,&QPushButton::clicked,this,&GameWidget::togglePauseContinue);
    pauseContinueButton->setGeometry(10,50,80,30);
}

GameWidget::~GameWidget() {
    delete timer;
    delete chickenMovementTimer;
    delete invincibilityTimer;
    delete scoreText;
    delete livesText;
    //free all the memories that referred to the pointers
}
void GameWidget::togglePauseContinue() {
    if (isPaused){
        chickenMovementTimer->start();
        secondChickenMovement->start();
        isPaused = false;
    }else{
        chickenMovementTimer->stop();
        secondChickenMovement->stop();
        isPaused = true;
    }
}

void GameWidget::nextSeason() { // hiding the scene of first season to show the scene for the second one
    this->hide();
    GameWidgetLevel2 *level2 = new GameWidgetLevel2;
    level2->show();
}


void GameWidget::keyPressEvent(QKeyEvent *event) {
    // the action of each key for keyboard

    switch (event->key()) {
        case Qt::Key_Left:
            leftPressed = true;
            break;
        case Qt::Key_Right:
            rightPressed = true;
            break;
        case Qt::Key_Up:
            upPressed = true;
            break;
        case Qt::Key_Down:
            downPressed = true;
            break;
        case Qt::Key_Space: {
            // generating the bullets by pressing space key
            QGraphicsPixmapItem *bullet = new QGraphicsPixmapItem(QPixmap(":/images/b.png")); // generating every new single bullet by pressing space button
            bullet->setScale(0.05);
            bullet->setPos(ship->x() + 30, ship->y());
            bullets.append(bullet);
            scene->addItem(bullet);
            break;
        }
        default:
            QGraphicsView::keyPressEvent(event);
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Left:
            leftPressed = false;
            break;
        case Qt::Key_Right:
            rightPressed = false;
            break;
        case Qt::Key_Up:
            upPressed = false;
            break;
        case Qt::Key_Down:
            downPressed = false;
            break;
        default:
            QGraphicsView::keyReleaseEvent(event);
    }
}

void GameWidget::mouseMoveEvent(QMouseEvent *event) {
    ship->setPos(event->x() - ship->boundingRect().width() / 18, event->y() - ship->boundingRect().height() / 18); // divided by 18 for better control to the ship by mouse cursor
}

void GameWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsPixmapItem *bullet = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
        bullet->setScale(0.05); // it will make the actual image smaller by ratio of 0.05
        bullet->setPos(ship->x() + ship->boundingRect().width() / 2 - bullet->boundingRect().width() / 2, ship->y());
        bullets.append(bullet);
        scene->addItem(bullet);
    }
}

void GameWidget::gameLoop() { // repeating the update of positions for every element after 16 m secs
    if (!isPaused){
        updateShipPosition();
        updateBullets();
        if (chickensMoving) { //checks if the chickens were generated and on the moving form to update them,
            updateChickens();
        }
        checkCollisions();
        scene->update();
    }
}

void GameWidget::startMovingChickens() { // the first group of chickens will start appearing after 4 secs by this slot
    if (isPaused) return;
    chickensMoving = true;

    for (int i = 0; i < 20; ++i) {//chickens entering the scene by -250 y coordinates on the top.
        int column = i % 5;
        int row = i / 5;
        QGraphicsPixmapItem *chicken = new QGraphicsPixmapItem(QPixmap(":/images/chicken.png"));
        chicken->setScale(0.1);
        chicken->setPos(column * 80 + 200, row * 80 - 250); //setting the positions base on the width and height of the scene
        chickens.append(chicken);
        scene->addItem(chicken);
    }
}
void GameWidget::startSecondMovingChickens() { // the second batch of the chickens will appear after 14 seconds of the game by this function
    if (isPaused) return;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 9; ++j) {
            QGraphicsPixmapItem *chicken = new QGraphicsPixmapItem(QPixmap(":/images/chicken.png"));
            chicken->setScale(0.1);
            chicken->setPos(j * 80 + 50, i * 80 - 250);
            chickens.append(chicken);
            scene->addItem(chicken);
        }
    }
}
void GameWidget::endInvincibility() {
    shipInvincible = false; // this is used due to  the problem that I had for collision of ship and chickens, with this we can control the ship to no lose all its lives after one collision
}

void GameWidget::updateShipPosition() { //after clicking every button this function will do its job by the call of gameLoop func
    if (isPaused) return;
    if (leftPressed) ship->setX(ship->x() - 5);
    if (rightPressed) ship->setX(ship->x() + 5);
    if (upPressed) ship->setY(ship->y() - 5);
    if(downPressed){
        if(ship->y() < scene->height() - 350){
            ship->setY(ship->y() + 2);
        }
    }
}

void GameWidget::updateBullets() { // like all other updates
    if (isPaused) return;
    for (QGraphicsPixmapItem *bullet : qAsConst(bullets)) {
        bullet->setY(bullet->y() - 2);
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [this](QGraphicsPixmapItem *bullet){ // this part will delete the bullet from its list of bullets and the scene
        if (bullet->y() < 0) { // method for freeing the memory that we gave to the bullet
            scene->removeItem(bullet);
            delete bullet;
            return true;
        }
        return false;
    }), bullets.end());
}

void GameWidget::updateChickens() {
    if (isPaused) return;
    for (int i = 0; i < chickens.size(); ++i){
        if(chickens[i]->y() < scene->height() * 0.6 - 150){ // by this, if the chickens get to 2/3 of the view, they will be deleted( the 150 is because the chickens were added from negative coordinates to the scene.
            chickens[i]->setY(chickens[i]->y() + 1);
        }
        else{
            scene->removeItem(chickens[i]);
            delete chickens[i];
            chickens.removeAt(i);
        }
    }
}

void GameWidget::checkCollisions() {
    for (int i = 0; i < bullets.size(); ++i) {
        for (int j = 0; j < chickens.size(); ++j) {
            if (bullets[i]->collidesWithItem(chickens[j])) { // the method for checking the collision of every single bullet and chicken inside the scene
                scene->removeItem(bullets[i]);
                scene->removeItem(chickens[j]);
                delete bullets[i];
                delete chickens[j];
                bullets.removeAt(i);
                chickens.removeAt(j);
                scene->removeItem(scoreText);
                delete scoreText;
                score += 5;
                scoreText = new QGraphicsTextItem();
                scoreText->setPlainText(QString("Score: %1").arg(score));
                scoreText->setDefaultTextColor(Qt::green);
                scoreText->setFont(QFont("Arial", 16));
                scoreText->setPos(10, 10);
                scene->addItem(scoreText);
                return;
            }
        }
    }
    if (!shipInvincible){
        for (QGraphicsPixmapItem *chicken : qAsConst(chickens)) { // controlling the ship for 2 seconds after colliding with every chicken to not lose all its lives
            if (chicken->collidesWithItem(ship)) {
                handleCollision();
                invincibilityTimer = new QTimer(this);
                invincibilityTimer->setSingleShot(true);
                connect(invincibilityTimer,&QTimer::timeout, this,&GameWidget::endInvincibility);
                invincibilityTimer->start(2000);
                return;
            }
        }
    }
}
void GameWidget::handleCollision() { // after the previous function, this slot will be called to get one of the lives
    lives -= 1;                      // and also updating the text score for the user
    scene->removeItem(livesText);
    delete livesText;
    livesText = new QGraphicsTextItem();
    livesText->setPlainText(QString("Lives: %1").arg(lives));
    livesText->setDefaultTextColor(Qt::white);
    livesText->setFont(QFont("Arial", 16));
    livesText->setPos(10, 550);
    scene->addItem(livesText);
    if(lives <= 0){
        //Game over
        delete livesText;
        livesText = new QGraphicsTextItem();
        livesText->setPlainText(QString("GAME OVER!!!"));
        livesText->setDefaultTextColor(Qt::red);
        livesText->setFont(QFont("Arial", 50));
        livesText->setPos(180, 200);
        scene->addItem(livesText);
        timer ->stop();
        return;
    }
    shipInvincible = true;
}

GameWidget::GameWidget(QWidget *parent, int n) // the second constructor was my solution for making the program object orienting to use as the parent constructor in the next seasons
    : QGraphicsView(parent), score(0), lives(3), leftPressed(false), rightPressed(false), upPressed(false), downPressed(false), chickensMoving(false) {
    // this constructor doesn't get the information that should be override in the next season
        setMouseTracking(true);

        scene = new QGraphicsScene(this);
        setScene(scene);
        setSceneRect(0, 0, 800, 600);
        scene->setBackgroundBrush(QBrush("#0D0944"));

        setScene(scene);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setFixedSize(800,600);

        ship = new QGraphicsPixmapItem(QPixmap(":/images/ship.png"));
        ship->setScale(0.15);
        ship->setPos(375, 500);
        scene->addItem(ship); // adding the ship to the scene

        scoreText = new QGraphicsTextItem();
        scoreText->setPlainText(QString("Score: %1").arg(score));
        scoreText->setDefaultTextColor(Qt::green);
        scoreText->setFont(QFont("Arial", 16));
        scoreText->setPos(10, 10);
        scene->addItem(scoreText);  // adding the text for scores to the top left corner

        livesText = new QGraphicsTextItem();
        livesText->setPlainText(QString("Lives: %1").arg(lives));
        livesText->setDefaultTextColor(Qt::white);
        livesText->setFont(QFont("Arial", 16));
        livesText->setPos(10, 550);
        scene->addItem(livesText);  // adding the text for lives(hearts) to the top left corner
}

