/* CLI-MSN Time Handling (online time, current time etc)
 * filename: /src/time.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 23/06/03
 */

#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include "headers/defs.h"
/*#include "headers/time.h" */
char logio[5][64];                                 /* login Time Buffer */
char status_time[2][32];                                /* Current time to show on the status bar */
time_t start, end, current;                             /* Time start and Time end for program */
long _time;
int png = 1;
int aaway = 0;

void elapsed_time();
void ping_time();
void record_time();
extern int tcp_sock[];
extern WINDOW *console;
extern WINDOW *_status;
extern WINDOW *_cmd;
extern void cli_msg();
extern void sendData();
extern int tid;
extern int args[NO_OF_ARGS][1];
/* calculate the current and elapsed time */

void elapsed_time(void)
{
	int ctr;	/* Integer for looping (counter) */
	end = time(NULL);
	waddch(console, '\n');
	cli_msg("Signon Time: %s", (char *) logio);
	cli_msg("Signoff Time: %s", ctime(&end));
	snprintf(logio[1], sizeof(logio[1]), "%f", difftime(end, start));
	for(ctr=0;logio[1][ctr] != '.';ctr++){
		logio[2][ctr] = logio[1][ctr];
	}

	/* Get total number of seconds online and place into int _time */

	_time = (logio[2][0] - 48);

	for(ctr=1;ctr < (int) strlen(logio[2]);ctr++){
		_time = (_time * 10) + (logio[2][ctr] - 48);
	}

	cli_msg("Elapsed Online Time: %dd %dh %dm %ds\n", ((_time/60)/60)/60%60, (_time/60)/60%60, _time/60%60, _time%60);

}

/* Function handles the pinging of the notification server
 * for the lag-o-meter, and also covers the Time display
 * in the status
*/
void initialize_time(void)
{
	current = time(NULL);
	return;
}
void ping_time(void)
{
	int ctr;	/* Integer for looping (counter) */
	char sendbuff[10];
	current = time(NULL);

        snprintf(logio[3], sizeof(logio[3]), "%f", difftime(current, start));

        for(ctr=0;logio[3][ctr] != '.';ctr++){
                logio[4][ctr] = logio[3][ctr];
        }

	_time = (logio[4][0] - 48);

        for(ctr=1;ctr < (int) strlen(logio[4]);ctr++){
                _time = (_time * 10) + (logio[4][ctr] - 48);
        }
	if (_time/5 == png){
		snprintf(sendbuff, sizeof(sendbuff), "PNG\r\n");
		sendData(0, sendbuff, strlen(sendbuff));
		png++;
	}

	if (png > _time/5){
/*		cli_msg("Possible TIME_PING_SYNC Failure\n");  THIS LINE IS FOR DETECTING WHETHER THE PINGS ARE OUT OF SYNC */
	}

	if (_time/5 > png){
/*		cli_msg("Possible TIME_PING_SYNC Failure\n");  THIS LINE IS FOR DETECTING WHETHER THE PINGS ARE OUT OF SYNC */
		png = _time/5;	
		png++;
	}
	strcpy(status_time[1], status_time[0]);
	snprintf(status_time[1], sizeof(status_time[1]), "[%.16s]", ctime(&current));
	if ( strcmp(status_time[0], status_time[1]) != 0 ) {
		strcpy(status_time[0], status_time[1]);
		mvwprintw(_status, 0, 8, "[%.16s]", ctime(&current));
		wrefresh(_status);
		wrefresh(_cmd);
	}
}

/* Records the sign-on time */

void record_time(void)
{
	start = time(NULL);
	snprintf(logio[0], sizeof(logio[0]), "%s", ctime(&start));
}

void away(int type)
{
	if ( args[3][0] > 0 ){
		if ( type == 1 && aaway < args[3][0]){
			aaway++;
		} else if ( type == 0 && aaway == (args[3][0] + 1)){
			aaway = 0;
			status("NLN");		
		}
		if ( aaway == args[3][0] ){
			status("IDL");
			aaway++;
		}
	}
}
