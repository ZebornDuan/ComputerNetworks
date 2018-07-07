#ifndef GOMOKU_H
#define GOMOKU_H

#include <QMainWindow>
#include <QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

namespace Ui {
class Gomoku;
}

class Gomoku : public QMainWindow {
    Q_OBJECT

public:
    explicit Gomoku(QWidget *parent = 0);
    ~Gomoku();

signals:
    void addChess(int row, int col);
    void startGame();
    void timeLimitExceed();
    void undo();
    void load(int id);

private slots:
    void on_createButton_clicked();
    void on_connectButton_clicked();
    void readData();
    void sendChessInfo(QString chessInfo);
    void sendWin();
    void sendSave();
    void requestQuit();
    void requestUndo();
    void requestLoad();

private:
    Ui::Gomoku *ui;
    QTcpServer *server;
    QTcpSocket *socket;
    QTimer *timer;
    int countDown;
    int playerTime;
    int enemyTime;
    int undoLeft;

    void save(int id);
};

#endif // GOMOKU_H
