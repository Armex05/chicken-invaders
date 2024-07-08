#include "GameWidgetLevel3.h"
#include <QGraphicsTextItem>
#include <QRandomGenerator>

GameWidgetLevel3::GameWidgetLevel3(QWidget *parent): GameWidget(parent,1), meatCount(0), wingUp(true),isPaused(false),isTowBullet(false),
gift(nullptr) {
    eggShootTimer = new QTimer(this);
    connect(eggShootTimer, &QTimer::timeout, this, &GameWidgetLevel3::chickensShootEggs);
    eggShootTimer->start(6500); // 5 seconds interval

    chickenMovementTimer = new QTimer(this);
    connect(chickenMovementTimer, &QTimer::timeout, this, &GameWidgetLevel3::startMovingChickens);
    chickenMovementTimer->setSingleShot(true);
    chickenMovementTimer->start(4000); // 4 secs to start generating chickens

    secondChickenMovement = new QTimer(this);
    connect(secondChickenMovement,&QTimer::timeout, this, &GameWidgetLevel3::startSecondMovingChickens);
    secondChickenMovement->setSingleShot(true);
    secondChickenMovement->start(18000);

    timer = new QTimer(this);
    connect(timer,&QTimer::timeout, this,&GameWidgetLevel3::gameLoop);
    timer->start(16);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &GameWidgetLevel3::animateChickens);
    animationTimer->start(500); // 500 milliseconds interval for animation

    giftTimer = new QTimer(this);
    connect(giftTimer,&QTimer::timeout, this, &GameWidgetLevel3::startGiftMovement);
    giftTimer->setSingleShot(true);
    giftTimer->start(15000); //generating a gift at the 15th second of the game

    meatText = new QGraphicsTextItem();
    meatText->setPlainText(QString("meats: %1").arg(meatCount));
    meatText->setDefaultTextColor(Qt::red);
    meatText->setFont(QFont("Arial", 16));
    meatText->setPos(10, 450);
    scene->addItem(meatText);

    pauseContinueButton = new QPushButton("Pause", this);
    connect(pauseContinueButton,&QPushButton::clicked,this,&GameWidgetLevel3::togglePauseContinue);
    pauseContinueButton->setGeometry(10,50,80,30);

}
void GameWidgetLevel3::togglePauseContinue() {
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

void GameWidgetLevel3::gameLoop() {
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
        updateGift();//the function for getting the gift all the way down just like eggs and others
    }
}
void GameWidgetLevel3::chickensShootEggs() {
    int numEggsToShoot = chickens.size() / 3;
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
void GameWidgetLevel3::startGiftMovement() {
    if (isPaused) return;

    gift = new QGraphicsPixmapItem(QPixmap(":/images/gift.png"));
    gift->setScale(0.075 );
    int random_number = rand() % 600; // making the gift come to the screen at a random time
    gift->setPos(random_number,0);
    scene->addItem(gift);
}

void GameWidgetLevel3::startMovingChickens(){
    if (isPaused) return;
    chickensMoving = true;

    int columns = 6;
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
            chicken = new QGraphicsPixmapItem(QPixmap(":/images/super-chicken.png"));
            chicken->setScale(0.07);
            chicken->setData(0,4); // Chicks have 1 health
            chicken->setData(1,"chicken");
        }

        chicken->setPos(column * 80 + 150, row * 80 - 250);
        chickens.append(chicken);
        scene->addItem(chicken);
    }
}
void GameWidgetLevel3::startSecondMovingChickens() {
    if (isPaused) return;
    int columns = 9;
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
            chicken = new QGraphicsPixmapItem(QPixmap(":/images/super-chicken.png"));
            chicken->setScale(0.07);
            chicken->setData(0,4); // Chicks have 1 health
            chicken->setData(1,"chicken");
        }

        chicken->setPos(column * 80 + 30, row * 80 - 250);
        chickens.append(chicken);
        scene->addItem(chicken);
    }
}

void GameWidgetLevel3::updateEggs() {
    for (QGraphicsPixmapItem *egg : qAsConst(eggs)) {
        if(egg->y() > GameWidgetLevel3::scene->height() * 0.6 - 10){
            delete egg;
            eggs.removeOne(egg);
        }else{
            egg->setY(egg->y() + 0.4);
        }
    }
}
void GameWidgetLevel3::updateChickens(){
    for (int i = 0; i < chickens.size(); ++i){
        if(chickens[i]->y() > GameWidgetLevel3::scene->height() * 0.6 - 170){
            //scene->removeItem(chickens[i]);
            delete chickens[i];
            chickens.removeAt(i);
        }
        else{
            chickens[i]->setY(chickens[i]->y() + 0.7);
        }
    }
}

