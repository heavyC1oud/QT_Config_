#ifndef CONFIGCOMMAND_H
#define CONFIGCOMMAND_H

#include "stdint.h"
#include "consts.h"
#include <QtGlobal>
#include <QByteArray>

#include <QDebug>

typedef enum {
    CRC_READ,
    CRC_WRITE,
} RW_CRC_TypeDef;

typedef enum {
    CRC_INVALID,
    CRC_CORRECT,
} READ_CRC_TypeDef;


class ConfigCommand
{
public:
    ConfigCommand();

    void transmitConfigCom(void);
    void setCommand(quint8 com);
    quint8 getCommand(void);
    void setDataLength(quint8 len);
    quint8 getDataLength(void);
    const QByteArray &getDataArray(void);
    READ_CRC_TypeDef CRC_Calc(RW_CRC_TypeDef set);


private:
    quint8 command;
    quint8 dataLength;
    quint8 data[CONFIG_COM_DATA_BUF_SIZE];

    QByteArray dataArray;
};



#endif // CONFIGCOMMAND_H
