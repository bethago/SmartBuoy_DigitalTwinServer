#ifndef CONFIG_H
#define CONFIG_H

#define CSE_SERVER_IP "127.0.0.1"
#define CSE_SERVER_PORT 3000
#define CSE_URL "http://127.0.0.1:3000"

#define TinyIoT
#ifdef TinyIoT
    #define X_M2M_ORIGIN "CAdmin"
    #define CSE_BASE "TinyIoT"
    #define X_M2M_RVI "3"
#endif

#define SUB_RESOURCE "/sensor_data"
#define SUB_NOTIFY_URL "http://127.0.0.1:10001/notify"

#define DT_SERVER_IP "127.0.0.1"
#define DT_SERVER_PORT 10001

#define VS_SERVER_IP "127.0.0.1"
#define VS_SERVER_PORT 10002

#endif //CONFIG_H
