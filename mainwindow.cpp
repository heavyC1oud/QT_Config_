#include <QThread>
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configcommand.h"
#include "consts.h"

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

    //  Logo
    QPixmap logoPM(":/img/Logo.png");
    ui->logo_LL->setPixmap(logoPM);

    //  Disable read line edits
    ui->readPortTCP_LE->setEnabled(false);
    ui->readPortUDP_LE->setEnabled(false);
    ui->readAdrIPv4_LE->setEnabled(false);
    ui->readAdrIPv6_LE->setEnabled(false);
    ui->readIPver_LE->setEnabled(false);
    ui->readAPN_LE->setEnabled(false);
    ui->readSWver_LE->setEnabled(false);
    ui->receiveData_TE->setEnabled(false);

    ui->openCom_PB->setEnabled(false);
    ui->connect_PB->setEnabled(false);

    addIPver();
    addPorts();
    disableButtons();

    connect(port, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(receiveTimer, &QTimer::timeout, this, &MainWindow::endReceive);
    connect(configPingTimer, &QTimer::timeout, this, &MainWindow::sendConfigPing);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//  set combo box with IP v4/v6
void MainWindow::addIPver()
{
    ui->writeIPver_CB->addItem("IPv4");
    ui->writeIPver_CB->addItem("IPv6");
}

//  set combo box with available serial ports
void MainWindow::addPorts()
{
    foreach (QSerialPortInfo pInf, portInfo->availablePorts()) {
        ui->selCOM_CB->addItem(pInf.portName());
    }

    ui->selCOM_CB->addItem("");
    ui->selCOM_CB->setCurrentText("");
}

void MainWindow::enableButtons()
{
    ui->readPortTCP_PB->setEnabled(true);
    ui->readPortUDP_PB->setEnabled(true);
    ui->readAdrIPv4_PB->setEnabled(true);
    ui->readAdrIPv6_PB->setEnabled(true);
    ui->readIPver_PB->setEnabled(true);
    ui->readAPN_PB->setEnabled(true);
    ui->readSWver_PB->setEnabled(true);
    ui->writePortTCP_PB->setEnabled(true);
    ui->writePortUDP_PB->setEnabled(true);
    ui->writeAdrIPv4_PB->setEnabled(true);
    ui->writeAdrIPv6_PB->setEnabled(true);
    ui->writeIPver_PB->setEnabled(true);
    ui->writeAPN_PB->setEnabled(true);

    ui->writePortTCP_LE->setEnabled(true);
    ui->writePortUDP_LE->setEnabled(true);
    ui->writeAdrIPv4_1_LE->setEnabled(true);
    ui->writeAdrIPv4_2_LE->setEnabled(true);
    ui->writeAdrIPv4_3_LE->setEnabled(true);
    ui->writeAdrIPv4_4_LE->setEnabled(true);
    ui->writeAdrIPv6_1_LE->setEnabled(true);
    ui->writeAdrIPv6_2_LE->setEnabled(true);
    ui->writeAdrIPv6_3_LE->setEnabled(true);
    ui->writeAdrIPv6_4_LE->setEnabled(true);
    ui->writeAdrIPv6_5_LE->setEnabled(true);
    ui->writeAdrIPv6_6_LE->setEnabled(true);
    ui->writeIPver_CB->setEnabled(true);
    ui->writeAPN_LE->setEnabled(true);
}

void MainWindow::disableButtons()
{
    ui->readPortTCP_PB->setEnabled(false);
    ui->readPortUDP_PB->setEnabled(false);
    ui->readAdrIPv4_PB->setEnabled(false);
    ui->readAdrIPv6_PB->setEnabled(false);
    ui->readIPver_PB->setEnabled(false);
    ui->readAPN_PB->setEnabled(false);
    ui->readSWver_PB->setEnabled(false);
    ui->writePortTCP_PB->setEnabled(false);
    ui->writePortUDP_PB->setEnabled(false);
    ui->writeAdrIPv4_PB->setEnabled(false);
    ui->writeAdrIPv6_PB->setEnabled(false);
    ui->writeIPver_PB->setEnabled(false);
    ui->writeAPN_PB->setEnabled(false);

    ui->writePortTCP_LE->setEnabled(false);
    ui->writePortUDP_LE->setEnabled(false);
    ui->writeAdrIPv4_1_LE->setEnabled(false);
    ui->writeAdrIPv4_2_LE->setEnabled(false);
    ui->writeAdrIPv4_3_LE->setEnabled(false);
    ui->writeAdrIPv4_4_LE->setEnabled(false);
    ui->writeAdrIPv6_1_LE->setEnabled(false);
    ui->writeAdrIPv6_2_LE->setEnabled(false);
    ui->writeAdrIPv6_3_LE->setEnabled(false);
    ui->writeAdrIPv6_4_LE->setEnabled(false);
    ui->writeAdrIPv6_5_LE->setEnabled(false);
    ui->writeAdrIPv6_6_LE->setEnabled(false);
    ui->writeIPver_CB->setEnabled(false);
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

    case CONF_COM_READ_IP_V6:
        configComReadIPv6();
        break;

    case CONF_COM_READ_IP_VER:
        configComReadIPver();
        break;

    case CONF_COM_READ_APN:
        configComReadAPN();
        break;

    case CONF_COM_READ_SW_VER:
        configComReadSWver();
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
            ui->receiveData_TE->clear();
            ui->receiveData_TE->setText("Open Port Failed");
            return;
        }
        else {
            ui->receiveData_TE->clear();
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

//        disableButtons();

        openFlag = true;
    }
}