void GameWidgetLevel3::updateMeats() {
    for (QGraphicsPixmapItem *meat : qAsConst(meats)) {
        meat->setY(meat->y() + 2);
        if (meat->y() > GameWidgetLevel3::scene->height() * 0.6 -5) {
          //  GameWidgetLevel3::scene->removeItem(meat);
            meats.removeOne(meat);
            delete meat;
        }
    }
}

void GameWidgetLevel3::updateGift(){
    if (isPaused || gift == nullptr) return;

    gift->setY(gift->y() + 2);
    if (gift->y() > GameWidgetLevel3::scene->height() * 0.6 - 5){
     //   GameWidgetLevel3::scene->removeItem(gift);
        delete gift;
        gift = nullptr;
    }
}

void GameWidgetLevel3::checkCollisions() {

    if (ship->collidesWithItem(gift)){
        isTowBullet = true;
        GameWidgetLevel3::scene->removeItem(gift);
        delete gift;
        gift = nullptr;
    }


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
         //   scene->removeItem(meat);
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
           //     scene->removeItem(bullets[i]);
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
                scoreText->setFont(QFont("Arial",16));
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
            connect(invincibilityTimer, &QTimer::timeout, this, &GameWidgetLevel3::endInvincibility);
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
                     //   scene->removeItem(bullet);
                        bullets.removeOne(bullet);
                        delete bullet;
                    }
                }
            }
        }
    }
}

void GameWidgetLevel3::decreaseChickenHealth(QGraphicsPixmapItem* chicken) {
    int health = chicken->data(0).toInt();
    health--;
    if (health <= 0) {
        QString type = chicken->data(1).toString();
        if (type == "hen") {//checking by the stored data that if it was chicken or super-chicken
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
        }
        chickens.removeOne(chicken);
        delete chicken;
    } else {
        chicken->setData(0, health); // Update the health data of the chickens by every new collision with bullets
        return;
    }
}

void GameWidgetLevel3::animateChickens() {
    for (QGraphicsPixmapItem *chicken : qAsConst(chickens)) {
        QString type = chicken->data(1).toString();
        if (type == "chicken") {
            if (wingUp) {
                 chicken->setPixmap(QPixmap(":/images/super-chicken.png"));
            } else {
                chicken->setPixmap(QPixmap(":/images/super-chicken.png"));
            }
        }
    }
    wingUp = !wingUp; // checking up for the animation of the chicken
}
void GameWidgetLevel3::keyPressEvent(QKeyEvent *event) {
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
            if (!isTowBullet){
                QGraphicsPixmapItem *bullet = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
                bullet->setScale(0.05);
                bullet->setPos(ship->x() + 30, ship->y());
                bullets.append(bullet);
                scene->addItem(bullet);
                break;
            } else{
                QGraphicsPixmapItem *bullet1 = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
                bullet1->setScale(0.05);
                bullet1->setPos(ship->x() + 30, ship->y());
                bullets.append(bullet1);
                scene->addItem(bullet1);
                QGraphicsPixmapItem *bullet2 = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
                bullet2->setScale(0.05);
                bullet2->setPos(ship->x() + 30, ship->y() - 30);
                bullets.append(bullet2);
                scene->addItem(bullet2);
            }
        }
        default:
            QGraphicsView::keyPressEvent(event);
    }
}

void GameWidgetLevel3::keyReleaseEvent(QKeyEvent *event) {
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
void GameWidgetLevel3::mouseMoveEvent(QMouseEvent *event) {
    ship->setPos(event->x() - ship->boundingRect().width() / 18, event->y() - ship->boundingRect().height() / 18);
}

void GameWidgetLevel3::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (!isTowBullet){
            QGraphicsPixmapItem *bullet = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
            bullet->setScale(0.05);
            bullet->setPos(ship->x() + 30, ship->y());
            bullets.append(bullet);
            scene->addItem(bullet);
        } else{
            QGraphicsPixmapItem *bullet1 = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
            bullet1->setScale(0.05);
            bullet1->setPos(ship->x() + 30, ship->y());
            bullets.append(bullet1);
            scene->addItem(bullet1);
            QGraphicsPixmapItem *bullet2 = new QGraphicsPixmapItem(QPixmap(":/images/b.png"));
            bullet2->setScale(0.05);
            bullet2->setPos(ship->x() + 30, ship->y() - 30);
            bullets.append(bullet2);
            scene->addItem(bullet2);
        }
    }
}

