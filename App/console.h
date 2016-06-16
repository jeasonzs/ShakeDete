#ifndef _CONSOLE_H
#define _CONSOLE_H

typedef int (*ConsoleExec)(int,char**);

void consoleInit(void);
void consoleAddExec(char *name,ConsoleExec exec);


#endif
