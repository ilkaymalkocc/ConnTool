#include "openconnectiondialog.h"
#include "ui_dialog.h"
#include <QNetworkInterface>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)

{

    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces)
    {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            for (const QNetworkAddressEntry &entry : interface.addressEntries())
            {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    ui->networkInfoText->appendPlainText(
                        "Interface: " + interface.humanReadableName() +
                        "\nIP Address: " + entry.ip().toString() +
                        "\nSubnet Mask: " + entry.netmask().toString() +
                        "\nGateway (Guess): " + entry.broadcast().toString() +
                        "\n"
                    );
                }
            }
        }
    }
}


Dialog::~Dialog()
{
    delete ui;
    ui = nullptr;

}

QString Dialog::getIpAddress() const
{
    return ui->ipAddrText->text();
}

quint16 Dialog::getPort() const
{


    int tabIndex = ui->tabWidget->currentIndex();

    if (tabIndex == 0) { // TCP Server
        if (ui->portText_2) {
            QString portString = ui->portText_2->text();
            return portString.toUShort();
        }

    }
    else if (tabIndex == 1) { // TCP Client
        if (ui->portText) {
            QString portString = ui->portText->text();
            return portString.toUShort();
        }

    }

    return 0;
}

bool Dialog::isClientMode() const
{
    return ui->tabWidget->currentIndex() == 1;
}

bool Dialog::isServerMode() const
{
    return ui->tabWidget->currentIndex() == 2;
}
