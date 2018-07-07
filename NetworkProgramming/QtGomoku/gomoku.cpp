#include "gomoku.h"
#include "ui_gomoku.h"
#include "createdialog.h"
#include "connectdialog.h"
#include "saveloaddialog.h"
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QSound>

Gomoku::Gomoku(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gomoku),
    countDown(60),
    enemyTime(0),
    playerTime(0),
    undoLeft(2) {
    QCoreApplication::setOrganizationName("DemonBlack");
    QCoreApplication::setApplicationName("Gomoku");
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(this, &Gomoku::addChess, ui->gameboard, &GameBoard::enemyChessAdded);
    connect(this, &Gomoku::startGame, ui->gameboard, &GameBoard::onStart);
    connect(this, &Gomoku::undo, ui->gameboard, &GameBoard::onUndo);
    connect(this, &Gomoku::load, ui->gameboard, &GameBoard::onLoad);
    connect(ui->tipButton,&QPushButton::clicked,ui->gameboard,&GameBoard::isDanger);
    connect(timer, &QTimer::timeout, [=]() {
        if (!ui->gameboard->inRound) {
            return;
        }

        --countDown;
        ui->countDown->display(countDown);

        if (countDown == 0) {
            emit timeLimitExceed();
        }
    });

    connect(this, &Gomoku::timeLimitExceed, ui->gameboard, &GameBoard::onTimeout);

    timer->setInterval(1000);
}

Gomoku::~Gomoku() {
    delete ui;
}

void Gomoku::on_createButton_clicked() {
    CreateDialog dialog;

    if (dialog.exec() == QDialog::Accepted) {
        server = new QTcpServer(this);
        server->listen(QHostAddress(dialog.getHostIpAddress()), 8888);
        ui->connectButton->setEnabled(false);
        ui->createButton->setEnabled(false);
        QMessageBox *waiting = new QMessageBox(QMessageBox::NoIcon,
                                               tr("waiting"),
                                               dialog.getHostIpAddress() + tr("\n等待建立连接..."),
                                               QMessageBox::Cancel,
                                               this);

        bool connecting = false;
        connect(server, &QTcpServer::newConnection, [&]() {
            connecting = true;
            socket = server->nextPendingConnection();
//            ui->undoButton->setEnabled(true);
            ui->saveButton->setEnabled(true);
            ui->loadButton->setEnabled(true);
            ui->quitButton->setEnabled(true);
            ui->tipButton->setEnabled(true);
            ui->createButton->setEnabled(false);
            ui->connectButton->setEnabled(false);
            ui->gameboard->playerColor = Qt::black;
            ui->gameboard->enemyColor = Qt::white;
            ui->gameboard->inRound = true;
            ui->colorLabel->setText("<font color='black'>黑方落子</font>");
            connect(socket, &QTcpSocket::readyRead, this, &Gomoku::readData);
            connect(ui->gameboard, &GameBoard::addChess, this, &Gomoku::sendChessInfo);
            connect(ui->gameboard, &GameBoard::win, this, &Gomoku::sendWin);
            connect(ui->quitButton, &QPushButton::clicked, this, &Gomoku::requestQuit);
            connect(ui->undoButton, &QPushButton::clicked, this, &Gomoku::requestUndo);
            connect(ui->saveButton, &QPushButton::clicked, this, &Gomoku::sendSave);
            connect(ui->loadButton, &QPushButton::clicked, this, &Gomoku::requestLoad);
            waiting->close();
        });

        if (waiting->exec() == QMessageBox::Cancel && !connecting) {
            server->close();
            ui->connectButton->setEnabled(true);
            ui->createButton->setEnabled(true);
        }
    }
}

void Gomoku::on_connectButton_clicked() {
    ConnectDialog dialog;

    if (dialog.exec() == QDialog::Accepted) {
        socket = new QTcpSocket(this);
        socket->connectToHost(QHostAddress(dialog.getHostIp()), 8888);

        if (socket->waitForConnected(3000)) {
//            ui->undoButton->setEnabled(true);
            ui->quitButton->setEnabled(true);
            ui->tipButton->setEnabled(true);
            ui->gameboard->playerColor = Qt::white;
            ui->gameboard->enemyColor = Qt::black;
            ui->createButton->setEnabled(false);
            ui->connectButton->setEnabled(false);
            ui->colorLabel->setText("<font color='black'>黑方落子</font>");
            connect(socket, &QTcpSocket::readyRead, this, &Gomoku::readData);
            connect(ui->gameboard, &GameBoard::addChess, this, &Gomoku::sendChessInfo);
            connect(ui->gameboard, &GameBoard::win, this, &Gomoku::sendWin);
            connect(ui->quitButton, &QPushButton::clicked, this, &Gomoku::requestQuit);
            connect(ui->undoButton, &QPushButton::clicked, this, &Gomoku::requestUndo);
            connect(ui->saveButton, &QPushButton::clicked, this, &Gomoku::sendSave);
            connect(ui->loadButton, &QPushButton::clicked, this, &Gomoku::requestLoad);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("连接失败"));
        }
    }
}

