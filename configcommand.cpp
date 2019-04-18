#include "configcommand.h"

ConfigCommand::ConfigCommand()
{
    dataArray.resize(CONFIG_COM_DATA_BUF_SIZE);
}

void ConfigCommand::transmitConfigCom(void)
{
    dataArray.clear();

    dataArray[0] = CONF_COM_PACKET_TOKEN;
    dataArray[1] = 0;
    dataArray[CONF_COM_CODE_BYTE] = static_cast<char>(command);
    dataArray[CONF_COM_DATA_SIZE_BYTE] = static_cast<char>(dataLength);

    for(int i = 0; i < dataLength; i++) {
        dataArray[CONF_COM_DATA_BYTES + i] = static_cast<char>(data[i]);
    }

    CRC_Calc(CRC_WRITE);

    dataArray.resize(dataLength + CONF_COM_SERVICE_DATA_LENGTH);
}

void ConfigCommand::setCommand(quint8 com)
{
    command = com;
}

quint8 ConfigCommand::getCommand()
{
    return command;
}

quint8 ConfigCommand::getDataLength()
{
    return dataLength;
}

const QByteArray &ConfigCommand::getDataArray()
{
    return dataArray;
}

void ConfigCommand::setDataLength(quint8 len)
{
    dataLength = len;
}

READ_CRC_TypeDef ConfigCommand::CRC_Calc(RW_CRC_TypeDef set)
{
    quint8 data[CONFIG_COM_DATA_BUF_SIZE];
    quint8 *pData = data;
    quint16 length = dataLength + CONF_COM_SERVICE_DATA_LENGTH;
    quint8 d;
    quint16 CRCValue;

    for(int i = 0; i < length; i++) {
        data[i] = static_cast<quint8>(dataArray[i]);
    }


    if(set == CRC_WRITE) {
        length -= 2;
    }

    CRCValue = 0xFFFF;

    do {
        d = *pData++ ^ (CRCValue & 0xFF);

        d ^= d << 4;

        CRCValue  = (d << 3) ^ (d << 8) ^ (CRCValue >> 8) ^ (d >> 4);
    }
    while(--length);

    if (set == CRC_WRITE) {                           // Store complement of CRC
        CRCValue ^= 0xFFFF;

        d = CRCValue & 0xFF;

        *pData = d;

        pData++;

        d = CRCValue >> 8;

        *pData = d;


        for(int i = 0; i < (dataLength + CONF_COM_SERVICE_DATA_LENGTH); i++) {
            dataArray[i] = static_cast<char>(data[i]);
        }

        return CRC_CORRECT;
    }
    else {
        if(CRCValue == 0xF0B8) {
            return CRC_CORRECT;
        }
        else {
            return CRC_INVALID;
        }
    }
}


