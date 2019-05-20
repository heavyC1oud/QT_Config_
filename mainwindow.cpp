#include <QThread>
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configcommand.h"
#include "consts.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    port = new QSerialPort();
    portInfo = new QSerialPortInfo();
    configCom = new ConfigCommand();

    receiveTimer = new QTimer();
    receiveTimer->setSingleShot(true);
    receiveTimer->setInterval(UART_RECEIVE_END_DELAY_MS);

    configPingTimer = new QTimer();
    configPingTimer->setInterval(CONFIG_PING_INTERVAL_MS);

    connectTimer = new QTimer();
    connectTimer->setSingleShot(true);
    connectTimer->setInterval(CONNECT_DELAY_TIMER_MS);

    //  Logo
    QPixmap logoPM(":/img/Logo.png");
    ui->logo_LL->setPixmap(logoPM);

    //  Disable read line edits
    ui->readPortTCP_LE->setEnabled(false);
    ui->readPortUDP_LE->setEnabled(false);
    ui->readAdrIPv4_LE->setEnabled(false);
    ui->readTCPUDPMode_LE->setEnabled(false);
    ui->readAPN_LE->setEnabled(false);
    ui->readSWver_LE->setEnabled(false);
    ui->receiveData_TE->setEnabled(false);

    ui->openCom_PB->setEnabled(false);
    ui->connect_PB->setEnabled(false);

    addTCPUDPMode();
    addPorts();
    disableButtons();

    connect(port, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(receiveTimer, &QTimer::timeout, this, &MainWindow::endReceive);
    connect(configPingTimer, &QTimer::timeout, this, &MainWindow::sendConfigPing);
    connect(connectTimer, &QTimer::timeout, this, &MainWindow::setConFailText);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//  set combo box with TCP/UDP Mode
void MainWindow::addTCPUDPMode()
{
    ui->writeTCPUDPMode_CB->addItem("TCP");
    ui->writeTCPUDPMode_CB->addItem("UDP");
}

//  set combo box with available serial ports
void MainWindow::addPorts()
{
    foreach (QSerialPortInfo pInf, portInfo->availablePorts()) {
        ui->selCOM_CB->addItem(pInf.portName());
    }

    if(ui->selCOM_CB->count()) {
        ui->selCOM_CB->setCurrentIndex(0);
    }
    else {
        ui->selCOM_CB->addItem("");
        ui->selCOM_CB->setCurrentText("");
    }
}

void MainWindow::enableButtons()
{
    ui->readPortTCP_PB->setEnabled(true);
    ui->readPortUDP_PB->setEnabled(true);
    ui->readAdrIPv4_PB->setEnabled(true);
    ui->readTCPUDPMode_PB->setEnabled(true);
    ui->readAPN_PB->setEnabled(true);
    ui->readSWver_PB->setEnabled(true);
    ui->writePortTCP_PB->setEnabled(true);
    ui->writePortUDP_PB->setEnabled(true);
    ui->writeAdrIPv4_PB->setEnabled(true);
    ui->writeTCPUDPMode_PB->setEnabled(true);
    ui->writeAPN_PB->setEnabled(true);

    ui->writePortTCP_LE->setEnabled(true);
    ui->writePortUDP_LE->setEnabled(true);
    ui->writeAdrIPv4_1_LE->setEnabled(true);
    ui->writeAdrIPv4_2_LE->setEnabled(true);
    ui->writeAdrIPv4_3_LE->setEnabled(true);
    ui->writeAdrIPv4_4_LE->setEnabled(true);
    ui->writeTCPUDPMode_CB->setEnabled(true);
    ui->writeAPN_LE->setEnabled(true);
}

void MainWindow::disableButtons()
{
    ui->readPortTCP_PB->setEnabled(false);
    ui->readPortUDP_PB->setEnabled(false);
    ui->readAdrIPv4_PB->setEnabled(false);
    ui->readTCPUDPMode_PB->setEnabled(false);
    ui->readAPN_PB->setEnabled(false);
    ui->readSWver_PB->setEnabled(false);
    ui->writePortTCP_PB->setEnabled(false);
    ui->writePortUDP_PB->setEnabled(false);
    ui->writeAdrIPv4_PB->setEnabled(false);
    ui->writeTCPUDPMode_PB->setEnabled(false);
    ui->writeAPN_PB->setEnabled(false);

    ui->writePortTCP_LE->setEnabled(false);
    ui->writePortUDP_LE->setEnabled(false);
    ui->writeAdrIPv4_1_LE->setEnabled(false);
    ui->writeAdrIPv4_2_LE->setEnabled(false);
    ui->writeAdrIPv4_3_LE->setEnabled(false);
    ui->writeAdrIPv4_4_LE->setEnabled(false);
    ui->writeTCPUDPMode_CB->setEnabled(false);
    ui->writeAPN_LE->setEnabled(false);
}

//  send configuration command
void MainWindow::sendConfigCom(ConfCom_TypeDef com, quint8 len, quint8 *data)
{
    configCom->setCommand(com);
    configCom->setDataLength(len);

    configCom->setData(len, data);

    configCom->transmitConfigCom();

    receiveData.clear();
    port->write(configCom->getDataArray());
}

void MainWindow::recConfigCom()
{
    //
    switch(receiveData[CONF_COM_CODE_BYTE]) {
    case CONF_COM_READ_PORT_TCP:
        configComReadPortTCP();
        break;

    case CONF_COM_READ_PORT_UDP:
        configComReadPortUDP();
        break;

    case CONF_COM_READ_IP_V4:
        configComReadIPv4();
        break;

    case CONF_COM_READ_TCP_UDP_MODE:
        configComReadTCPUDPMode();
        break;

    case CONF_COM_READ_APN:
        configComReadAPN();
        break;

    case CONF_COM_READ_SW_VER:
        configComReadSWver();
        break;

    case CONF_COM_START_CONFIG:
        configComStart();
        break;

    case CONF_COM_STOP_CONFIG:
        configComStop();
        break;

    case CONF_COM_WRITE_PORT_TCP:
        configComWritePortTCP();
        break;

    case CONF_COM_WRITE_PORT_UDP:
        configComWritePortUDP();
        break;

    case CONF_COM_WRITE_IP_V4:
        configComWriteIPv4();
        break;

    case CONF_COM_WRITE_TCP_UDP_MODE:
        configComWriteTCPUDPMode();
        break;

    case CONF_COM_WRITE_APN:
        configComWriteAPN();
        break;

    default:
        break;
    }
}


//  set active serial port
void MainWindow::on_selCOM_CB_activated(const QString &arg1)
{
    foreach (QSerialPortInfo pInf, portInfo->availablePorts()) {
        if(pInf.portName() == arg1) {
            port->setPort(pInf);
            ui->openCom_PB->setEnabled(true);
            break;
        }
        else {
            ui->openCom_PB->setEnabled(false);
        }
    }
}

//  open/close serial port
void MainWindow::on_openCom_PB_clicked()
{
    bool portState = false;
    static bool openFlag = true;

    //  port open
    if(openFlag) {
        portState = port->open(QIODevice::ReadWrite);

        if(!portState) {
            ui->receiveData_TE->setText("Open Port Failed");
            return;
        }
        else {
            ui->receiveData_TE->setText("Open Port OK");
        }

        ui->openCom_PB->setText("CLOSE PORT");
        ui->selCOM_CB->setEnabled(false);
        ui->connect_PB->setEnabled(true);

//        enableButtons();

        openFlag = false;
    }
    //  port close
    else {
        port->close();

        ui->openCom_PB->setText("OPEN PORT");
        ui->selCOM_CB->setEnabled(true);
        ui->connect_PB->setEnabled(false);

        connectTimer->stop();
        ui->receiveData_TE->clear();

//        disableButtons();

        openFlag = true;
    }
}

//  connect to modem
void MainWindow::on_connect_PB_clicked()
{
    //  connect
    if(connectFlag) {
        sendConfigCom(CONF_COM_START_CONFIG, 0, nullptr);

        connectTimer->start();

        ui->receiveData_TE->setText("Connecting...");
    }
    //  disconnect
    else {
        ui->connect_PB->setText("CONNECT");

        disableButtons();
        ui->openCom_PB->setEnabled(true);

        sendConfigCom(CONF_COM_STOP_CONFIG, 0, nullptr);

        //  clear device data
        ui->readPortTCP_LE->clear();
        ui->readPortUDP_LE->clear();
        ui->readAdrIPv4_LE->clear();
        ui->readTCPUDPMode_LE->clear();
        ui->readAPN_LE->clear();
        ui->readSWver_LE->clear();
        ui->writePortTCP_LE->clear();
        ui->writePortUDP_LE->clear();
        ui->writeAdrIPv4_1_LE->clear();
        ui->writeAdrIPv4_2_LE->clear();
        ui->writeAdrIPv4_3_LE->clear();
        ui->writeAdrIPv4_4_LE->clear();
        ui->writeAPN_LE->clear();

        ui->receiveData_TE->clear();

        //  stop ping
        configPingTimer->stop();

        connectFlag = true;
    }
}

void MainWindow::readData(void)
{
    receiveTimer->start();
}

void MainWindow::endReceive()
{
    receiveData = port->readAll();

    recConfigCom();
}

void MainWindow::sendConfigPing()
{
    sendConfigCom(CONF_COM_PING, 0, nullptr);
}

void MainWindow::setConFailText()
{
    ui->receiveData_TE->setText("Connection Failed");
}

//  Configuration Commands
//  read TCP Port
void MainWindow::on_readPortTCP_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_PORT_TCP, 0, nullptr);

    ui->receiveData_TE->setText("Reading...");
}

