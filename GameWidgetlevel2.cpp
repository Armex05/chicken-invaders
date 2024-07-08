#include "gamewidgetlevel2.h"
#include "GameWidgetLevel3.h"
#include <QGraphicsTextItem>
#include <QRandomGenerator>

GameWidgetLevel2::GameWidgetLevel2(QWidget *parent): GameWidget(parent,1), meatCount(0), wingUp(true),isPaused(false) {
    eggShootTimer = new QTimer(this);
    connect(eggShootTimer, &QTimer::timeout, this, &GameWidgetLevel2::chickensShootEggs);
    eggShootTimer->start(5000); // 5 seconds interval for each egg to generate

    chickenMovementTimer = new QTimer(this);
    connect(chickenMovementTimer, &QTimer::timeout, this, &GameWidgetLevel2::startMovingChickens);
    chickenMovementTimer->setSingleShot(true);
    chickenMovementTimer->start(4000); // 4 secs to start generating the first batch of chickens

    secondChickenMovement = new QTimer(this);
    connect(secondChickenMovement,&QTimer::timeout, this, &GameWidgetLevel2::startSecondMovingChickens);
    secondChickenMovement->setSingleShot(true);
    secondChickenMovement->start(18000);// 18 secs to start generating the second batch of chickens


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWidgetLevel2::gameLoop);
    timer->start(16);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &GameWidgetLevel2::animateChickens);
    animationTimer->start(500); // 500 milliseconds interval for animation

    meatText = new QGraphicsTextItem();
    meatText->setPlainText(QString("meats: %1").arg(meatCount));
    meatText->setDefaultTextColor(Qt::red);
    meatText->setFont(QFont("Arial", 16));
    meatText->setPos(10, 450);
    scene->addItem(meatText);

    pauseContinueButton = new QPushButton("Pause", this);
    connect(pauseContinueButton,&QPushButton::clicked,this,&GameWidgetLevel2::togglePauseContinue);
    pauseContinueButton->setGeometry(10,50,80,30);

    nextSeasonTimer =  new QTimer(this);
    connect(nextSeasonTimer,&QTimer::timeout, this,&GameWidgetLevel2::nextSeason);
    nextSeasonTimer->setSingleShot(true);
    nextSeasonTimer->start(38000);
}
void GameWidgetLevel2::togglePauseContinue() {
    if (isPaused){
        eggShootTimer->start();
        chickenMovementTimer->start();
        isPaused = false;
    }else{
        eggShootTimer->stop();
        chickenMovementTimer->stop();
        isPaused = true;
    }
}
void GameWidgetLevel2::nextSeason(){
    this->hide();
    GameWidgetLevel3 *level3 = new GameWidgetLevel3;
    level3->show();
}

void GameWidgetLevel2::gameLoop() {
    if (!isPaused){
    updateShipPosition();
    updateBullets();
    if (chickensMoving) { //checks if the chickens were generated and on the moving form to update them,
        updateChickens();
    }
    checkCollisions();
    scene->update();
    updateEggs();
    updateMeats();
}
}

void GameWidgetLevel2::chickensShootEggs() {
    int numEggsToShoot = chickens.size() / 8; //selecting 1/8th of the chickens to make them throw eggs
    for (int i = 0; i < numEggsToShoot; ++i) {
        int index = QRandomGenerator::global()->bounded(chickens.size());
        QGraphicsPixmapItem *chicken = chickens.at(index);
        QGraphicsPixmapItem *egg = new QGraphicsPixmapItem(QPixmap(":/images/egg.png"));
        egg->setScale(0.015);
        egg->setPos(chicken->x() + 50, chicken->y());
        eggs.append(egg);
        scene->addItem(egg);
    }
}