//  connect to modem
void MainWindow::on_connect_PB_clicked()
{
    static bool connectFlag = true;

    //  connect
    if(connectFlag) {
        ui->connect_PB->setText("DISCONNECT");

        enableButtons();
        ui->openCom_PB->setEnabled(false);

        sendConfigCom(CONF_COM_START_CONFIG, 0, nullptr);

        //  start ping
        configPingTimer->start();

        connectFlag = false;
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
        ui->readAdrIPv6_LE->clear();
        ui->readIPver_LE->clear();
        ui->readAPN_LE->clear();
        ui->readSWver_LE->clear();

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

    qDebug() << receiveData.toHex();
    qDebug() << receiveData;
}

void MainWindow::sendConfigPing()
{
    sendConfigCom(CONF_COM_PING, 0, nullptr);
}

//  Configuration Commands
//  read TCP Port
void MainWindow::on_readPortTCP_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_PORT_TCP, 0, nullptr);
}

//  read UDP Port
void MainWindow::on_readPortUDP_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_PORT_UDP, 0, nullptr);
}

//  read Adr IPv4
void MainWindow::on_readAdrIPv4_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_IP_V4, 0, nullptr);
}

//  read Adr IPv6
void MainWindow::on_readAdrIPv6_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_IP_V6, 0, nullptr);
}

//  read IP ver
void MainWindow::on_readIPver_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_IP_VER, 0, nullptr);
}

//  read APN
void MainWindow::on_readAPN_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_APN, 0, nullptr);
}

//  read Software version
void MainWindow::on_readSWver_PB_clicked()
{
    sendConfigCom(CONF_COM_READ_SW_VER, 0, nullptr);
}

//  write TCP Port
void MainWindow::on_writePortTCP_PB_clicked()
{
    quint8 port[2];
    qint16 data = static_cast<qint16>((ui->writePortTCP_LE->text()).toInt());

    port[0] = static_cast<quint8>(data >> 8);
    port[1] = static_cast<quint8>(data);

    sendConfigCom(CONF_COM_WRITE_PORT_TCP, 2, port);
}

//  write UDP Port
void MainWindow::on_writePortUDP_PB_clicked()
{
    quint8 port[2];
    qint16 data = static_cast<qint16>((ui->writePortUDP_LE->text()).toInt());

    port[0] = static_cast<quint8>(data >> 8);
    port[1] = static_cast<quint8>(data);

    sendConfigCom(CONF_COM_WRITE_PORT_UDP, 2, port);
}

//  write IP v4 address
void MainWindow::on_writeAdrIPv4_PB_clicked()
{
    quint8 adr[4] = {0};

    adr[0] = static_cast<quint8>((ui->writeAdrIPv4_1_LE->text()).toInt());
    adr[1] = static_cast<quint8>((ui->writeAdrIPv4_2_LE->text()).toInt());
    adr[2] = static_cast<quint8>((ui->writeAdrIPv4_3_LE->text()).toInt());
    adr[3] = static_cast<quint8>((ui->writeAdrIPv4_4_LE->text()).toInt());

//    QString str = ui->writeAdrIPv4_LE->text();
//    QStringList list = str.split('.');

//    foreach (QString s, list) {
//        adr[i] = static_cast<quint8>(s.toInt());
//        i++;
//        if(i > 3) i = 0;
//    }

    sendConfigCom(CONF_COM_WRITE_IP_V4, 4, adr);
}