//  read UDP Port
void MainWindow::on_readPortUDP_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_PORT_UDP, 0, nullptr);

    ui->receiveData_TE->setText("Reading...");
}

//  read Adr IPv4
void MainWindow::on_readAdrIPv4_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_IP_V4, 0, nullptr);

    ui->receiveData_TE->setText("Reading...");
}

//  read Mode - TCP/UDP
void MainWindow::on_readTCPUDPMode_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_TCP_UDP_MODE, 0, nullptr);

    ui->receiveData_TE->setText("Reading...");
}

//  read APN
void MainWindow::on_readAPN_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_APN, 0, nullptr);

    ui->receiveData_TE->setText("Reading...");
}

//  read Software version
void MainWindow::on_readSWver_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_SW_VER, 0, nullptr);

    ui->receiveData_TE->setText("Reading...");
}

//  write TCP Port
void MainWindow::on_writePortTCP_PB_clicked()
{
    quint8 port[2];
    qint16 data = static_cast<qint16>((ui->writePortTCP_LE->text()).toInt());

    port[0] = static_cast<quint8>(data >> 8);
    port[1] = static_cast<quint8>(data);

    sendConfigCom(CONF_COM_WRITE_PORT_TCP, 2, port);

    ui->receiveData_TE->setText("Writing...");
}

