#include "createdialog.h"
#include "ui_createdialog.h"
#include <QNetworkInterface>
#include <QtNetwork/QHostAddress>

CreateDialog::CreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDialog) {
    QString ipAddress;
//    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

//    // use the first non-localhost IPv4 address
//    for (int i = 0; i < ipAddressesList.size(); ++i) {
//        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
//                ipAddressesList.at(i).toIPv4Address()) {
//            ipAddress = ipAddressesList.at(i).toString();
//            break;
//        }
//    }

//    // if we did not find one, use IPv4 localhost
//    if (ipAddress.isEmpty())//
    //ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
      //QHostAddress result;
      foreach(QHostAddress address, AddressList){
          if(address.protocol() == QAbstractSocket::IPv4Protocol &&
             address != QHostAddress::Null &&
             address != QHostAddress::LocalHost){
              if (address.toString().contains("127.0.")||address.toString().contains("169.254.")){
                continue;
              }
              ipAddress = address.toString();
              break;
          }
      }
      //return result;
//ipAddress = QHostAddress(QHostAddress::Any).toString();
    ui->setupUi(this);
    ui->lineEdit->setText(ipAddress);
//    ui->lineEdit->setText("183.172.162.88");
}

QString CreateDialog::getHostIpAddress() {
    return ui->lineEdit->text();
}

CreateDialog::~CreateDialog() {
    delete ui;
}
