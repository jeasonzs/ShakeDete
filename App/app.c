#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "app.h"
#include "uart.h"
#include "console.h"

#define AppVersion "0.0.1"
static int appInfo(int argc,char *argv[])
{
	if (argc == 2 && (!strcmp("--help", (const char*)argv[1]))) {
		printf("info #show the system info\r\n");
	}
	else {
		printf("------------------------------\r\n"
			"os = freeRTOS(%s)\r\n"
			"AppVersion = %s\r\n"
			"--------------------------------\r\n\r\n",
			tskKERNEL_VERSION_NUMBER,
			AppVersion);
	}
	return 0;
}


void appStart()
{
	printf("appStart....\r\n");
	consoleInit();
	consoleAddExec("info",appInfo);
}

