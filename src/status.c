#include <stdio.h>
#include <string.h>
#include "headers/defs.h"
extern int tid;
extern char nick[384];

/* Change online status */
void status(char *status)
{
        char sendbuff[16];
        snprintf(sendbuff, sizeof(sendbuff), "CHG %d %s\r\n", tid, status);
        sendData(0, sendbuff, strlen(sendbuff));

	if (! strcmp(status, "NLN") ) nick_out(nick, "Online");
	if (! strcmp(status, "AWY") ) nick_out(nick, "Away");
	if (! strcmp(status, "IDL") ) nick_out(nick, "Idle");
	if (! strcmp(status, "BSY") ) nick_out(nick, "Busy");
	if (! strcmp(status, "HDN") ) nick_out(nick, "Hiding");
	if (! strcmp(status, "BRB") ) nick_out(nick, "Be Right Back");
	if (! strcmp(status, "LUN") ) nick_out(nick, "Out To Lunch");
	if (! strcmp(status, "PHN") ) nick_out(nick, "On The Phone");

	return;
}
