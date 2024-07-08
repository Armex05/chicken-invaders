#include "gamewidgetlevel2.h"
#include "GameWidgetLevel3.h"
#include <QGraphicsTextItem>
#include <QRandomGenerator>

GameWidgetLevel2::GameWidgetLevel2(QWidget *parent): GameWidget(parent,1), meatCount(0), wingUp(true),isPaused(false) {
    eggShootTimer = new QTimer(this);
    connect(eggShootTimer, &QTimer::timeout, this, &GameWidgetLevel2::chickensShootEggs);
    eggShootTimer->start(5000); // 5 seconds interval

    chickenMovementTimer = new QTimer(this);
    connect(chickenMovementTimer, &QTimer::timeout, this, &GameWidgetLevel2::startMovingChickens);
    chickenMovementTimer->setSingleShot(true);
    chickenMovementTimer->start(4000); // 4 secs to start generating chickens

    secondChickenMovement = new QTimer(this);
    connect(secondChickenMovement,&QTimer::timeout, this, &GameWidgetLevel2::startSecondMovingChickens);
    secondChickenMovement->setSingleShot(true);
    secondChickenMovement->start(18000);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWidgetLevel2::gameLoop);
    timer->start(16);


    meatText = new QGraphicsTextItem();
    meatText->setPlainText(QString("meats: %1").arg(meatCount));
    meatText->setDefaultTextColor(Qt::red);
    meatText->setFont(QFont("Arial", 16));
    meatText->setPos(10, 450);
    scene->addItem(meatText);

}
void GameWidgetLevel2::togglePauseContinue() {

}
void GameWidgetLevel2::nextSeason(){

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
    int numEggsToShoot = chickens.size() / 8;
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
            chicken->setData(0,2); // Chickens have 2 health
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
            ship->setY(ship->y() + 2);
        }
    }
}
void GameWidgetLevel2::updateEggs() {
    for (QGraphicsPixmapItem *egg : qAsConst(eggs)) {
            if(egg->y() > GameWidgetLevel2::scene->height() * 0.6 - 10){
                    delete egg;
                    eggs.removeOne(egg);
            }else{
                egg->setY(egg->y() + 0.4);
            }
    }
}
void GameWidgetLevel2::updateChickens(){
    for (int i = 0; i < chickens.size(); ++i){
        if(chickens[i]->y() > GameWidgetLevel2::scene->height() * 0.6 - 170){
        //    scene->removeItem(chickens[i]);
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
        //    scene->removeItem(egg);
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
            if (meatCount == 30) {
                score += 50;
                meatCount = 0;
                return;
            }
        }
    }

    for (int i = 0; i < bullets.size(); ++i) {
        for (int j = 0; j < eggs.size(); ++j) {
            if (bullets[i]->collidesWithItem(eggs[j])) {
            //    scene->removeItem(bullets[i]);
            //    scene->removeItem(eggs[j]);
                delete bullets[i];
                delete eggs[j];
                bullets.removeAt(i);
                eggs.removeAt(j);
            //    scene->removeItem(scoreText);
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
        //    scene->removeItem(chicken);
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
                    //    scene->removeItem(bullet);
                        bullets.removeOne(bullet);
                        delete bullet;
                    }
                }
            }
        }
    }
}

void GameWidgetLevel2::decreaseChickenHealth(QGraphicsPixmapItem* chicken) {
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
    //    scene->removeItem(chicken);
        chickens.removeOne(chicken);
        delete chicken;
    } else {
        chicken->setData(0, health); // Update the health data
        return;
    }
}

void GameWidgetLevel2::animateChickens() {

}