void GameWidgetLevel2::startMovingChickens(){
    if (isPaused) return;
    chickensMoving = true;

    int columns = 8;
    int numChickens = columns* 3; // 3 rows, each with 8 columns

    for (int i = 0; i < numChickens; ++i) {
        int column = i % columns;
        int row = i / columns;
        QGraphicsPixmapItem *chicken;

        if (column % 2 == 0) {
            // Every other column is a chicken
            chicken = new QGraphicsPixmapItem(QPixmap(":/images/hen.png"));
            chicken->setScale(0.1);
            chicken->setData(0,2); // Chickens have 2 health(it is used here to help us find the wanted chicken in the future
            chicken->setData(1,"hen");
        } else {
            // Every other column is a chick
            chicken = new QGraphicsPixmapItem(QPixmap(":/images/chicken.jpg"));
            chicken->setScale(0.1);
            chicken->setData(0,1); // Chicks have 1 health
            chicken->setData(1,"chicken");
        }

        chicken->setPos(column * 80 + 100, row * 80 - 250);
        chickens.append(chicken);
        scene->addItem(chicken);
    }
}
void GameWidgetLevel2::startSecondMovingChickens() {
    if (isPaused) return;
    int columns = 10;
    int numChickens = columns* 3; // 3 rows, each with 8 columns

    for (int i = 0; i < numChickens; ++i) {
        int column = i % columns;
        int row = i / columns;
        QGraphicsPixmapItem *chicken;

        if (column % 4 == 0 ||column % 4 == 1) {
            // Every other column is a chicken
            chicken = new QGraphicsPixmapItem(QPixmap(":/images/hen.png"));
            chicken->setScale(0.1);
            chicken->setData(0,2); // Chickens have 2 health
            chicken->setData(1,"hen");
        } else {
            // Every other column is a chick
            chicken = new QGraphicsPixmapItem(QPixmap(":/images/chicken.png"));
            chicken->setScale(0.1);
            chicken->setData(0,1); // Chicks have 1 health
            chicken->setData(1,"chicken");
        }

        chicken->setPos(column * 80 + 15, row * 80 - 250);
        chickens.append(chicken);
        scene->addItem(chicken);
    }
}
void GameWidgetLevel2::updateShipPosition(){
    if (leftPressed) ship->setX(ship->x() - 5);
    if (rightPressed) ship->setX(ship->x() + 5);
    if (upPressed) ship->setY(ship->y() - 5);
    if(downPressed){
        if(ship->y() < scene->height() - 400){
            ship->setY(ship->y() + 2); // the speed of ship after every click by gameloop
        }
    }
}
void GameWidgetLevel2::updateEggs() {
    for (QGraphicsPixmapItem *egg : qAsConst(eggs)) {
            if(egg->y() > GameWidgetLevel2::scene->height() * 0.6 - 10){
                    delete egg;
                    eggs.removeOne(egg);
            }else{
                egg->setY(egg->y() + 0.4); // changing the position of each egg by 0.4 px after every gameloop
            }
    }
}
void GameWidgetLevel2::updateChickens(){
    for (int i = 0; i < chickens.size(); ++i){
        if(chickens[i]->y() > GameWidgetLevel2::scene->height() * 0.6 - 170){
            delete chickens[i];
            chickens.removeAt(i);
        }
        else{
            chickens[i]->setY(chickens[i]->y() + 0.7);
        }
    }
}

void GameWidgetLevel2::updateMeats() {
    for (QGraphicsPixmapItem *meat : qAsConst(meats)) {
        meat->setY(meat->y() + 2);
        if (meat->y() > GameWidgetLevel2::scene->height() * 0.6 - 10) {
            scene->removeItem(meat);
            meats.removeOne(meat);
            delete meat;
        }
    }
}

