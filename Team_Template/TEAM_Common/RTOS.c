/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "LED.h"
#include "Keys.h"
#include "Event.h"
#include "Application.h"
#include "Trigger.h"
#include "Keys.h"
#include "KeyDebounce.h"


#include "LED.h"


#include "Shell.h"

#if PL_LOCAL_CONFIG_BOARD_IS_ROBO

static void CallBackLED(void*p) {			// this is not a Task ! Its a Callback function
    LED1_Neg();
}

static void BlinkyTask(void *pvParameter){
	for(;;){
      //LED1_Neg();
      //(void)TRG_SetTrigger(TRG_LED_BLINK, 0, CallBackLED, NULL);  //to set a Trigger is also possible
      vTaskDelay(100/portTICK_PERIOD_MS); // 1000*ticks /10ms = 1s
	}
}

static void MainLoopTask(void *pvParameter){
	for(;;){
	  KEY_Scan();
	  EVNT_HandleEvent(APP_EventHandler, TRUE);
	  vTaskDelay(100/portTICK_PERIOD_MS); // 1000*ticks /10ms = 1s
	}
}


void RTOS_Init(void) {
  /*! \todo Create tasks here */
  //xTaskCreate()
  EVNT_SetEvent(EVNT_STARTUP); /* set startup event */

  BaseType_t res ;
  xTaskHandle taskHndl ;
  /* BlinkyTask */
  res = xTaskCreate(BlinkyTask,			/* function */
		  "Blinky",						/* Kernel awareness name */
		  configMINIMAL_STACK_SIZE+50,  /* stack */
		  (void*)NULL,					/* task parameter */
		  tskIDLE_PRIORITY,				/* priority */
		  &taskHndl						/* handle */
		  );
  if(res!=pdPASS) { /*Error handling here*/}

  /* MainLoopTask */
  res = xTaskCreate(MainLoopTask,		/* function */
		  "MainLoopT",						/* Kernel awareness name */
		  configMINIMAL_STACK_SIZE+50,  /* stack */
		  (void*)NULL,					/* task parameter */
		  tskIDLE_PRIORITY,				/* priority */
		  &taskHndl						/* handle */
		  );
  if(res!=pdPASS) { /*Error handling here*/}

}


void RTOS_Deinit(void) {
  /* nothing needed for now */
}


#else /* PL_LOCAL_CONFIG_BOARD_IS_REMOTE*/


static void AppTask(void *pvParameters){

	for(;;){
	#if PL_CONFIG_HAS_KEYS
    	//KEY_Scan(); //Polling Keys
    	KEYDBNC_Process(); //using interrupts
	#endif
	#if PL_CONFIG_HAS_EVENTS
    	EVNT_HandleEvent(APP_EventHandler, TRUE);
	#endif
    	vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/* Testfunction for Task just to Blink an LED */
static void blinkyTask(void *pvParameters){
	for(;;){
	//SHELL_SendString("Blinky!!\r\n");
	TickType_t xLastWakeTime = xTaskGetTickCount();
	LEDPin1_NegVal();
	vTaskDelayUntil(&xLastWakeTime,1000/portTICK_PERIOD_MS);
	}
}


void RTOS_Init(void) {
  /*! \todo Create tasks here */
	EVNT_SetEvent(EVNT_STARTUP);

	BaseType_t res;
	xTaskHandle tskHndl;
	res = xTaskCreate(blinkyTask,"Blinky",configMINIMAL_STACK_SIZE+50,(void*)NULL,tskIDLE_PRIORITY,&tskHndl);
	if(res!=pdPASS) { /*Error handling here*/}

	res = xTaskCreate(AppTask, "App",configMINIMAL_STACK_SIZE+50,(void*)NULL,tskIDLE_PRIORITY,&tskHndl);
	if(res!=pdPASS) {/*Error handling here*/}

}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}




#endif /* PL_LOCAL_CONFIG_BOARD_IS_REMOTE*/
#endif /* PL_CONFIG_HAS_RTOS */
