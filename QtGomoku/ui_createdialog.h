/********************************************************************************
** Form generated from reading UI file 'createdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEDIALOG_H
#define UI_CREATEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CreateDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;

    void setupUi(QDialog *CreateDialog)
    {
        if (CreateDialog->objectName().isEmpty())
            CreateDialog->setObjectName(QStringLiteral("CreateDialog"));
        CreateDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(CreateDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(CreateDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(40, 110, 301, 23));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(layoutWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEdit->setReadOnly(true);

        horizontalLayout->addWidget(lineEdit);


        retranslateUi(CreateDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CreateDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CreateDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CreateDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateDialog)
    {
        CreateDialog->setWindowTitle(QApplication::translate("CreateDialog", "Dialog", 0));
        label->setText(QApplication::translate("CreateDialog", "\346\234\254\346\234\272IP", 0));
    } // retranslateUi

};

namespace Ui {
    class CreateDialog: public Ui_CreateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEDIALOG_H
