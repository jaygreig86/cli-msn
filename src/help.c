/* CLI-MSN Command/Function Help
 * filename: /src/help.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/06/03
 */
#include "headers/help.h"
#include <string.h>
extern void cli_msg();
void help(char *msgbody)
{
	int tmp;
	int ctr;
	char cmd[8];
	extern int lc;

        /* Convert uppercase characters to lowercase */

        ctr = 6;
	memset(cmd, '\0', sizeof(cmd));
        do {

        if ( (msgbody[ctr] >= 'a') && (msgbody[ctr] <= 'z')) cmd[strlen(cmd)] = (msgbody[ctr] -= lc);

        ctr++;

        } while ( msgbody[ctr] != '\n' && msgbody[ctr] != '\0');

	for(ctr=0;ctr < FUNC_COUNT;ctr++){
		if (! strcmp(cmd, help_funcs[ctr]) ){
			cli_msg("USAGE: %s\n", help_usage[ctr]);
			return;
		}
	}

/*	tmp = 0;
	cli_msg("");
	for(ctr=0;ctr < FUNC_COUNT;ctr++){
		tmp++;
		waddstr(console, help_funcs[ctr]);
		if ( ( strlen(help_funcs[ctr]) >= 6 && tmp == 1 ) || (  strlen(help_funcs[ctr]) >= 8 && tmp == 2 ) ) waddstr(console, "\t");
		if ( ( strlen(help_funcs[ctr]) < 6 && tmp == 1 ) || (  strlen(help_funcs[ctr]) < 8 && tmp == 2 ) )  waddstr(console, "\t\t");
		if ( tmp == 3 ) waddstr(console, "\n");
		if ( tmp == 3 ){
			tmp = 0;
			cli_msg("");
		}
	}
	waddstr(console, "\n"); */
	ctr = 0;
	do {
		cli_msg("");
		print_help(help_funcs[ctr], help_funcs[ctr+1],help_funcs[ctr+2]);
		ctr += 3;
	} while ( ctr < FUNC_COUNT);
	
	cli_msg("USAGE: /help [command] - Show usage for a specfic command\n");
	
}

