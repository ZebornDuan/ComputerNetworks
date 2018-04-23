#include "gameboard.h"
#include "saveloaddialog.h"
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QMessageBox>
#include <QSettings>

namespace {

GameBoard::State toState(QColor color) {
    return (color == Qt::white)? GameBoard::State::WHITE: GameBoard::State::BLACK;
}

}

int GameBoard::sideLength = 420;
int GameBoard::gameSize = 15;
int GameBoard::margin = 20;
int GameBoard::spacing = GameBoard::sideLength / (GameBoard::gameSize - 1);

GameBoard::GameBoard(QWidget *parent) :
    QWidget(parent),
    inRound(false),
    isRunning(false) {
    // TODO
    playerColor = Qt::white;
    enemyColor = Qt::black;
    installEventFilter(this);

    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            states[i][j] = State::NONE;
}

bool GameBoard::eventFilter(QObject *obj, QEvent *ev) {
    if (obj != this) {
        return false;
    }

    if (ev->type() == QEvent::MouseButtonDblClick) {
        return true;
    }

    if (ev->type() != QEvent::MouseButtonPress) {
        return false;
    }

    if (!inRound)
        return true;

    QMouseEvent *mouseEv = static_cast<QMouseEvent *>(ev);

    if (states[toIndex(mouseEv->y())][toIndex(mouseEv->x())] != State::NONE) {
        return true;
    }

    return false;
}

void GameBoard::paintEvent(QPaintEvent *ev) {

    QPainter p(this);
    p.setBrush(Qt::gray);
    p.setPen(Qt::NoPen);
    p.drawRect(0, 0, sideLength + 2 * margin, sideLength + 2 * margin);
    p.translate(margin, margin);
    p.setPen(QPen(Qt::black, 1));

    for (int i = 0; i < gameSize; i++) {
        p.drawLine(0, i * spacing, sideLength, i * spacing);
        p.drawLine(i * spacing, 0, i * spacing, sideLength);
    }

    p.setPen(QPen(Qt::black, 5));
    p.drawPoint(3 * spacing, 3 * spacing);
    p.drawPoint(3 * spacing, 11 * spacing);
    p.drawPoint(11 * spacing, 3 * spacing);
    p.drawPoint(11 * spacing, 11 * spacing);
    p.drawPoint(7 * spacing, 7 * spacing);

    p.setPen(Qt::NoPen);
    p.setBrush(playerColor);

    for (int i = 0; i < playerChesses.size(); i++) {
        if (playerChesses[i].row == -1)
            continue;

        p.drawEllipse(QPoint(playerChesses[i].getX(), playerChesses[i].getY()), (int)(spacing * 0.4), (int)(spacing * 0.4));
    }

    p.setBrush(enemyColor);

    for (int i = 0; i < enemyChesses.size(); i++) {
        if (enemyChesses[i].row == -1)
            continue;

        p.drawEllipse(QPoint(enemyChesses[i].getX(), enemyChesses[i].getY()), (int)(spacing * 0.4), (int)(spacing * 0.4));
    }
    p.setBrush(Qt::red);

    for(int i = 0;i < indanger.size();i++){
        if(indanger[i].row == -1)
            continue;
        p.drawEllipse(QPoint(indanger[i].getX(),indanger[i].getY()),(int)(spacing * 0.2),(int)(spacing * 0.2));
    }
    p.setBrush(Qt::green);

    for(int i = 0;i < victory.size();i++){
        if(victory[i].row == -1)
            continue;
         p.drawEllipse(QPoint(victory[i].getX(),victory[i].getY()),(int)(spacing * 0.2),(int)(spacing * 0.2));
    }
}

void GameBoard::mousePressEvent(QMouseEvent *mouseEv) {
    int row = toIndex(mouseEv->y());
    int col = toIndex(mouseEv->x());

    states[row][col] = toState(playerColor);
    Chess chess(row, col, playerColor);
    playerChesses.push_back(chess);
    indanger.clear();
    victory.clear();
    emit addChess(chess.toString());
    update();
    inRound = false;

    if (isWin(row, col)) {
        emit win();
    }
}