//  write UDP Port
void MainWindow::on_writePortUDP_PB_clicked()
{
    quint8 port[2];
    qint16 data = static_cast<qint16>((ui->writePortUDP_LE->text()).toInt());

    port[0] = static_cast<quint8>(data >> 8);
    port[1] = static_cast<quint8>(data);

    sendConfigCom(CONF_COM_WRITE_PORT_UDP, 2, port);

    ui->receiveData_TE->setText("Writing...");
}

//  write IP v4 address
void MainWindow::on_writeAdrIPv4_PB_clicked()
{
    quint8 adr[4] = {0};

    adr[0] = static_cast<quint8>((ui->writeAdrIPv4_1_LE->text()).toInt());
    adr[1] = static_cast<quint8>((ui->writeAdrIPv4_2_LE->text()).toInt());
    adr[2] = static_cast<quint8>((ui->writeAdrIPv4_3_LE->text()).toInt());
    adr[3] = static_cast<quint8>((ui->writeAdrIPv4_4_LE->text()).toInt());

    sendConfigCom(CONF_COM_WRITE_IP_V4, 4, adr);

    ui->receiveData_TE->setText("Writing...");
}

//  write mode - TCP/UDP
void MainWindow::on_writeTCPUDPMode_PB_clicked()
{
    quint8 mode = 0;
    QString str;

    str = ui->writeTCPUDPMode_CB->currentText();

    if(str == "TCP") {
        mode = MODE_TCP;
    }
    else if(str == "UDP") {
        mode = MODE_UDP;
    }
    else {
        mode = MODE_TCP;
    }

    sendConfigCom(CONF_COM_WRITE_TCP_UDP_MODE, 1, &mode);

    ui->receiveData_TE->setText("Writing...");
}

