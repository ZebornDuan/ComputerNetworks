#ifndef SAVELOADDIALOG_H
#define SAVELOADDIALOG_H


#include <QDialog>

namespace Ui {
class saveLoadDialog;
}

class saveLoadDialog : public QDialog {
    Q_OBJECT

public:
    explicit saveLoadDialog(QWidget *parent = 0);
    int getResult() const;
    ~saveLoadDialog();

private:
    Ui::saveLoadDialog *ui;
};


#endif // SAVELOADDIALOG_H
