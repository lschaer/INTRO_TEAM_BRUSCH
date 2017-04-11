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
	}
}

static void AppTask(void* param) {
  const int *whichLED = (int*)param;

  for(;;) {
    if (*whichLED==1) {
      LED1_Neg();
    } else if (*whichLED==2) {
      LED2_Neg();
    }
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

  /* Es wird 2 mal der TASK AppTask erstellt */
  static const int led1 = 1;
  static const int led2 = 2;

  res = xTaskCreate(AppTask,
		  (signed portCHAR *)"App1",
		  100,
		  (void*)&led1,
		  tskIDLE_PRIORITY,
		  NULL
		  );
  if(res!=pdPASS) { /*Error handling here*/}
  res = xTaskCreate(AppTask,
		  (signed portCHAR *)"App2",
		  100,
		  (void*)&led2,
		  tskIDLE_PRIORITY,
		  NULL
		  );
  if(res!=pdPASS) { /*Error handling here*/}

}




void RTOS_Deinit(void) {
  /* nothing needed for now */
}


#else /* PL_LOCAL_CONFIG_BOARD_IS_REMOTE*/
void RTOS_Init(void) {
  /*! \todo Create tasks here */
  //xTaskCreate()


}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}




#endif /* PL_LOCAL_CONFIG_BOARD_IS_REMOTE*/
#endif /* PL_CONFIG_HAS_RTOS */
