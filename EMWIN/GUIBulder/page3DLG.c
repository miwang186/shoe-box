/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.26                          *
*        Compiled Aug 18 2014, 17:12:05                              *
*        (c) 2014 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"
#include "device_info.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_2     (GUI_ID_USER + 0x30)
#define ID_GRAPH_30     (GUI_ID_USER + 0x31)
#define ID_CHECKBOX_30     (GUI_ID_USER + 0x32)
#define ID_CHECKBOX_31     (GUI_ID_USER + 0x33)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GRAPH_DATA_Handle  humiData[2];                         /* ���� GRAPH_DATA ���͵ľ�� */
static GRAPH_SCALE_Handle humiVscale;
static GUI_COLOR _hColor[2] = {GUI_DARKBLUE,GUI_DARKRED};/* ���ߵ���ɫֵ */
// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogpage3Create
*/
static const GUI_WIDGET_CREATE_INFO _aDialogpage3Create[] = {
  { WINDOW_CreateIndirect, "page3", ID_WINDOW_2, 0, 0, 310, 120, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "allhumi", ID_GRAPH_30, 0, 24, 306, 76, 0, 0x0, 2 },
  { CHECKBOX_CreateIndirect, "hum1", ID_CHECKBOX_30, 50, 2, 80, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "humi2", ID_CHECKBOX_31, 180, 2, 80, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbpage3Dialog
*/
static void _cbpage3Dialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
	  
	hItem = pMsg->hWin;
	WINDOW_SetBkColor(hItem, GUI_DARKCYAN);  
    //
    // Initialization of 'allhumi'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_30);
    GRAPH_SetBorder(hItem,20, 2, 2, 2);
	GRAPH_SetGridDistX(hItem,20);	//����X��С
	GRAPH_SetGridDistY(hItem,10);	//����Y��С
	GRAPH_SetGridVis(hItem,1);		//��ʾ����
//	GRAPH_DATA_YT_SetOffY(hItem,-10);	//Y�����ݵ�ƫ��
//	GRAPH_SetVSizeY(hItem,80);
	humiVscale = GRAPH_SCALE_Create(2,GUI_TA_HORIZONTAL,GRAPH_SCALE_CF_VERTICAL,20);
	GRAPH_SCALE_SetFont(humiVscale,GUI_FONT_8_ASCII);
	GRAPH_SCALE_SetTextColor(humiVscale,GUI_BLUE);
	GRAPH_AttachScale(hItem,humiVscale); 
  
//	humiHscale = GRAPH_SCALE_Create(2,GUI_TA_BOTTOM,GRAPH_SCALE_CF_HORIZONTAL,40);
//	GRAPH_SCALE_SetFont(humiHscale,GUI_FONT_8_ASCII);
//	GRAPH_SCALE_SetTextColor(humiHscale,GUI_WHITE);
//	GRAPH_AttachScale(hItem,humiHscale); 
  
	humiData[0] = GRAPH_DATA_YT_Create(_hColor[0], 300, 0, 0);
	humiData[1] = GRAPH_DATA_YT_Create(_hColor[1], 300, 0, 0);

//	/* Ϊ��ͼ�ؼ��������ݶ��� */
//	GRAPH_AttachData(hItem, humiData[0]);
//	GRAPH_AttachData(hItem, humiData[1]);
	WIDGET_SetEffect(hItem, &WIDGET_Effect_None);  
    //
    // Initialization of 'hum1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_30);
    CHECKBOX_SetText(hItem, "ʪ��1");
    CHECKBOX_SetTextColor(hItem, _hColor[0]);
  	CHECKBOX_SetFont(hItem,&GUI_FontHZ12);
	CHECKBOX_SetState(hItem,1);	
    //
    // Initialization of 'humi2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_31);
    CHECKBOX_SetText(hItem, "ʪ��2");
    CHECKBOX_SetTextColor(hItem, _hColor[1]);
  	CHECKBOX_SetFont(hItem,&GUI_FontHZ12);  
	CHECKBOX_SetState(hItem,1);
		
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_CHECKBOX_30: // Notifications sent by 'hum1'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_30);
		if(CHECKBOX_IsChecked(hItem))
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_30);
			GRAPH_AttachData(hItem,humiData[0]);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_30);
			GRAPH_DATA_YT_Clear(humiData[0]);
			GRAPH_DetachData(hItem,humiData[0]);	
		}
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_31: // Notifications sent by 'humi2'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_31);
		if(CHECKBOX_IsChecked(hItem))
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_30);
			GRAPH_AttachData(hItem,humiData[1]);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_30);
			GRAPH_DATA_YT_Clear(humiData[1]);
			GRAPH_DetachData(hItem,humiData[1]);
		}
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
	case WM_TIMER:
		if(WM_GetTimerId(pMsg->Data.v) == 0)
		{
			GRAPH_DATA_YT_AddValue(humiData[0], dht11Info.humidity_1);
			GRAPH_DATA_YT_AddValue(humiData[1], dht11Info.humidity_2);
			/* ������ʱ�� */
			WM_RestartTimer(pMsg->Data.v, 1000);
		}
		break;		
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       Createpage3
*/
WM_HWIN Createpage3(void);
WM_HWIN Createpage3(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogpage3Create, GUI_COUNTOF(_aDialogpage3Create), _cbpage3Dialog, WM_HBKWIN, 0, 0);
	/* ����һ����ʱ�� */
	WM_CreateTimer(hWin, 		/* ������Ϣ�Ĵ��ڵľ�� */
				   0, 	        /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
				   1000,        /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
				   0);	
	
	return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/