void Gomoku::sendChessInfo(QString chessInfo) {
    int costTime = 60 - countDown;
    ui->colorLabel->setText((ui->gameboard->playerColor == Qt::white)? "<font color='black'>黑方落子</font>": "<font color='white'>白方落子</font>");
    countDown = 60;
    ui->countDown->display(countDown);
    playerTime += costTime;
    ui->playerTimeCount->display(playerTime);
    undoLeft = 2;
    ui->loadButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->undoButton->setEnabled(false);
    ui->tipButton->setEnabled(false);
    socket->write((QString("add ") + chessInfo + QString::number(costTime) + QString(" ")).toStdString().c_str());
}

void Gomoku::sendWin() {
    timer->stop();
   // QSound::play(":/res/win.wav");
    QMessageBox *message = new QMessageBox(QMessageBox::NoIcon,
                                           tr("Win"),
                                           tr("恭喜 <strong>胜利</strong>.\n我方用时 %1 秒.\n敌方用时 %2 秒.").
                                           arg(QString::number(ui->playerTimeCount->value()), QString::number(ui->enemyTimeCount->value())),
                                           QMessageBox::Ok,
                                           this);
    message->show();
    ui->quitButton->setEnabled(false);
    socket->write("win ");
}

void Gomoku::sendSave() {
    saveLoadDialog dialog;

    if (dialog.exec() != QDialog::Accepted)
        return;

    int id = dialog.getResult();

    save(id);
    socket->write((QString("save %1 ").arg(QString::number(id))).toStdString().c_str());
}

void Gomoku::requestQuit() {
    ui->quitButton->setEnabled(false);
    ui->quitButton->setText("waiting...");
    socket->write("quit ");
}

void Gomoku::requestUndo() {
    ui->undoButton->setEnabled(false);
    ui->undoButton->setText("waiting...");
    socket->write("undo ");
}

void Gomoku::requestLoad() {
    saveLoadDialog dialog;
    QSettings settings;

    if (dialog.exec() == QDialog::Accepted) {
        int id = dialog.getResult();
        if (!settings.contains("time" + QString::number(id))) {
            QMessageBox::warning(this, tr("Load"), tr("Load failed"));
            return;
        }

        ui->loadButton->setEnabled(false);
        ui->loadButton->setText("waiting...");
        socket->write((QString("load %1 ").arg(QString::number(id))).toStdString().c_str());
    }
}

void Gomoku::save(int id) {
    QSettings settings;
    QString date = QDateTime().currentDateTime().toString();
    settings.setValue(QString("time") + QString::number(id), date);
    settings.setValue(QString((ui->gameboard->playerColor == Qt::white)? "whiteTime": "blackTime") + QString::number(id), playerTime);
    settings.setValue(QString((ui->gameboard->playerColor == Qt::white)? "blackTime": "whiteTime") + QString::number(id), enemyTime);

    QString color = ((ui->gameboard->playerColor == Qt::white)? "white": "black") + QString::number(id);
    settings.beginWriteArray(color);
    const auto &pchess = ui->gameboard->getPlayerChesses();

    for (size_t i = 0; i < pchess.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("row", pchess[i].row);
        settings.setValue("col", pchess[i].col);
    }

    settings.endArray();

    color = (ui->gameboard->playerColor == Qt::black)? "white": "black";
    settings.beginWriteArray(color);
    const auto &echess = ui->gameboard->getEnemyChesses();

    for (size_t i = 0; i < echess.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("row", echess[i].row);
        settings.setValue("col", echess[i].col);
    }

    settings.endArray();
}

