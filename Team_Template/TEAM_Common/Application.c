/**
 * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "Trigger.h"   	//added by livio
#include "WAIT1.h"
#include "CS1.h"
#include "Keys.h"
#include "KeyDebounce.h"
#include "KIN1.h"
#include "LCD.h"		//added by Luci
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_BOARD_IS_ROBO_V2
  #include "PORT_PDD.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
  #include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LCD_MENU
  #include "LCD.h"
#endif

#if PL_LOCAL_CONFIG_BOARD_IS_ROBO
#if PL_CONFIG_HAS_EVENTS


/* Eventhandler ROBO */
void APP_EventHandler(EVNT_Handle event) {
	  /*! \todo handle events */
	  switch(event) {
	  case EVNT_STARTUP:
		  //(void)BUZ_PlayTune(BUZ_TUNE_WELCOME);
	    break;
	  case EVNT_LED_OFF:
	    break;
	  case EVNT_LED_HEARTBEAT:
		  //LEDPin1_NegVal();
		break;
	  case EVNT_SW1_PRESSED:
		  //LEDPin1_NegVal();
		break;
	  case EVNT_SW1_SHORT_PRESSED:
		  CLS1_SendStr("Hello World! ",CLS1_GetStdio()->stdOut);
		  (void)BUZ_PlayTune(BUZ_TUNE_BUTTON);
		  SQUEUE_SendString("ABCDEFG");							//Sending over Shell Queue
		  //(void)TRG_SetTrigger(TRG_LED_BLINK, 1000/TRG_TICKS_MS, CallBackLED(), NULL);
		break;
	  case EVNT_SW1_LONG_PRESSED:
		  //LEDPin1_ClrVal();     OLD
		  CLS1_SendStr("Long Press \r\n", CLS1_GetStdio()->stdOut);
		break;
	  default:
	    break;
	  } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */
#else /* PL_LOCAL_CONFIG_BOARD_IS_REMOTE*/
#if PL_CONFIG_HAS_EVENTS
/* Eventhandler REMOTE */
void APP_EventHandler(EVNT_Handle event) {
	  /*! \todo handle events */
	  switch(event) {
	  case EVNT_STARTUP:
		  LED1_Neg();
		  WAIT1_Waitms(200);
		  LED1_Neg();
	    break;
	  case EVNT_SW1_PRESSED:
		  SHELL_SendString("Button 1 right! \r\n");
		  ShowTextOnLCD("right");
	  	  break;
	  case EVNT_SW2_PRESSED:
		  SHELL_SendString("Button 2 left! \r\n");
		  ShowTextOnLCD("left");
	  	  break;
	  case EVNT_SW3_PRESSED:
		  SHELL_SendString("Button 3 down! \r\n");
		  ShowTextOnLCD("down");
		  break;
	  case EVNT_SW4_PRESSED:
		  SHELL_SendString("Button 4 push! \r\n");
		  ShowTextOnLCD("push center");
		  break;
	  case EVNT_SW5_PRESSED:
		  SHELL_SendString("Button 5 up! \r\n");
		  ShowTextOnLCD("up");
		  break;
	  case EVNT_SW6_PRESSED:
		  SHELL_SendString("Button 6 push side bottom! \r\n");
		  ShowTextOnLCD("side bottom");
		  break;
	  case EVNT_SW7_PRESSED:
		  SHELL_SendString("Button 7 push side top! \r\n");
		  ShowTextOnLCD("side top");
		  break;

	  case EVNT_LED_HEARTBEAT:
		  LED1_Neg();
		break;
	  default:
	    break;
	  } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */
#endif /* PL_LOCAL_CONFIG_BOARD_IS_REMOTE*/


static const KIN1_UID RoboIDs[] = {
  /* 0: L20, V2 */ {{0x00,0x03,0x00,0x00,0x67,0xCD,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 1: L21, V2 */ {{0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 2: L4, V1  */ {{0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}}, /* revert right motor */
  /* 3: L23, V2 */ {{0x00,0x0A,0x00,0x00,0x67,0xCD,0xB8,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert left & right motor */
  /* 4: L11, V2 */ {{0x00,0x19,0x00,0x00,0x67,0xCD,0xB9,0x11,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert right encoder, possible damaged motor driver? */
  /* 5: L4, V1 */  {{0x00,0x0B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}},
};

static void APP_AdoptToHardware(void) {
  KIN1_UID id;
  uint8_t res;

  res = KIN1_UIDGet(&id);
  if (res!=ERR_OK) {
    for(;;); /* error */
  }
#if PL_CONFIG_HAS_MOTOR
  if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[0])) { /* L20 */
    (void)Q4CRight_SwapPins(TRUE);
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[3])) { /* L23 */
    (void)Q4CRight_SwapPins(TRUE);
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[4])) { /* L11 */
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[5])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  }
#endif
#if PL_CONFIG_HAS_QUADRATURE && PL_CONFIG_BOARD_IS_ROBO_V2
  /* pull-ups for Quadrature Encoder Pins */
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 10, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 10, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 11, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 11, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 16, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 16, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 17, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 17, PORT_PDD_PULL_ENABLE);
#endif
}

#include "CLS1.h"

void APP_Start(void) {
#if PL_CONFIG_HAS_RTOS
#if configUSE_TRACE_HOOKS /* FreeRTOS using Percepio Trace */
  #if TRC_CFG_RECORDER_MODE==TRC_RECORDER_MODE_SNAPSHOT
    vTraceEnable(TRC_START); /* start streaming */
  #elif TRC_CFG_RECORDER_MODE==TRC_RECORDER_MODE_STREAMING
  //  vTraceEnable(TRC_INIT); /* Streaming trace, start streaming */
  #endif
#endif /* configUSE_TRACE_HOOKS */
#endif
  PL_Init();
#if PL_CONFIG_HAS_EVENTS
  EVNT_SetEvent(EVNT_STARTUP);
#endif
#if PL_CONFIG_HAS_SHELL && CLS1_DEFAULT_SERIAL
  CLS1_SendStr((uint8_t*)"Hello World!\r\n", CLS1_GetStdio()->stdOut);
#endif
  APP_AdoptToHardware();
#if PL_CONFIG_HAS_RTOS
  vTaskStartScheduler(); /* start the RTOS, create the IDLE task and run my tasks (if any) */
  /* does usually not return! */
#else

   //EVNT_Init(); already init ???
  __asm volatile("cpsie i"); /* enable interrupts */

#if PL_LOCAL_CONFIG_BOARD_IS_ROBO

  /* Code by Livio for Timer/Events
  for(;;) {
	    WAIT1_Waitms(50); /* just wait for some arbitrary time ....
	    EVNT_HandleEvent(APP_EventHandler, TRUE);
  }
   End Code by Livio */

  /* Code by Livio Keys */
  for(;;){
	  KEY_Scan();
	  EVNT_HandleEvent(APP_EventHandler, TRUE);
  }
  /* End Code by Livio Keys */

  /* Code by Livio Console
  int i = 0;
  for(;;){
	  CLS1_SendStr("Hello World! ",CLS1_GetStdio()->stdOut);
	  CLS1_SendNum8u(i, CLS1_GetStdio()->stdOut);
	  CLS1_SendStr("\r\n",CLS1_GetStdio()->stdOut);
	  WAIT1_Waitms(500);
	  KEY_Scan();
	  EVNT_HandleEvent(APP_EventHandler, TRUE);
	  i++;
  }
  /* End Code by Livio Console */





#else //PL_LOCAL_CONFIG_BOARD_IS_REMOTE
/* Implementation for REMOTE */

  CLS1_SendStr("**#####################INTRO########################** \r\n",CLS1_GetStdio()->stdOut);
  CLS1_SendStr("Connected to REMOTE...waiting for Action! \r\n",CLS1_GetStdio()->stdOut);

  for(;;){

	  KEY_Scan();
	  EVNT_HandleEvent(APP_EventHandler, TRUE);

  }

#endif

#endif
}


