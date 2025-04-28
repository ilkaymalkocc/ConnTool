#ifndef OPENCONNECTIONDIALOG_H
#define OPENCONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    QString getIpAddress() const;
    quint16 getPort() const;
    bool isClientMode() const;
    bool isServerMode() const;

private:
    Ui::Dialog *ui;
};

#endif // OPENCONNECTIONDIALOG_H