void Gomoku::readData() {
    auto args = QString(socket->readAll()).split(' ');
    qDebug() << args;
    bool isFinished = false;

    while (!isFinished) {
        if (args[0] == "add") {
            countDown = 60;
            emit addChess(args[1].toInt(), args[2].toInt());
            timer->start();
            int costTime = args[3].toInt();
            enemyTime += costTime;
            ui->enemyTimeCount->display(enemyTime);
            ui->saveButton->setEnabled(true);
            ui->loadButton->setEnabled(true);
            ui->tipButton->setEnabled(true);
            undoLeft = 2;

            if (ui->gameboard->getPlayerChesses().size() >= 1 && ui->gameboard->getEnemyChesses().size() >= 1 && undoLeft > 0)
                ui->undoButton->setEnabled(true);

            ui->colorLabel->setText((ui->gameboard->playerColor == Qt::white)?
                                    "<font color='white'>白方落子</font>":
                                    "<font color='black'>黑方落子</font>");

            if (!args[4].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 4; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        } else if (args[0] == "win") {
            timer->stop();
            //QSound::play(":/res/lose.wav");
            QMessageBox *message = new QMessageBox(QMessageBox::NoIcon,
                                                   tr("Lose"),
                                                   tr("可惜 <strong>失败</strong>.\n我方用时 %1 秒.\n敌方用时 %2 秒.").
                                                   arg(QString::number(ui->playerTimeCount->value()), QString::number(ui->enemyTimeCount->value())),
                                                   QMessageBox::Ok,
                                                   this);
            message->show();
            ui->quitButton->setEnabled(false);
            ui->tipButton->setEnabled(false);
            ui->undoButton->setEnabled(false);
            ui->saveButton->setEnabled(false);
            ui->loadButton->setEnabled(false);
            ui->gameboard->inRound = false;

            if (!args[1].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 1; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        } else if (args[0] == "save") {
            int id = args[1].toInt();
            save(id);

            if (!args[2].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 1; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        } else if (args[0] == "quit") {
            auto reply = QMessageBox::question(this,
                                               tr("Quit"),
                                               tr("对方请求认输，是否同意?"),
                                               QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                socket->write("agree quit ");
                socket->waitForBytesWritten();
                //qApp->quit();
                ui->gameboard->clear();
            } else {
                socket->write("reject quit ");
            }

            if (!args[1].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 1; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        } else if (args[0] == "undo") {
            auto reply = QMessageBox::question(this,
                                               tr("Undo"),
                                               tr("对方请求悔棋，是否同意？"),
                                               QMessageBox::Yes, QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                emit undo();
                --undoLeft;
                socket->write("agree undo ");
            } else {
                socket->write("reject undo ");
            }

            if (!args[1].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 1; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        } else if (args[0] == "load") {
            QString id = args[1];
            auto reply = QMessageBox::question(this,
                                               tr("Load"),
                                               tr("对方请求加载游戏 %1 ，是否同意?").arg(id),
                                               QMessageBox::Yes, QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                QSettings settings;
                int whiteTime = settings.value("whiteTime" + id).value<int>();
                int blackTime = settings.value("blackTime" + id).value<int>();
                playerTime = (ui->gameboard->playerColor == Qt::white)? whiteTime: blackTime;
                enemyTime = (ui->gameboard->playerColor == Qt::black)? whiteTime: blackTime;
                ui->playerTimeCount->display(playerTime);
                ui->enemyTimeCount->display(enemyTime);
                emit load(id.toInt());
                socket->write((QString("agree load %1 ").arg(id)).toStdString().c_str());
            } else {
                socket->write("reject load ");
            }

            if (!args[2].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 2; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        } else if (args[0] == "agree") {
            if (args[1] == "quit") {
                //qApp->quit();
                ui->gameboard->clear();
                ui->gameboard->update();
                ui->quitButton->setEnabled(true);
                ui->quitButton->setText("认输");
                if (!args[2].isEmpty()) {
                    auto temp = args;
                    args.clear();

                    for (int i = 2; i < temp.length(); i++) {
                        args << temp[i];
                    }
                } else {
                    isFinished = true;
                }

            } else if (args[1] == "undo") {
                emit undo();
                --undoLeft;

                if (ui->gameboard->getPlayerChesses().size() >= 1 &&
                        ui->gameboard->getEnemyChesses().size() >= 1 &&
                        undoLeft > 0)
                    ui->undoButton->setEnabled(true);

                ui->undoButton->setText("undo");

                if (!args[2].isEmpty()) {
                    auto temp = args;
                    args.clear();

                    for (int i = 2; i < temp.length(); i++) {
                        args << temp[i];
                    }
                } else {
                    isFinished = true;
                }
            } else if (args[1] == "load") {
                int id = args[2].toInt();
                QSettings settings;
                int whiteTime = settings.value("whiteTime" + QString::number(id)).value<int>();
                int blackTime = settings.value("blackTime" + QString::number(id)).value<int>();
                playerTime = (ui->gameboard->playerColor == Qt::white)? whiteTime: blackTime;
                enemyTime = (ui->gameboard->playerColor == Qt::black)? whiteTime: blackTime;
                ui->playerTimeCount->display(playerTime);
                ui->enemyTimeCount->display(enemyTime);
                emit load(id);
                ui->loadButton->setEnabled(true);
                ui->loadButton->setText("load");

                if (!args[3].isEmpty()) {
                    auto temp = args;
                    args.clear();

                    for (int i = 2; i < temp.length(); i++) {
                        args << temp[i];
                    }
                } else {
                    isFinished = true;
                }
            }
        } else if (args[0] == "reject") {
            if (args[1] == "quit") {
                ui->quitButton->setText("认输");
                ui->quitButton->setEnabled(true);
            } else if (args[1] == "undo") {
                ui->undoButton->setEnabled(true);
                ui->undoButton->setText("悔棋");
            } else if (args[1] == "load") {
                ui->loadButton->setEnabled(true);
                ui->loadButton->setText("读取");
            }

            if (!args[2].isEmpty()) {
                auto temp = args;
                args.clear();

                for (int i = 2; i < temp.length(); i++) {
                    args << temp[i];
                }
            } else {
                isFinished = true;
            }
        }
    }
}
