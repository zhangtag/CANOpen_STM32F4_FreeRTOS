#include "SillySlave.h"
#include "CO_slave.h"
#include "can.h"
#include "main.h"
#include "tim.h"

unsigned int TimeCNT = 0;			  //时间计数
unsigned int NextTime = 0;			  //下一次触发时间计数
unsigned int TIMER_MAX_COUNT = 70000; //最大时间计数
TIMEVAL last_time_set = TIMEVAL_MAX;  //上一次的时间计数

/**
 * Set the timer for the next alarm.
 * @param   value TIMEVAL (unsigned long)
 * @return  void
 */
void setTimer(TIMEVAL value)
{
	NextTime = (TimeCNT + value) % TIMER_MAX_COUNT;
}
/**
 * Return the elapsed time to tell the stack how much time is spent since last call.
 * @return  TIMEVAL (unsigned long) the elapsed time
 */
TIMEVAL getElapsedTime(void)
{
	int ret = 0;
	ret = TimeCNT > last_time_set ? TimeCNT - last_time_set : TimeCNT + TIMER_MAX_COUNT - last_time_set;
	return ret;
}


/**
 * 处理定时器的函数 定时1ms
 */
//void  vApplicationTickHook(void)
void timerforCAN(void)
{
	TimeCNT++;
	if (TimeCNT >= TIMER_MAX_COUNT)
	{
		TimeCNT = 0;
	}
	if (TimeCNT == NextTime)
	{
		last_time_set = TimeCNT;
		TimeDispatch();
	}
}