//  write APN
void MainWindow::on_writeAPN_PB_clicked()
{
    QString str = ui->writeAPN_LE->text();
    QByteArray array = str.toUtf8();

    quint8 data[CONFIG_COM_DATA_BUF_SIZE];

    for(int i = 0; i < str.length(); i++){
       data[i] = static_cast<quint8>(array[i]);
    }

    sendConfigCom(CONF_COM_WRITE_APN, static_cast<quint8>(str.length()), data);

    ui->receiveData_TE->setText("Writing...");
}

//  Configuration Answers
//  read TCP Port
void MainWindow::configComReadPortTCP()
{
    int data = 0;

    data = (receiveData[CONF_COM_DATA_BYTES] << 8) + receiveData[CONF_COM_DATA_BYTES + 1];

    ui->readPortTCP_LE->setText(QString::number(data));

    ui->receiveData_TE->setText("Read OK");
}

//  read UDP Port
void MainWindow::configComReadPortUDP()
{
    int data = 0;

    data = (receiveData[CONF_COM_DATA_BYTES] << 8) + receiveData[CONF_COM_DATA_BYTES + 1];

    ui->readPortUDP_LE->setText(QString::number(data));

    ui->receiveData_TE->setText("Read OK");
}

//  read IP v4 Address
void MainWindow::configComReadIPv4()
{
    quint8 ip1 = 0;
    quint8 ip2 = 0;
    quint8 ip3 = 0;
    quint8 ip4 = 0;
    QString str;

    ip1 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES]));
    ip2 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 1]));
    ip3 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 2]));
    ip4 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 3]));

    str = QString::number(ip1) + "." + QString::number(ip2) + "." + QString::number(ip3) + "." + QString::number(ip4);

    ui->readAdrIPv4_LE->setText(str);

    ui->receiveData_TE->setText("Read OK");
}

//  read mode - TCP/UDP
void MainWindow::configComReadTCPUDPMode()
{
    int data = 0;

    data = (receiveData[CONF_COM_DATA_BYTES]);

    if(data == MODE_TCP) {
        ui->readTCPUDPMode_LE->setText("TCP");
    }
    else if(data == MODE_UDP) {
        ui->readTCPUDPMode_LE->setText("UDP");
    }
    else {
        ui->readTCPUDPMode_LE->setText("Unknown Data");
    }

    ui->receiveData_TE->setText("Read OK");
}

//  read APN
void MainWindow::configComReadAPN()
{
    QString str;

    for(int i = 0; i < receiveData[CONF_COM_DATA_SIZE_BYTE]; i++) {
        str += receiveData[CONF_COM_DATA_BYTES + i];
    }

    ui->readAPN_LE->setText(str);

    ui->receiveData_TE->setText("Read OK");
}

