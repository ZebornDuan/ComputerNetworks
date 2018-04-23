#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QVector>
#include <QPaintEvent>
#include <QMouseEvent>

class GameBoard : public QWidget {
    Q_OBJECT
public:
    explicit GameBoard(QWidget *parent = 0);
    bool eventFilter(QObject *obj, QEvent *ev);
    void paintEvent(QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *mouseEv);

    enum class State {
        NONE = 0,
        WHITE = 1,
        BLACK = 2
    };

    struct Chess {
        int row;
        int col;
        QColor color;
        Chess() { }
        Chess(int row, int col, QColor color): row(row), col(col), color(color) { }

        int getX() {
            return GameBoard::spacing * (col);
        }

        int getY() {
            return GameBoard::spacing * (row);
        }

        QString toString() {
            return QString("%1 %2 ").
                    arg(QString::number(row),
                       QString::number(col));
        }
    };

    QColor playerColor;
    QColor enemyColor;
    bool inRound;
    void clear();

    const std::vector<Chess> &getPlayerChesses() const;
    const std::vector<Chess> &getEnemyChesses() const;

private:
    bool isRunning;
    State states[15][15] = {{(State)0}};
    static int sideLength;
    static int gameSize;
    static int margin;
    static int spacing;
    bool isWin(int row, int col);

    std::vector<Chess> playerChesses;
    std::vector<Chess> enemyChesses;

    std::vector<Chess> indanger;
    std::vector<Chess> victory;

    inline int toIndex(int pixel) const;
signals:
    void addChess(QString chessInfo);
    void win();

public slots:
    void enemyChessAdded(int row, int col);
    void onStart();
    void onTimeout();
    void onUndo();
    void onLoad(int id);
    void isDanger();
};

#endif // GAMEBOARD_H
