#ifndef CONFIG_H
#define CONFIG_H

#define DT_SERVER_IP "127.0.0.1"
#define DT_SERVER_PORT 10001

#define CSE_SERVER_IP "127.0.0.1"
#define CSE_SERVER_PORT 3000
#define TinyIoT
#ifdef TinyIoT
    #define X_M2M_ORIGIN "CAdmin"
    #define CSE_BASE "TinyIoT"
    #define X_M2M_RVI "3"
#endif

#define VS_SERVER_IP "127.0.0.1"
#define VS_SERVER_PORT 10002

#endif //CONFIG_H