//  write IP v6 address
void MainWindow::on_writeAdrIPv6_PB_clicked()
{
    quint8 adr[6] = {0};

    adr[0] = static_cast<quint8>((ui->writeAdrIPv6_1_LE->text()).toInt());
    adr[1] = static_cast<quint8>((ui->writeAdrIPv6_2_LE->text()).toInt());
    adr[2] = static_cast<quint8>((ui->writeAdrIPv6_3_LE->text()).toInt());
    adr[3] = static_cast<quint8>((ui->writeAdrIPv6_4_LE->text()).toInt());
    adr[4] = static_cast<quint8>((ui->writeAdrIPv6_5_LE->text()).toInt());
    adr[5] = static_cast<quint8>((ui->writeAdrIPv6_6_LE->text()).toInt());

//    QString str = ui->writeAdrIPv6_LE->text();
//    QStringList list = str.split('.');

//    foreach (QString s, list) {
//        adr[i] = static_cast<quint8>(s.toInt());
//        i++;
//        if(i > 5) i = 0;
//    }

    sendConfigCom(CONF_COM_WRITE_IP_V6, 6, adr);
}

//  write IP version
void MainWindow::on_writeIPver_PB_clicked()
{
    QString str;
    quint8 data;

    str = ui->writeIPver_CB->currentText();

    if(str == "IPv4") {
        data = IP_V4;
        sendConfigCom(CONF_COM_WRITE_IP_VER, 1, &data);
    }
    else if(str == "IPv6") {
        data = IP_V6;
        sendConfigCom(CONF_COM_WRITE_IP_VER, 1, &data);
    }
}

//  Configuration Answers
//  read TCP Port
void MainWindow::configComReadPortTCP()
{
    int data = 0;

    data = (receiveData[CONF_COM_DATA_BYTES] << 8) + receiveData[CONF_COM_DATA_BYTES + 1];

    ui->readPortTCP_LE->clear();
    ui->readPortTCP_LE->setText(QString::number(data));
}

//  read UDP Port
void MainWindow::configComReadPortUDP()
{
    int data = 0;

    data = (receiveData[CONF_COM_DATA_BYTES] << 8) + receiveData[CONF_COM_DATA_BYTES + 1];

    ui->readPortUDP_LE->clear();
    ui->readPortUDP_LE->setText(QString::number(data));
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

    ui->readAdrIPv4_LE->clear();
    ui->readAdrIPv4_LE->setText(str);
}

//  read IP v6 Address
void MainWindow::configComReadIPv6()
{
    quint8 ip1 = 0;
    quint8 ip2 = 0;
    quint8 ip3 = 0;
    quint8 ip4 = 0;
    quint8 ip5 = 0;
    quint8 ip6 = 0;
    QString str;

    ip1 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES]));
    ip2 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 1]));
    ip3 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 2]));
    ip4 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 3]));
    ip5 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 4]));
    ip6 = static_cast<quint8>((receiveData[CONF_COM_DATA_BYTES + 5]));

    str = QString::number(ip1) + "." + QString::number(ip2) + "." + QString::number(ip3) + "." + QString::number(ip4) + "." + QString::number(ip5) + "." + QString::number(ip6);

    ui->readAdrIPv6_LE->clear();
    ui->readAdrIPv6_LE->setText(str);
}

//  read IP version
void MainWindow::configComReadIPver()
{
    int data = 0;

    data = (receiveData[CONF_COM_DATA_BYTES]);

    if(data == IP_V4) {
        ui->readIPver_LE->clear();
        ui->readIPver_LE->setText("IPv4");
    }
    else if(data == IP_V6) {
        ui->readIPver_LE->clear();
        ui->readIPver_LE->setText("IPv6");
    }
    else {
        ui->readIPver_LE->clear();
        ui->readIPver_LE->setText("Error");
    }
}

//  read APN
void MainWindow::configComReadAPN()
{
    QString str;

    for(int i = 0; i < receiveData[CONF_COM_DATA_SIZE_BYTE]; i++) {
        str += receiveData[CONF_COM_DATA_BYTES + i];
    }

    ui->readAPN_LE->clear();
    ui->readAPN_LE->setText(str);
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
}