bool GameBoard::isWin(int row, int col) {
    State state = states[row][col];
    int cnt = 1;

    for (int i = row - 1; i >= 0; i--) {
        if (states[i][col] != state)
            break;

        ++cnt;
    }

    for (int i = row + 1; i < gameSize; i++) {
        if (states[i][col] != state)
            break;

        ++cnt;
    }

    if (cnt >= 5)
        return true;

    cnt = 1;

    for (int i = col - 1; i >= 0; i--) {
        if (states[row][i] != state)
            break;

        ++cnt;
    }

    for (int i = col + 1; i < gameSize; i++) {
        if (states[row][i] != state)
            break;

        ++cnt;
    }

    if (cnt >= 5)
        return true;

    cnt = 1;

    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        if (states[i][j] != state)
            break;

        ++cnt;
    }

    for (int i = row + 1, j = col + 1; i < gameSize && j < gameSize; i++, j++) {
        if (states[i][j] != state)
            break;

        ++cnt;
    }

    if (cnt >= 5)
        return true;

    cnt = 1;

    for (int i = row - 1, j = col + 1; i >= 0 && j < gameSize; i--, j++) {
        if (states[i][j] != state)
            break;

        ++cnt;
    }

    for (int i = row + 1, j = col - 1; i < gameSize && j >= 0; i++, j--) {
        if (states[i][j] != state)
            break;

        ++cnt;
    }

    if (cnt >= 5)
        return true;

    return false;
}

const std::vector<GameBoard::Chess> &GameBoard::getEnemyChesses() const {
    return enemyChesses;
}

const std::vector<GameBoard::Chess> &GameBoard::getPlayerChesses() const {
    return playerChesses;
}

inline int GameBoard::toIndex(int pixel) const {
    return ((pixel - margin + 0.5 * spacing) / spacing);
}

void GameBoard::clear() {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            states[i][j] = State::NONE;
        }
    }

    playerChesses.clear();
    enemyChesses.clear();
}

void GameBoard::enemyChessAdded(int row, int col) {
    enemyChesses.push_back(Chess(row, col, enemyColor));

    if (row != -1 && col != -1)
        states[row][col] = toState(enemyColor);

    update();
    inRound = true;
}

void GameBoard::onStart() {
    //    isRunning = true;
}

void GameBoard::onTimeout() {
    Chess chess(-1, -1, playerColor);
    playerChesses.push_back(chess);
    emit addChess(chess.toString());
}

void GameBoard::onUndo() {
    auto chess1 = playerChesses.back();

    if (chess1.row != -1 && chess1.col != -1)
        states[chess1.row][chess1.col] = State::NONE;

    playerChesses.pop_back();

    auto chess2 = enemyChesses.back();

    if (chess2.row != -1 && chess2.col != -1)
        states[chess2.row][chess2.col] = State::NONE;

    enemyChesses.pop_back();

    update();
}

void GameBoard::onLoad(int id) {
    clear();

    QSettings settings;

    int size = settings.beginReadArray(QString("white") + QString::number(id));

    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        Chess chess;
        chess.row = settings.value("row").toInt();
        chess.col = settings.value("col").toInt();
        states[chess.row][chess.col] = State::WHITE;
        chess.color = Qt::white;
        ((playerColor == Qt::white)? playerChesses: enemyChesses).push_back(chess);
    }

    settings.endArray();

    size = settings.beginReadArray(QString("black") + QString::number(id));
    qDebug() << "black" << size;

    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        Chess chess;
        chess.row = settings.value("row").toInt();
        chess.col = settings.value("col").toInt();

        if (chess.row != -1 && chess.col != -1)
            states[chess.row][chess.col] = State::BLACK;

        chess.color = Qt::black;
        ((playerColor == Qt::white)? enemyChesses: playerChesses).push_back(chess);
    }

    settings.endArray();

    update();
}