void GameWidgetLevel2::checkCollisions() {


    for (QGraphicsPixmapItem *egg : qAsConst(eggs)) {
        if (ship->collidesWithItem(egg)) {
        //    if the ship collides with egg, with this the egg will be erased and one of ships lives will decrease
            eggs.removeOne(egg);
            delete egg;
            handleCollision();
            return;
        }
    }

    for (QGraphicsPixmapItem *meat : qAsConst(meats)) {
        if (ship->collidesWithItem(meat)) {
        //    scene->removeItem(meat);
            meats.removeOne(meat);
            delete meat;
            meatCount++;
            delete meatText;
            meatText = new QGraphicsTextItem();
            meatText->setPlainText(QString("meats: %1").arg(meatCount));
            meatText->setDefaultTextColor(Qt::red);
            meatText->setFont(QFont("Arial", 16));
            meatText->setPos(10, 450);
            scene->addItem(meatText);
            if (meatCount == 30) { //if the counter of meats reach to 30, you'll get 50 extra points
                score += 50;
                meatCount = 0;
                return;
            }
        }
    }

    for (int i = 0; i < bullets.size(); ++i) {
        for (int j = 0; j < eggs.size(); ++j) {
            if (bullets[i]->collidesWithItem(eggs[j])) {
                //the bullets will erase the eggs by colliding with this func
                delete bullets[i];
                delete eggs[j];
                bullets.removeAt(i);
                eggs.removeAt(j);
                delete scoreText;
                score += 5; //updating the scene score after destroying every egg
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

    for (QGraphicsPixmapItem *chicken : qAsConst(chickens)) {
        if (ship->collidesWithItem(chicken)) {
        //    scene->removeItem(chicken);
            chickens.removeOne(chicken);
            delete chicken;
            handleCollision();
            invincibilityTimer = new QTimer(this);
            invincibilityTimer->setSingleShot(true);
            connect(invincibilityTimer, &QTimer::timeout, this, &GameWidgetLevel2::endInvincibility);
            invincibilityTimer->start(2000);
            return;
        }
    }

    for (QGraphicsPixmapItem *chicken : qAsConst(chickens)) {
        if (ship->collidesWithItem(chicken)) {
            chickens.removeOne(chicken);
            delete chicken;
            handleCollision();
            return;
        } else {
            QList<QGraphicsItem *> collidingItems = chicken->collidingItems();
            for (QGraphicsItem *item : collidingItems) {
                if (QGraphicsPixmapItem *bullet = dynamic_cast<QGraphicsPixmapItem*>(item)) {
                    if (bullet->pixmap().cacheKey() == QPixmap(":/images/b.png").cacheKey()) {
                        decreaseChickenHealth(chicken);
                        bullets.removeOne(bullet);
                        delete bullet;
                    }
                }
            }
        }
    }
}

void GameWidgetLevel2::decreaseChickenHealth(QGraphicsPixmapItem* chicken) {//because in this season some chickens will die with two bullets,
    // we defined health for every chicken, first we decrease the health and then check to make them die or not
    int health = chicken->data(0).toInt();
    health--;
    if (health <= 0) {
        QString type = chicken->data(1).toString();
        if (type == "hen") {
        //    scene->removeItem(scoreText);
            delete scoreText;
            score += 10;
            scoreText = new QGraphicsTextItem();
            scoreText->setPlainText(QString("Score: %1").arg(score));
            scoreText->setDefaultTextColor(Qt::green);
            scoreText->setFont(QFont("Arial", 16));
            scoreText->setPos(10, 10);
            scene->addItem(scoreText);
            QGraphicsPixmapItem *meat = new QGraphicsPixmapItem(QPixmap(":/images/meat.png"));
            meat->setScale(0.05);
            meat->setPos(chicken->x(), chicken->y());
            meats.append(meat);
            scene->addItem(meat);
        } else {
        //    scene->removeItem(scoreText);
            delete scoreText;
            score += 5;
            scoreText = new QGraphicsTextItem();
            scoreText->setPlainText(QString("Score: %1").arg(score));
            scoreText->setDefaultTextColor(Qt::green);
            scoreText->setFont(QFont("Arial", 16));
            scoreText->setPos(10, 10);
            scene->addItem(scoreText);
        }
        chickens.removeOne(chicken);
        delete chicken;
    } else {
        chicken->setData(0, health); // Update the health data.
        return;
    }
}

void GameWidgetLevel2::animateChickens() {
    for (QGraphicsPixmapItem *chicken : qAsConst(chickens)) {
        QString type = chicken->data(1).toString();
        if (type == "chicken") {
            if (wingUp) {
                chicken->setPixmap(QPixmap(":/images/hen.png"));
            } else {
                chicken->setPixmap(QPixmap(":/images/chicken.png"));
            }
        }
    }
    wingUp = !wingUp;
}


