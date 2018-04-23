#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <QPushButton>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog) {
    ui->setupUi(this);
    // Debug
    ui->lineEdit->setText("");

    for (int i = 1; i <= 12; i++) {
        QPushButton *button = new QPushButton(this);
        ui->gridLayout->addWidget(button, (i - 1) / 3, (i - 1) % 3);

        switch (i) {
        case 10:
            button->setText("AC");
            break;

        case 11:
            button->setText("0");
            break;

        case 12:
            button->setText(".");
            break;

        default:
            button->setText(QString::number(i));
        }

        connect(button, &QPushButton::clicked, [=]() {
            switch (i) {
            case 10:
                ui->lineEdit->setText("");
                break;

            case 11:
                ui->lineEdit->setText(ui->lineEdit->text() + "0");
                break;

            case 12:
                ui->lineEdit->setText(ui->lineEdit->text() + ".");
                break;

            default:
                ui->lineEdit->setText(ui->lineEdit->text() + QString::number(i));
            }
        });
    }
}

QString ConnectDialog::getHostIp() const {
    return ui->lineEdit->text();
}

ConnectDialog::~ConnectDialog() {
    delete ui;
}
