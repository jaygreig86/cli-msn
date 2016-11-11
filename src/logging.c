/* Message logging
   Designed by Jaacoppi
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "headers/defs.h"
extern time_t current;
extern void cli_msg();
extern int args[3][1];
/* The _switch signifies whether it should be a "you said" or "blah@blah said"
 0 = You Said
 1 = They Said
*/

void logging(int _switch, char *user, char *message)
{
        FILE *logging;
        char fullpath[100]="";
        char logpath[12] ="/.msn/";
        char *homedir = getenv("HOME");
	char buffer[MAX_ACCOUNT_LEN + MAX_MESSAGE_LEN + 40];
	if ( args[2][0] != 1 ) return;

        snprintf(fullpath, sizeof(fullpath), "%s%s%s", homedir, logpath, user);
	if (! _switch) snprintf(buffer, sizeof(buffer), "[%.16s] You said to: %s\n%s\n", ctime(&current), user, message);
	if (_switch) snprintf(buffer, sizeof(buffer), "[%.16s] %s said:\n%s\n", ctime(&current), user, message);
	if ( (logging = fopen (fullpath, "a"))){
	        fputs (buffer, logging);
	        fclose (logging);
	} else cli_msg("Error: Unable to log conversation to path \"%s\", create path or disable logging\n", fullpath);
}

