#ifndef CONSTS_H
#define CONSTS_H

const int UART_RECEIVE_END_DELAY_MS = 50;
const int ANSWER_TIMEOUT_DELAY_MS = 500;
const int CONFIG_PING_INTERVAL_MS = 15000;
const int CONNECT_DELAY_TIMER_MS = 40000;

const int CONF_COM_PACKET_TOKEN = 0x04;									//	признак пакета конфигурации модема
const int CONF_COM_CODE_BYTE = 2;										//	порядковый номер байта, содержащего код команды
const int CONF_COM_DATA_SIZE_BYTE = (CONF_COM_CODE_BYTE + 1);			//	порядковый номер байта, содержащего количество байт - данных команды
const int CONF_COM_DATA_BYTES = (CONF_COM_DATA_SIZE_BYTE + 1);			//	порядковый номер байта, содержащего первый байт передаваемых данных
const int CONF_COM_SERVICE_DATA_LENGTH = 6;								//	количество байт сервисных данных

const int CONFIG_COM_DATA_BUF_SIZE = 256;


typedef enum {
    CONF_COM_UNKNOWN = 0,
    CONF_COM_READ_CSQ = 1,
    CONF_COM_READ_PORT_TCP = 2,
    CONF_COM_WRITE_PORT_TCP = 3,
    CONF_COM_READ_PORT_UDP = 4,
    CONF_COM_WRITE_PORT_UDP = 5,
    CONF_COM_READ_IP_V4 = 6,
    CONF_COM_WRITE_IP_V4 = 7,
    CONF_COM_READ_TCP_UDP_MODE = 8,
    CONF_COM_WRITE_TCP_UDP_MODE = 9,
    CONF_COM_READ_APN = 10,
    CONF_COM_READ_SW_VER = 11,
    CONF_COM_START_CONFIG = 12,
    CONF_COM_STOP_CONFIG = 13,
    CONF_COM_PING = 14,
    CONF_COM_READ_ALL_DATA = 15,
    CONF_COM_WRITE_APN = 16,
} ConfCom_TypeDef;

typedef enum {
    MODE_TCP = 0,
    MODE_UDP = 1,
} TCP_UDP_Mode_Typedef;

#endif // CONSTS_H
