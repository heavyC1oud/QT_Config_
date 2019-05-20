#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QByteArray>
#include <QTimer>
#include "configcommand.h"

#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addTCPUDPMode();
    void addPorts(void);
    void enableButtons(void);
    void disableButtons(void);
    void sendConfigCom(ConfCom_TypeDef com, uint8_t len, uint8_t *data);
    void recConfigCom(void);

    void configComReadPortTCP(void);
    void configComReadPortUDP(void);
    void configComReadIPv4(void);
    void configComReadTCPUDPMode(void);
    void configComReadAPN(void);
    void configComReadSWver(void);
    void configComStart(void);
    void configComStop(void);
    void configComWritePortTCP(void);
    void configComWritePortUDP(void);
    void configComWriteIPv4(void);
    void configComWriteTCPUDPMode(void);
    void configComWriteAPN(void);


private slots:
    void on_selCOM_CB_activated(const QString &arg1);
    void on_openCom_PB_clicked();
    void on_connect_PB_clicked();
    void on_readPortTCP_PB_clicked();
    void readData(void);
    void endReceive(void);
    void sendConfigPing(void);
    void setConFailText(void);
    void on_readPortUDP_PB_clicked();
    void on_readAdrIPv4_PB_clicked();
    void on_readTCPUDPMode_PB_clicked();
    void on_readAPN_PB_clicked();
    void on_readSWver_PB_clicked();
    void on_writePortTCP_PB_clicked();
    void on_writePortUDP_PB_clicked();
    void on_writeAdrIPv4_PB_clicked();
    void on_writeTCPUDPMode_PB_clicked();
    void on_writeAPN_PB_clicked();
    void on_writeAdrIPv4_1_LE_editingFinished();
    void on_writeAdrIPv4_2_LE_editingFinished();
    void on_writeAdrIPv4_3_LE_editingFinished();
    void on_writeAdrIPv4_4_LE_editingFinished();

    void on_writePortTCP_LE_editingFinished();

    void on_writePortUDP_LE_editingFinished();

private:
    Ui::MainWindow *ui;

    QSerialPort *port;
    QSerialPortInfo *portInfo;
    ConfigCommand *configCom;
    QByteArray receiveData;
    QTimer *receiveTimer;
    QTimer *configPingTimer;
    QTimer *connectTimer;
    bool connectFlag = true;
};

#endif // MAINWINDOW_H
