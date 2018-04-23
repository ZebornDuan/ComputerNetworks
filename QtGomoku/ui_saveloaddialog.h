/********************************************************************************
** Form generated from reading UI file 'saveloaddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVELOADDIALOG_H
#define UI_SAVELOADDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_saveLoadDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QButtonGroup *buttonGroup;

    void setupUi(QDialog *saveLoadDialog)
    {
        if (saveLoadDialog->objectName().isEmpty())
            saveLoadDialog->setObjectName(QStringLiteral("saveLoadDialog"));
        saveLoadDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(saveLoadDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        widget = new QWidget(saveLoadDialog);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(80, 50, 241, 151));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        radioButton = new QRadioButton(widget);
        buttonGroup = new QButtonGroup(saveLoadDialog);
        buttonGroup->setObjectName(QStringLiteral("buttonGroup"));
        buttonGroup->addButton(radioButton);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setChecked(true);

        verticalLayout->addWidget(radioButton);

        radioButton_2 = new QRadioButton(widget);
        buttonGroup->addButton(radioButton_2);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        verticalLayout->addWidget(radioButton_2);

        radioButton_3 = new QRadioButton(widget);
        buttonGroup->addButton(radioButton_3);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        verticalLayout->addWidget(radioButton_3);


        retranslateUi(saveLoadDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), saveLoadDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), saveLoadDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(saveLoadDialog);
    } // setupUi

    void retranslateUi(QDialog *saveLoadDialog)
    {
        saveLoadDialog->setWindowTitle(QApplication::translate("saveLoadDialog", "Dialog", 0));
        radioButton->setText(QApplication::translate("saveLoadDialog", "1 none", 0));
        radioButton_2->setText(QApplication::translate("saveLoadDialog", "2 none", 0));
        radioButton_3->setText(QApplication::translate("saveLoadDialog", "3 none", 0));
    } // retranslateUi

};

namespace Ui {
    class saveLoadDialog: public Ui_saveLoadDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVELOADDIALOG_H
