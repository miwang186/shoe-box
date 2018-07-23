/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : This file provides emWin Interface with FreeRTOS
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "GUI.h"
    
/* FreeRTOS include files */
#include <rtthread.h>
#include "sw_delay.h"    
/*********************************************************************
*
* Global data
*/
static struct 	rt_semaphore  	DispSem;  	//显示的信号量
static struct 	rt_semaphore	EventSem;  


static struct 	rt_semaphore	KeySem;  	//按键信号量
static int		KeyPressed;
static char		KeyIsInited;
/*********************************************************************
*
* Timing:
* GUI_X_GetTime()
* GUI_X_Delay(int)

Some timing dependent routines require a GetTime
and delay function. Default time unit (tick), normally is
1 ms.
*/

int GUI_X_GetTime(void)
{
  return ((int) rt_tick_get() * 5);
}

void GUI_X_Delay(int ms)
{
	rt_uint8_t sw_ms;
//	rt_uint32_t ticks;
//    ticks = (ms * 1000) / RT_TICK_PER_SECOND;
//	rt_thread_delay(ticks);
	if(ms >= 5)
	{
		rt_thread_delay(ms/5);	
	}
	sw_ms = ms%5;
	sw_delay_ms(sw_ms);
}

/*********************************************************************
*
* GUI_X_ExecIdle
*
* Note:
* Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) 
{
    GUI_X_Delay(10);
}

/*********************************************************************
*
* Multitasking:
*
* GUI_X_InitOS()
* GUI_X_GetTaskId()
* GUI_X_Lock()
* GUI_X_Unlock()
*
* Note:
* The following routines are required only if emWin is used in a
* true multi task environment, which means you have more than one
* thread using the emWin API.
* In this case the
* #define GUI_OS 1
* needs to be in GUIConf.h
*/

/* Init OS */
void GUI_X_InitOS(void)
{ 
	if(rt_sem_init(&DispSem, "Disp_SEM", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("DispSem init faill \r\n");
	}
	if(rt_sem_init(&EventSem, "Event_SEM", 0, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("EventSem init faill \r\n");		
	}
}

//等待信号量
void GUI_X_Lock(void)
{
	if(rt_sem_take(&DispSem, RT_WAITING_FOREVER) != RT_EOK) /*阻塞 获取信号量*/
	{
		rt_kprintf("DispSem take timeout! \r\n");				
	}
}

//发送信号量
void GUI_X_Unlock(void)
{
	if(rt_sem_release(&DispSem) != RT_EOK)				//发送信号量
	{
		rt_kprintf("DispSem release faill! \r\n");						
	}
}
extern rt_uint8_t rt_current_priority;
//放回任务ID，此处返回的是任务优先级，由于UCOSIII支持时间片
//轮转调度，因此如果使用了时间片轮转调度功能的话有可能会出错！
U32 GUI_X_GetTaskId(void)
{
	return rt_current_priority;
}

void GUI_X_WaitEvent (void) 
{
	rt_sem_take(&EventSem, RT_WAITING_FOREVER);
}


void GUI_X_SignalEvent (void) 
{
	rt_sem_release(&EventSem);			//发送信号量
}

/*********************************************************************
*
* Logging: OS dependent

Note:
Logging is used in higher debug levels only. The typical target
build does not use logging and does therefor not require any of
the logging routines below. For a release build without logging
the routines below may be eliminated to save some space.
(If the linker is not function aware and eliminates unreferenced
functions automatically)

*/

void  CheckInit (void) 
{
	if (KeyIsInited == 0u) {
		KeyIsInited = 1u;
		GUI_X_Init();
	}
}
/*********************************************************************
*
* GUI_X_Init()
*
* Note:
* GUI_X_Init() is called from GUI_Init is a possibility to init
* some hardware which needs to be up and running before the GUI.
* If not required, leave this routine blank.
*/
void GUI_X_Init (void) 
{
	rt_sem_init(&KeySem, "Key_SEM", 0, RT_IPC_FLAG_FIFO);	
}

int  GUI_X_GetKey (void) 
{
	int r;

	r = KeyPressed;
	CheckInit();
	KeyPressed = 0;
	return (r);
}

int  GUI_X_WaitKey (void) 
{
	int    r;

	CheckInit();
	if (KeyPressed == 0) 
	{   
		rt_sem_take(&KeySem, 0);
	}
	r = KeyPressed;
	KeyPressed = 0;
	return (r);		   
}

void  GUI_X_StoreKey (int k) 
{
	KeyPressed = k;
	rt_sem_release(&KeySem);
}

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }


/*************************** End of file ****************************/
