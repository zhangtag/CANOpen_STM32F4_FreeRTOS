#include "can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "SillySlave.h"
#include "tim.h"
extern CAN_TxHeaderTypeDef TxHeader; //

extern osMessageQId CAN_RX_MSGHandle; //rx queue

void CAN_Config(void)
{
  CAN_FilterTypeDef sFilterConfig;

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
}


void CAN_RXmsg_process(void)
{
	static CANmsgTypeDef RxMsg;
	static Message msg;
  if (xQueueReceive(CAN_RX_MSGHandle, &RxMsg, portMAX_DELAY) == pdPASS)
  {
    msg.cob_id = (UNS16)RxMsg.Header.StdId; //CAN-ID

    if (CAN_RTR_REMOTE == RxMsg.Header.RTR)
      msg.rtr = 1; //
    else
      msg.rtr = 0; //

    msg.len = (UNS8)RxMsg.Header.DLC; //

    for (int i = 0; i < RxMsg.Header.DLC; i++) //
      msg.data[i] = RxMsg.Data[i];
		
		HAL_TIM_Base_Stop_IT(&htim13);
    canDispatch(&SillySlave_Data, &msg);
		HAL_TIM_Base_Start_IT(&htim13);
  }
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if (hcan->Instance == CAN1)
  {
		static CANmsgTypeDef CAN_Rx_msg;
		static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN_Rx_msg.Header, CAN_Rx_msg.Data) == HAL_OK)
    {
      xQueueSendToBackFromISR(CAN_RX_MSGHandle, &CAN_Rx_msg, &xHigherPriorityTaskWoken); //xQueueSendToBackFromISR
      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
  }
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
  if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &TxMailbox) != HAL_OK) //
  {
    return 1;
  }
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
  {
  }

  return 0;
}


u8 CAN1_Send_Msg(u8 *msg, u8 len)
{
  u8 i = 0;
  u32 TxMailbox = 0;
  u8 message[8];
  TxHeader.StdId = 0X12;
  TxHeader.ExtId = 0x12;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = len;
  for (i = 0; i < len; i++)
  {
    message[i] = msg[i];
  }
  if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &TxMailbox) != HAL_OK)
  {
    return 1;
  }
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
  {
  }
  return 0;
}
