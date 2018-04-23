/********************************************************************************
** Form generated from reading UI file 'gomoku.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GOMOKU_H
#define UI_GOMOKU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gameboard.h"

QT_BEGIN_NAMESPACE

class Ui_Gomoku
{
public:
    QWidget *centralWidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *createButton;
    QPushButton *connectButton;
    QPushButton *undoButton;
    QPushButton *tipButton;
    QPushButton *quitButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    GameBoard *gameboard;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *colorLabel;
    QLCDNumber *countDown;
    QSpacerItem *verticalSpacer;
    QLabel *label_2;
    QLCDNumber *enemyTimeCount;
    QSpacerItem *verticalSpacer_2;
    QLabel *label;
    QLCDNumber *playerTimeCount;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Gomoku)
    {
        if (Gomoku->objectName().isEmpty())
            Gomoku->setObjectName(QStringLiteral("Gomoku"));
        Gomoku->resize(692, 539);
        centralWidget = new QWidget(Gomoku);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(510, 20, 160, 201));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        createButton = new QPushButton(verticalLayoutWidget);
        createButton->setObjectName(QStringLiteral("createButton"));

        verticalLayout->addWidget(createButton);

        connectButton = new QPushButton(verticalLayoutWidget);
        connectButton->setObjectName(QStringLiteral("connectButton"));

        verticalLayout->addWidget(connectButton);

        undoButton = new QPushButton(verticalLayoutWidget);
        undoButton->setObjectName(QStringLiteral("undoButton"));
        undoButton->setEnabled(false);

        verticalLayout->addWidget(undoButton);

        tipButton = new QPushButton(verticalLayoutWidget);
        tipButton->setObjectName(QStringLiteral("tipButton"));
        tipButton->setEnabled(false);
        tipButton->setCheckable(false);

        verticalLayout->addWidget(tipButton);

        quitButton = new QPushButton(verticalLayoutWidget);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setEnabled(false);

        verticalLayout->addWidget(quitButton);

        saveButton = new QPushButton(verticalLayoutWidget);
        saveButton->setObjectName(QStringLiteral("saveButton"));
        saveButton->setEnabled(false);

        verticalLayout->addWidget(saveButton);

        loadButton = new QPushButton(verticalLayoutWidget);
        loadButton->setObjectName(QStringLiteral("loadButton"));
        loadButton->setEnabled(false);

        verticalLayout->addWidget(loadButton);

        gameboard = new GameBoard(centralWidget);
        gameboard->setObjectName(QStringLiteral("gameboard"));
        gameboard->setGeometry(QRect(10, 30, 460, 460));
        gameboard->setMinimumSize(QSize(460, 460));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(510, 230, 151, 251));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        colorLabel = new QLabel(layoutWidget);
        colorLabel->setObjectName(QStringLiteral("colorLabel"));
        colorLabel->setStyleSheet(QLatin1String("font: 18pt \".Helvetica Neue DeskInterface\";\n"
"background-color: rgb(170, 89, 99);"));
        colorLabel->setLineWidth(3);

        verticalLayout_2->addWidget(colorLabel);

        countDown = new QLCDNumber(layoutWidget);
        countDown->setObjectName(QStringLiteral("countDown"));
        countDown->setMinimumSize(QSize(0, 30));
        countDown->setStyleSheet(QStringLiteral("background-color: rgb(76, 170, 83);"));
        countDown->setProperty("value", QVariant(60));

        verticalLayout_2->addWidget(countDown);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_2->addWidget(label_2);

        enemyTimeCount = new QLCDNumber(layoutWidget);
        enemyTimeCount->setObjectName(QStringLiteral("enemyTimeCount"));
        enemyTimeCount->setMinimumSize(QSize(0, 30));
        enemyTimeCount->setStyleSheet(QStringLiteral("background-color: rgb(255, 0, 0);"));

        verticalLayout_2->addWidget(enemyTimeCount);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Preferred);

        verticalLayout_2->addItem(verticalSpacer_2);

        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        playerTimeCount = new QLCDNumber(layoutWidget);
        playerTimeCount->setObjectName(QStringLiteral("playerTimeCount"));
        playerTimeCount->setMinimumSize(QSize(0, 30));
        playerTimeCount->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 255);"));

        verticalLayout_2->addWidget(playerTimeCount);

        Gomoku->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Gomoku);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 692, 23));
        Gomoku->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Gomoku);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Gomoku->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Gomoku);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Gomoku->setStatusBar(statusBar);

        retranslateUi(Gomoku);

        QMetaObject::connectSlotsByName(Gomoku);
    } // setupUi

    void retranslateUi(QMainWindow *Gomoku)
    {
        Gomoku->setWindowTitle(QApplication::translate("Gomoku", "Gomoku", 0));
        createButton->setText(QApplication::translate("Gomoku", "\345\210\233\345\273\272", 0));
        connectButton->setText(QApplication::translate("Gomoku", "\350\277\236\346\216\245", 0));
        undoButton->setText(QApplication::translate("Gomoku", "\346\202\224\346\243\213", 0));
        tipButton->setText(QApplication::translate("Gomoku", "\346\217\220\347\244\272", 0));
        quitButton->setText(QApplication::translate("Gomoku", "\350\256\244\350\276\223", 0));
        saveButton->setText(QApplication::translate("Gomoku", "\344\277\235\345\255\230", 0));
        loadButton->setText(QApplication::translate("Gomoku", "\350\257\273\345\217\226", 0));
        colorLabel->setText(QApplication::translate("Gomoku", "\347\255\211\345\276\205\350\277\236\346\216\245", 0));
        label_2->setText(QApplication::translate("Gomoku", "\346\225\214\346\226\271\347\224\250\346\227\266", 0));
        label->setText(QApplication::translate("Gomoku", "\346\210\221\346\226\271\347\224\250\346\227\266", 0));
    } // retranslateUi

};

namespace Ui {
    class Gomoku: public Ui_Gomoku {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GOMOKU_H