void GameBoard::isDanger(){
    State state = toState(enemyColor);

    for(int i = 0;i < 15;i++)
        for(int j = 0;j < 15;j++){
            int count = 1;
            int live3 = 0;
            int dead4 = 0;
            int left = 0;
            int right = 0;
            int up = 0;
            int down = 0;
            if(states[i][j] != State::NONE)
                continue;
            for(int a = i - 1;a >= 0;a-- ){
                if(states[a][j] != state){
                    up = a;
                    break;
                }
                ++count;
            }
            for(int a = i + 1;a < gameSize;a++){
                if(states[a][j] != state){
                    down = a;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[up][j] == State::NONE && states[down][j] == State::NONE){
               Chess chess(i,j,enemyColor);
               indanger.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,enemyColor);
                indanger.push_back(chess);
                continue;
            }
            if(count == 3 && states[up][j] == State::NONE && states[down][j] == State::NONE)
                live3++;
            if(count == 4 && ((states[up][j] == State::NONE && states[down][j] != State::NONE)
                              || (states[up][j] != State::NONE && states[down][j] == State::NONE)))
                dead4++;
            count = 1;
            for(int a = j - 1;a > 0;a--){
                if(states[i][a] != state){
                    left = a;
                    break;
                }
                ++count;
            }
            for(int a = j + 1;a < gameSize;a++){
                if(states[i][a] != state){
                    right = a;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[i][left] == State::NONE && states[i][right] == State::NONE){
               Chess chess(i,j,enemyColor);
               indanger.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,enemyColor);
                indanger.push_back(chess);
                continue;
            }
            if(count == 4 && ((states[i][left] == State::NONE && states[i][right] != State::NONE)
                              || (states[i][left] != State::NONE && states[i][right] == State::NONE)))
                dead4++;
            if(count == 3 && states[i][left] == State::NONE && states[i][right] == State::NONE)
                live3++;
            count =1;
            for(int a = i - 1,b = j - 1;a > 0 && b > 0;a--,b--){
                if(states[a][b] != state){
                    up = a;
                    left = b;
                    break;
                }
                ++count;
            }
            for(int a = i + 1,b = j + 1;a < gameSize && b < gameSize;a++,b++){
                if(states[a][b] != state){
                    down = a;
                    right = b;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[up][left] == State::NONE && states[down][right] == State::NONE){
               Chess chess(i,j,enemyColor);
               indanger.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,enemyColor);
                indanger.push_back(chess);
                continue;
            }
            if(count == 4 && ((states[up][left] == State::NONE && states[down][right] != State::NONE)
                              || (states[up][left] != State::NONE && states[down][right] == State::NONE)))
                dead4++;
            if(count == 3 && states[up][left] == State::NONE && states[down][right] == State::NONE)
                live3++;
            count = 1;
            for(int a = i - 1,b = j + 1;a > 0 && b < gameSize;a--,b++){
                if(states[a][b] != state){
                    up = a;
                    right = b;
                    break;
                }
                ++count;
            }
            for(int a = i + 1,b = j - 1;a < gameSize && b > 0;a++,b--){
                if(states[a][b] != state){
                    down = a;
                    left = b;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[up][right] == State::NONE && states[down][left] == State::NONE){
               Chess chess(i,j,enemyColor);
               indanger.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,enemyColor);
                indanger.push_back(chess);
                continue;
            }
            if(count == 4 && ((states[up][right] == State::NONE && states[down][left] != State::NONE)
                              || (states[up][right] != State::NONE && states[down][left] == State::NONE)))
                dead4++;
            if(count == 3 && states[up][right] == State::NONE && states[down][left] == State::NONE)
                live3++;
            if(live3 + dead4 > 1){
                Chess chess(i,j,enemyColor);
                indanger.push_back(chess);
                continue;
            }

        }
    state = toState(playerColor);
    for(int i = 0;i < 15;i++)
        for(int j = 0;j < 15;j++){
            int count = 1;
            int live3 = 0;
            int dead4 = 0;
            int left = 0;
            int right = 0;
            int up = 0;
            int down = 0;
            if(states[i][j] != State::NONE)
                continue;
            for(int a = i - 1;a >= 0;a-- ){
                if(states[a][j] != state){
                    up = a;
                    break;
                }
                ++count;
            }
            for(int a = i + 1;a < gameSize;a++){
                if(states[a][j] != state){
                    down = a;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[up][j] == State::NONE && states[down][j] == State::NONE){
               Chess chess(i,j,playerColor);
               victory.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,playerColor);
                victory.push_back(chess);
                continue;
            }
            if(count == 3 && states[up][j] == State::NONE && states[down][j] == State::NONE)
                live3++;
            if(count == 4 && ((states[up][j] == State::NONE && states[down][j] != State::NONE)
                              || (states[up][j] != State::NONE && states[down][j] == State::NONE)))
                dead4++;
            count = 1;
            for(int a = j - 1;a > 0;a--){
                if(states[i][a] != state){
                    left = a;
                    break;
                }
                ++count;
            }
            for(int a = j + 1;a < gameSize;a++){
                if(states[i][a] != state){
                    right = a;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[i][left] == State::NONE && states[i][right] == State::NONE){
               Chess chess(i,j,playerColor);
               victory.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,playerColor);
                victory.push_back(chess);
                continue;
            }
            if(count == 4 && ((states[i][left] == State::NONE && states[i][right] != State::NONE)
                              || (states[i][left] != State::NONE && states[i][right] == State::NONE)))
                dead4++;
            if(count == 3 && states[i][left] == State::NONE && states[i][right] == State::NONE)
                live3++;
            count =1;
            for(int a = i - 1,b = j - 1;a > 0 && b > 0;a--,b--){
                if(states[a][b] != state){
                    up = a;
                    left = b;
                    break;
                }
                ++count;
            }
            for(int a = i + 1,b = j + 1;a < gameSize && b < gameSize;a++,b++){
                if(states[a][b] != state){
                    down = a;
                    right = b;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[up][left] == State::NONE && states[down][right] == State::NONE){
               Chess chess(i,j,playerColor);
               victory.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,playerColor);
                victory.push_back(chess);
                continue;
            }
            if(count == 4 && ((states[up][left] == State::NONE && states[down][right] != State::NONE)
                              || (states[up][left] != State::NONE && states[down][right] == State::NONE)))
                dead4++;
            if(count == 3 && states[up][left] == State::NONE && states[down][right] == State::NONE)
                live3++;
            count = 1;
            for(int a = i - 1,b = j + 1;a > 0 && b < gameSize;a--,b++){
                if(states[a][b] != state){
                    up = a;
                    right = b;
                    break;
                }
                ++count;
            }
            for(int a = i + 1,b = j - 1;a < gameSize && b > 0;a++,b--){
                if(states[a][b] != state){
                    down = a;
                    left = b;
                    break;
                }
                ++count;
            }
            if(count == 4 && states[up][right] == State::NONE && states[down][left] == State::NONE){
               Chess chess(i,j,playerColor);
               victory.push_back(chess);
               continue;
            }
            if(count >= 5){
                Chess chess(i,j,playerColor);
                victory.push_back(chess);
                continue;
            }
            if(count == 4 && ((states[up][right] == State::NONE && states[down][left] != State::NONE)
                              || (states[up][right] != State::NONE && states[down][left] == State::NONE)))
                dead4++;
            if(count == 3 && states[up][right] == State::NONE && states[down][left] == State::NONE)
                live3++;
            if(live3 + dead4 > 1){
                Chess chess(i,j,playerColor);
                victory.push_back(chess);
                continue;
            }

        }
    update();

}
