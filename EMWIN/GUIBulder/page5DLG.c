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
#define ID_WINDOW_4     (GUI_ID_USER + 0x50)
#define ID_SPINBOX_50     (GUI_ID_USER + 0x51)
#define ID_SPINBOX_51     (GUI_ID_USER + 0x52)
#define ID_SPINBOX_52     (GUI_ID_USER + 0x53)
#define ID_SPINBOX_53     (GUI_ID_USER + 0x54)
#define ID_CHECKBOX_50     (GUI_ID_USER + 0x55)
#define ID_CHECKBOX_51     (GUI_ID_USER + 0x56)
#define ID_CHECKBOX_52     (GUI_ID_USER + 0x57)
#define ID_CHECKBOX_53     (GUI_ID_USER + 0x58)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "page5", ID_WINDOW_4, 0, 0, 310, 120, 0, 0x0, 0 },
  { SPINBOX_CreateIndirect, "timing", ID_SPINBOX_50, 85, 5, 64, 36, 0, 0x0, 0 },
  { SPINBOX_CreateIndirect, "humi", ID_SPINBOX_51, 85, 50, 64, 36, 0, 0x0, 0 },
  { SPINBOX_CreateIndirect, "tempmax", ID_SPINBOX_52, 240, 5, 64, 36, 0, 0x0, 0 },
  { SPINBOX_CreateIndirect, "tempmin", ID_SPINBOX_53, 240, 50, 64, 36, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "time_key", ID_CHECKBOX_50, 3, 15, 76, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "humi",     ID_CHECKBOX_51, 3, 60, 76, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "tempB_key", ID_CHECKBOX_52, 160, 15, 76, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "tempS_key", ID_CHECKBOX_53, 160, 60, 76, 20, 0, 0x0, 0 },
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
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	  
  case WM_INIT_DIALOG:
	 hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_DARKCYAN);	  
	GUI_SetColor(GUI_BLACK);	
  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_50);
    SPINBOX_SetFont(hItem, GUI_FONT_20_1);
    SPINBOX_SetRange(hItem,1,9);
	SPINBOX_SetValue(hItem,warning_info.timing);
  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_51);
    SPINBOX_SetFont(hItem, GUI_FONT_20_1);
    SPINBOX_SetRange(hItem,1,99);
	SPINBOX_SetValue(hItem,warning_info.max_humi);  


    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_52);
    SPINBOX_SetFont(hItem, GUI_FONT_20_1);
    SPINBOX_SetRange(hItem,50,99);
	SPINBOX_SetValue(hItem,warning_info.max_temp);  

  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_53);
    SPINBOX_SetFont(hItem, GUI_FONT_20_1);
    SPINBOX_SetRange(hItem,-9,50);
	SPINBOX_SetValue(hItem,warning_info.min_temp);    

    //
    // Initialization of 'Checkbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_50);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ12);
    CHECKBOX_SetText(hItem, "定时/小时");
	if(warning_info.enable_flag & TIMING_MAX_SWITCH)
		CHECKBOX_SetState(hItem,1);		
    //
    // Initialization of 'Checkbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_51);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ12);  
    CHECKBOX_SetText(hItem, "湿度上限");
	if(warning_info.enable_flag & HUMI_MAX_SWITCH)
		CHECKBOX_SetState(hItem,1);		
    //
    // Initialization of 'Checkbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_52);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ12);
    CHECKBOX_SetText(hItem, "温度上限");
	if(warning_info.enable_flag & TEMP_MAX_SWITCH)
		CHECKBOX_SetState(hItem,1);		
    //
    // Initialization of 'Checkbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_53);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ12);    
	CHECKBOX_SetText(hItem, "温度下限");
	if(warning_info.enable_flag & TEMP_MIN_SWITCH)
		CHECKBOX_SetState(hItem,1);		
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_SPINBOX_50: // Notifications sent by 'p6value'
      switch(NCode) {  
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_50);
		warning_info.timing =  SPINBOX_GetValue(hItem);
		// USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_SPINBOX_51: // Notifications sent by 'Spinbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_51);
		warning_info.max_humi =  SPINBOX_GetValue(hItem);
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_SPINBOX_52: // Notifications sent by 'Spinbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_52);
		warning_info.max_temp =  SPINBOX_GetValue(hItem);
	 
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_53);	
		SPINBOX_SetRange(hItem,-9,warning_info.max_temp); 	  
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_SPINBOX_53: // Notifications sent by 'Spinbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_53);
		warning_info.min_temp =  SPINBOX_GetValue(hItem);
	  
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_52);	
		SPINBOX_SetRange(hItem,warning_info.min_temp,99); 	  
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_50: // Notifications sent by 'Checkbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_50);
		if(CHECKBOX_IsChecked(hItem))
		{
			deviceStatus.timing_sec = warning_info.timing * 3600;
			warning_info.enable_flag |= TIMING_MAX_SWITCH; 
		}
		else
		{
			warning_info.enable_flag &= ~TIMING_MAX_SWITCH; 			
		}
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_51: // Notifications sent by 'Checkbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_51);
		if(CHECKBOX_IsChecked(hItem))
		{
			warning_info.enable_flag |= HUMI_MAX_SWITCH; 
		}
		else
		{
			warning_info.enable_flag &= ~HUMI_MAX_SWITCH; 			
		}
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_52: // Notifications sent by 'Checkbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_52);
		if(CHECKBOX_IsChecked(hItem))
		{
			warning_info.enable_flag |= TEMP_MAX_SWITCH; 
		}
		else
		{
			warning_info.enable_flag &= ~TEMP_MAX_SWITCH; 			
		}
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_53: // Notifications sent by 'Checkbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_53);
		if(CHECKBOX_IsChecked(hItem))
		{
			warning_info.enable_flag |= TEMP_MIN_SWITCH; 
		}
		else
		{
			warning_info.enable_flag &= ~TEMP_MIN_SWITCH; 			
		}
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
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
*       Createpage5
*/
WM_HWIN Createpage5(void);
WM_HWIN Createpage5(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
