#include "saveloaddialog.h"
#include "ui_saveloaddialog.h"
#include <QSettings>
#include <QDebug>

saveLoadDialog::saveLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saveLoadDialog) {
    ui->setupUi(this);
    ui->buttonGroup->setId(ui->radioButton, 1);
    ui->buttonGroup->setId(ui->radioButton_2, 2);
    ui->buttonGroup->setId(ui->radioButton_3, 3);
    QSettings settings;
    if (settings.contains("time1"))
        ui->radioButton->setText(settings.value("time1").value<QString>());

    if (settings.contains("time2"))
        ui->radioButton_2->setText(settings.value("time2").value<QString>());

    if (settings.contains("time3"))
        ui->radioButton_3->setText(settings.value("time1").value<QString>());
}

int saveLoadDialog::getResult() const {
    return ui->buttonGroup->checkedId();
}

saveLoadDialog::~saveLoadDialog() {
    delete ui;
}
