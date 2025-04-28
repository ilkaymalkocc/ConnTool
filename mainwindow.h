#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_Connection_triggered();
    void on_actionSave_Log_triggered();
    void on_actionAbout_triggered();
    void onSendButtonClicked();
    void onClearButtonClicked();
    void onDisconnectButtonClicked();
    void onNewConnection();
    void onConnected();
    void onReadyRead();
    void onClientDisconnected();
    void on_repeatButton_clicked();
    void on_repeatTimeout();

private:
    Ui::MainWindow *ui;
    QTcpServer *server = nullptr;
    QTcpSocket *clientSocket = nullptr;
    bool alreadyOpened = false;
    QTimer *repeatTimer;
    int repeatRemaining;
    void updateConnectionStatus(bool isConnected);
};

#endif // MAINWINDOW_H