//  read software version
void MainWindow::configComReadSWver()
{
    QString str;

    for(int i = 0; i < receiveData[CONF_COM_DATA_SIZE_BYTE]; i++) {
        str += receiveData[CONF_COM_DATA_BYTES + i];
    }

    ui->readSWver_LE->clear();
    ui->readSWver_LE->setText(str);

    ui->receiveData_TE->setText("Read OK");
}

//  start configuration mode in device
void MainWindow::configComStart()
{
    //  connect
    if(connectFlag) {
        ui->connect_PB->setText("DISCONNECT");

        enableButtons();
        ui->openCom_PB->setEnabled(false);

        //  start ping
        configPingTimer->start();

        connectFlag = false;
    }

    connectTimer->stop();

    ui->receiveData_TE->setText("Connect OK");
}

//  stop configuration mode in device
void MainWindow::configComStop()
{
    ui->receiveData_TE->setText("Disconnect OK");
    ui->receiveData_TE->append("Device Will Restart");

}

//  write TCP Port answer
void MainWindow::configComWritePortTCP()
{
    ui->receiveData_TE->setText("Write OK");
}

//  write UDP Port answer
void MainWindow::configComWritePortUDP()
{
    ui->receiveData_TE->setText("Write OK");
}

//  write IP v4 address answer
void MainWindow::configComWriteIPv4()
{
    ui->receiveData_TE->setText("Write OK");
}

//  write IP v6 address answer
void MainWindow::configComWriteTCPUDPMode()
{
    ui->receiveData_TE->setText("Write OK");
}

//  write APN answer
void MainWindow::configComWriteAPN()
{
    ui->receiveData_TE->setText("Write OK");
}

//  Edit IP address input data
void MainWindow::on_writeAdrIPv4_1_LE_editingFinished()
{
    int intData;

    intData = (ui->writeAdrIPv4_1_LE->text()).toInt();

    if((intData > 255) || (intData < 0)) {
        intData = 0;
    }

    ui->writeAdrIPv4_1_LE->setText(QString::number(intData));
}

//  Edit IP address input data
void MainWindow::on_writeAdrIPv4_2_LE_editingFinished()
{
    int intData;

    intData = (ui->writeAdrIPv4_2_LE->text()).toInt();

    if((intData > 255) || (intData < 0)) {
        intData = 0;
    }

    ui->writeAdrIPv4_2_LE->setText(QString::number(intData));
}

//  Edit IP address input data
void MainWindow::on_writeAdrIPv4_3_LE_editingFinished()
{
    int intData;

    intData = (ui->writeAdrIPv4_3_LE->text()).toInt();

    if((intData > 255) || (intData < 0)) {
        intData = 0;
    }

    ui->writeAdrIPv4_3_LE->setText(QString::number(intData));
}

//  Edit IP address input data
void MainWindow::on_writeAdrIPv4_4_LE_editingFinished()
{
    int intData;

    intData = (ui->writeAdrIPv4_4_LE->text()).toInt();

    if((intData > 255) || (intData < 0)) {
        intData = 0;
    }

    ui->writeAdrIPv4_4_LE->setText(QString::number(intData));
}

//  Edit TCP PORT input data
void MainWindow::on_writePortTCP_LE_editingFinished()
{
    int intData;

    intData = (ui->writePortTCP_LE->text()).toInt();

    if((intData > 65535) || (intData < 0)) {
        intData = 0;
    }

    ui->writePortTCP_LE->setText(QString::number(intData));
}

//  Edit UDP PORT input data
void MainWindow::on_writePortUDP_LE_editingFinished()
{
    int intData;

    intData = (ui->writePortUDP_LE->text()).toInt();

    if((intData > 65535) || (intData < 0)) {
        intData = 0;
    }

    ui->writePortUDP_LE->setText(QString::number(intData));
}
