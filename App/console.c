#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "uart.h"
#include "string.h"
#include "console.h"

#define CMD_BUF_SIZE 64

typedef struct {
	char *name;
	ConsoleExec exec;
}ConsoleExecInfo;


static osThreadId taskHandle;
static List_t execList;

static char* shellGetLine()
{
	
	static char cmdBuf[CMD_BUF_SIZE] = {0};
	int cmdPos = 0;
	char rxByte;

	while(1) {
		if (pdTRUE == uartRx(0,&rxByte,1,100)) {
			if (rxByte >= 0x20 && rxByte <= 0x7e) {
				uartTX(0,&rxByte,1,10);
			
				if (cmdPos == CMD_BUF_SIZE - 1) {
					cmdPos = 0;
				}
				cmdBuf[cmdPos++] = rxByte;
			}

			if(rxByte == '\r') {
				cmdBuf[cmdPos] = '\0';
				cmdPos = 0;
				return cmdBuf;
			}

			if(rxByte == '\b') {
	        	if(cmdPos > 0) {
					uartTX(0,"\b \b",3,10);					        
	        		cmdPos--;
	        	}
	        	else {				
					uartTX(0,"\a",1,10);	
			    }
		    }

		}
	}
}

static void shellProcess(char *shellBuffer) 
{
	int argc = 0;
	char *argv[16] = {0};
	char *p = strtok(shellBuffer, " ");
	while (p) {
		argv[argc++] = p;
		p = strtok(NULL, " ");
	}

	printf("\r\n");

	if (argc > 0 ) {
		/*
		if(!(strcmp(argv[0], "help"))) {
			printf("help\r\n");
		}	*/
		int errorFlg = 1;
		for (int i = 0; i < execList.uxNumberOfItems; i++) {
			ConsoleExecInfo *info = NULL;
			listGET_OWNER_OF_NEXT_ENTRY( info, &execList );
			if (!(strcmp(argv[0], info->name))) {
				info->exec(argc,argv);
				errorFlg = 0;
			}
		}
		if (errorFlg) {
			printf("cmd %s no found!!!!\r\n",argv[0]);
		}
	}
	
}



static void consoleTask(void const * argument)
{

	for(;;)
	{
		char *shellBuffer = shellGetLine();
		shellProcess(shellBuffer);
		printf("$>");
	}
}

static int execConsoleHelp(int argc,char *argv[])
{
	if (argc == 2 && (!strcmp("--help", argv[1]))) {
		if (!strcmp("help", argv[0])) {
			printf("help #to show this text\r\n");
		}
	}
	else {
		for (int i = 0; i < execList.uxNumberOfItems; i++) {
			ConsoleExecInfo *info = NULL;
			listGET_OWNER_OF_NEXT_ENTRY( info, &execList );
			char *argHelp[2] = {info->name,"--help"};
			info->exec(2,argHelp);
		}
	}
	return 0;
}

static int execLs(int argc,char *argv[])
{
	if (argc == 2 && (!strcmp("--help", argv[1]))) {

	}
	else {
		for (int i = 0; i < execList.uxNumberOfItems; i++) {
			ConsoleExecInfo *info = NULL;
			listGET_OWNER_OF_NEXT_ENTRY( info, &execList );
			printf("%s ",info->name);
		}
		printf("\r\n");
	}
	return 0;
}


void consoleInit()
{
	vListInitialise( &execList);
	consoleAddExec("help",execConsoleHelp);
	consoleAddExec("ls",execLs);
	consoleAddExec("?",execConsoleHelp);
	uartInit(0);
	osThreadDef(ConsoleTask, consoleTask, osPriorityNormal, 0, 128);
	taskHandle = osThreadCreate(osThread(ConsoleTask), NULL);
}



void consoleAddExec(char *name,ConsoleExec exec)
{
	ListItem_t *item = pvPortMalloc(sizeof(ListItem_t));
	vListInitialiseItem(item);
	
	ConsoleExecInfo *info = pvPortMalloc(sizeof(ConsoleExecInfo));
	info->name = name;
	info->exec = exec;

	item->pvOwner= info;
	vListInsert( &execList, item );
}


