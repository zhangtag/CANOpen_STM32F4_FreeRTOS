#include "SillySlave.h"
#include "CO_slave.h"
#include "can.h"
#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"
CO_Data *OD_Data = &SillySlave_Data;

void CO_slave_initialisation(CO_Data *d)
{
    UNS32 PDO1_COBID = 0x0180 + NODE_MASTER;
    UNS32 size = sizeof(PDO1_COBID);

    printf("CO_slave_initialisation\n");

    /* sets TXPDO1 COB-ID to match master node ID */
    writeLocalDict(
        OD_Data,     /*CO_Data* d*/
        0x1800,      /*UNS16 index*/
        0x01,        /*UNS8 subind*/
        &PDO1_COBID, /*void * pSourceData,*/
        &size,       /* UNS32 * pExpectedSize*/
        RW);         /* UNS8 checkAccess */

    /* value sent to master at each SYNC received */
    LifeSignal = 0;
}

void CO_slave_preOperational(CO_Data *d)
{
    printf("CO_slave_preOperational\n");
}

void CO_slave_operational(CO_Data *d)
{
    printf("CO_slave_operational\n");
}

void CO_slave_stopped(CO_Data *d)
{
    printf("CO_slave_stopped\n");
}

void CO_slave_post_sync(CO_Data *d)
{
    //    printf("CO_slave_post_sync: \n");
    //    LifeSignal++;
}

void CO_slave_post_TPDO(CO_Data *d)
{
    //    printf("CO_slave_post_TPDO: \n");
    //    printf("LifeSignal = %u\n", LifeSignal);
}

void CO_slave_post_emcy(CO_Data *d, UNS8 nodeID, UNS16 errCode, UNS8 errReg, const UNS8 errSpec[5])
{
    //    printf("Slave received EMCY message. Node: %2.2xh  ErrorCode: %4.4x  ErrorRegister: %2.2xh\n", nodeID, errCode, errReg);
}

void CO_slave_heartbeatError(CO_Data *d, UNS8 heartbeatID)
{
    //    printf("CO_slave_heartbeatError %d\n", heartbeatID);
}

UNS32 CO_slave_storeODSubIndex(CO_Data *d, UNS16 wIndex, UNS8 bSubindex)
{
    /*TODO : 
     * - call getODEntry for index and subindex, 
     * - save content to file, database, flash, nvram, ...
     * 
     * To ease flash organisation, index of variable to store
     * can be established by scanning d->objdict[d->ObjdictSize]
     * for variables to store.
     * 
     * */
    //    printf("CO_slave_storeODSubIndex : %4.4x %2.2xh\n", wIndex,  bSubindex);
	return 0;
}


TIMEVAL last_counter_val = 0;
TIMEVAL elapsed_time = 0;

void setTimer(TIMEVAL value)
{
    uint32_t timer = __HAL_TIM_GET_COUNTER(&htim12); // Copy the value of the running timer
    elapsed_time += timer - last_counter_val;
    last_counter_val = htim12.Init.Period - value;
    __HAL_TIM_SET_COUNTER(&htim12, htim12.Init.Period - value);
    HAL_TIM_Base_Start_IT(&htim12);
	
    //	__HAL_TIM_SET_AUTORELOAD(&htim12,value);
}
TIMEVAL getElapsedTime(void)
{
    uint32_t timer = __HAL_TIM_GET_COUNTER(&htim12); // Copy the value of the running timer
	
    if (timer < last_counter_val)
        timer += htim12.Init.Period;

    TIMEVAL elapsed = timer - last_counter_val + elapsed_time;

    return elapsed;
}
unsigned char canSend(CAN_PORT port, Message *m)
{
    u8 i = 0;
    u32 TxMailbox = 0;
    u8 message[8];
    TxHeader.StdId = m->cob_id; //
    TxHeader.ExtId = 0;         //
    TxHeader.IDE = CAN_ID_STD;  
    TxHeader.RTR = m->rtr;      
    TxHeader.DLC = m->len;
    for (i = 0; i < m->len; i++)
    {
        message[i] = m->data[i];
    }
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &TxMailbox) != HAL_OK) //����
    {
        return 1;
    }
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
    {
    }

    return 0;
}

//void _CO_slave_init(CO_Data* NOT_USED1, UNS32 NOT_USED2)
//{
//    setNodeId(OD_Data, NODE_SLAVE);
//    setState(OD_Data, Initialisation);
//}

int canopen_init(void)
{
    //static s_BOARD board  = {"0", "1M"};

    OD_Data->heartbeatError = CO_slave_heartbeatError;
    OD_Data->initialisation = CO_slave_initialisation;
    OD_Data->preOperational = CO_slave_preOperational;
    OD_Data->operational = CO_slave_operational;
    OD_Data->stopped = CO_slave_stopped;
    OD_Data->post_sync = CO_slave_post_sync;
    OD_Data->post_TPDO = CO_slave_post_TPDO;
    OD_Data->storeODSubIndex = (void *)CO_slave_storeODSubIndex;
    OD_Data->post_emcy = (void *)CO_slave_post_emcy;

    //canOpen(&board, OD_Data);

    ///TimerInit();

    //StartTimerLoop(&_CO_slave_init);

    setNodeId(OD_Data, NODE_SLAVE);
    setState(OD_Data, Initialisation);

    return 0;
}
