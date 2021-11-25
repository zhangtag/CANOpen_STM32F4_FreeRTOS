#include "canfestival.h"

/*
 * Please tune the following defines to suit your needs:
 */
#define NODE_MASTER 1
#define NODE_SLAVE  4
#define BAUDRATE 1000


/* 定时器TIM相关变量 */
extern TIMEVAL last_counter_val;
extern TIMEVAL elapsed_time;

extern CO_Data *OD_Data;

void CO_slave_initialisation(CO_Data* d);
void CO_slave_preOperational(CO_Data* d);
void CO_slave_operational(CO_Data* d);
void CO_slave_stopped(CO_Data* d);

void CO_slave_post_sync(CO_Data* d);
void CO_slave_post_TPDO(CO_Data* d);
void CO_slave_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg, const UNS8 errSpec[5]);

void CO_slave_heartbeatError(CO_Data* d, UNS8);

UNS32 CO_slave_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex);
