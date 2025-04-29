#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openconnectiondialog.h"
#include "aboutdialog.h"

#include <QDateTime>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),
 ui(new Ui::MainWindow),
 server(nullptr),
 clientSocket(nullptr),
 alreadyOpened(false)

{
    repeatTimer = new QTimer(this);
    repeatTimer->setInterval(1000);
    repeatTimer->setSingleShot(false);

    ui->setupUi(this);
    ui->logText->setReadOnly(true);
    ui->sendButton->setEnabled(!ui->dataText->toPlainText().trimmed().isEmpty());
    ui->repeatButton->setEnabled(ui->spinBox->value() > 0);

    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,this, &MainWindow::onClearButtonClicked);
    connect(ui->disconnectButton, &QPushButton::clicked,this, &MainWindow::onDisconnectButtonClicked);
    connect(repeatTimer, &QTimer::timeout, this, &MainWindow::on_repeatTimeout);
    connect(ui->repeatButton, &QPushButton::clicked,this, &MainWindow::on_repeatButton_clicked);
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int val){
        ui->repeatButton->setEnabled(val > 0);
    });
    connect(ui->dataText, &QTextEdit::textChanged, this, [this](){
        bool hasText = !ui->dataText->toPlainText().trimmed().isEmpty();
        ui->sendButton->setEnabled(hasText);
    });

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_Connection_triggered()
{
    ui->actionOpen_Connection->setEnabled(false);

    if (alreadyOpened)
    {
        ui->logText->append("Connection already open! Disconnect first.");
        return;
    }

    Dialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->actionOpen_Connection->setEnabled(true);
        quint16 port      = dialog.getPort();
        QString ipAddress = dialog.getIpAddress();
        bool    isClient  = dialog.isClientMode();

        if (!isClient)
        {
            server = new QTcpServer(this);
            connect(server, &QTcpServer::newConnection,
                    this, &MainWindow::onNewConnection);

            if (!server->listen(QHostAddress::Any, port))
            {
                ui->logText->append("Server could not start: "
                                    + server->errorString());
                server->deleteLater();
                server = nullptr;
            } else {
                ui->logText->append("Server started on port: "
                                    + QString::number(port));
                alreadyOpened = true;
            }
        } else {
            clientSocket = new QTcpSocket(this);
            connect(clientSocket, &QTcpSocket::connected,
                    this, &MainWindow::onConnected);
            connect(clientSocket, &QTcpSocket::readyRead,
                    this, &MainWindow::onReadyRead);
            connect(clientSocket, &QTcpSocket::disconnected,
                    this, &MainWindow::onClientDisconnected);

            clientSocket->connectToHost(ipAddress, port);
        }

        activateWindow();
        raise();
    }
}
void MainWindow::on_repeatButton_clicked()
{
    repeatRemaining = ui->spinBox->value();
    if (repeatRemaining <= 0) return;
    ui->repeatButton->setText("Stop");
    repeatTimer->start();
    onSendButtonClicked();
    --repeatRemaining;
}
void MainWindow::on_repeatTimeout()
{
    if (repeatRemaining <= 0)
    {
        repeatTimer->stop();
        ui->repeatButton->setText("Repeat");
        ui->logText->append("[Repeater] Done");
        return;
    }
    onSendButtonClicked();
    --repeatRemaining;
}
void MainWindow::on_actionSave_Log_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save Log", QString(), "Text Files (*.txt)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->logText->append("File could not be opened: "
                            + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << ui->logText->toPlainText();
    file.close();
    ui->logText->append("Log saved to: " + fileName);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onSendButtonClicked()
{
    if (!clientSocket || !clientSocket->isOpen()) {
        ui->logText->append("Client socket not open, cannot send message.");
        return;
    }

    QString message = ui->dataText->toPlainText();
    if (message.isEmpty()) {
        ui->logText->append("Message is empty!");
        return;
    }

    QByteArray data;
    QString mode = ui->dataComboBox->currentText();

    if (mode == "Hex")
    {
        message = message.simplified();
        QStringList hexList = message.split(' ');
        for (const QString &hexByte : hexList) {
            bool ok;
            char byte = hexByte.toUInt(&ok, 16);
            if (ok)
                data.append(byte);
            else
                ui->logText->append("Invalid HEX byte: " + hexByte);
        }
    } else
    {
        data = message.toUtf8();
    }

    clientSocket->write(data);
    clientSocket->flush();

    QString timestamp = QDateTime::currentDateTime()
                        .toString("yyyy-MM-dd HH:mm:ss");
    ui->logText->append("[" + timestamp + "] Sent: " + message);
       if (!repeatTimer->isActive())
           ui->dataText->clear();
}
void MainWindow::onClearButtonClicked()
{
    ui->logText->clear();
}

void MainWindow::onDisconnectButtonClicked()
{
    if (!alreadyOpened)
    {
        ui->logText->append("No active connection to disconnect.");
        return;
    }
    if (clientSocket)
    {
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }
    if (server) {
        server->close();
        server->deleteLater();
        server = nullptr;
    }

    alreadyOpened = false;
    ui->actionOpen_Connection->setEnabled(true);

    QString timestamp = QDateTime::currentDateTime()
                        .toString("yyyy-MM-dd HH:mm:ss");
    ui->logText->append("[" + timestamp + "] Connection manually closed!");
}

void MainWindow::onNewConnection()
{
    clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead,
            this, &MainWindow::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected,
            this, &MainWindow::onClientDisconnected);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->logText->append("[" + timestamp + "] New connection opened!");
}

void MainWindow::onConnected()
{
    alreadyOpened = true;
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->logText->append("[" + timestamp + "] Connected to server!");
}

void MainWindow::onReadyRead()
{
    QByteArray data = clientSocket->readAll();
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString mode = ui->dataComboBox->currentText();
    QString formattedData;
    if (mode == "Hex")
        formattedData = QString(data.toHex(' ')).toUpper();
    else
        formattedData = QString::fromUtf8(data);

    ui->logText->append("[" + timestamp + "] Incoming data: " + formattedData);

    QTextCursor cursor = ui->logText->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->logText->setTextCursor(cursor);
}

void MainWindow::onClientDisconnected()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->logText->append("[" + timestamp + "] Connection closed!");

    if (clientSocket)
    {
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }
    if (server) {
        server->close();
        server->deleteLater();
        server = nullptr;
    }

    alreadyOpened = false;
    ui->actionOpen_Connection->setEnabled(true);
}
