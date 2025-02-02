#ifndef CONFIG_H
#define CONFIG_H

#define CLIENT "BUOY1"
#define GRP_RI "grp_BUOY1"

#define CSE_SERVER_PROTOCOL "http"
#define CSE_SERVER_IP "127.0.0.1"
#define CSE_SERVER_PORT 3000
#define CSE_BASE "TinyIoT"
#define CSE_URL CSE_SERVER_PROTOCOL "://" CSE_SERVER_IP ":" CSE_SERVER_PORT

#define X_M2M_ORIGIN "CAdmin"
#define X_M2M_RVI "3"
#define SUB_RESOURCE "/sensor_data"
#define SUB_NOTIFY_URL CSE_SERVER_PROTOCOL "://" DT_SERVER_IP ":" DT_SERVER_PORT "/notify"

#define DT_SERVER_IP "127.0.0.1"
#define DT_SERVER_PORT 10001

#define VS_SERVER_IP "127.0.0.1"
#define VS_SERVER_PORT 10002

#endif // CONFIG_H
