/* MSN Input0 read source file
 * filename: /src/command.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 29/04/05
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include "headers/defs.h"
#include "headers/command.h"
extern int TOP_PAN;
extern char panusr[MAX_TCP_CON][MAX_ACCOUNT_LEN];
extern char convo_type[MAX_TCP_CON][16];                /* Conversation type in open panel (public/private/file) */
extern char convo_users[MAX_TCP_CON][255];              /* Users currently in conversation in open panel */
extern char convo_socket[MAX_TCP_CON][1];               /* Socket number to match with panel, i.e. socket 3 = panel 2 */
extern void switchwin();
char command[32];
int no_of_elements;
void input_case(char *msgbody)
{

	/* Send message to currently open query */
	if ( msgbody[0] != '/' && strlen(msgbody) > 1){
		char message[255]; /* Temporary Message Storage */
		int ii = 0;
		if ( TOP_PAN ){
			strcpy(message, msgbody);
			message[strlen(message) - 1] = '\0';

			/* Check if a conversation is open, if not open it */

			if ( convo_socket[TOP_PAN - 1][0] > 0 ){
                                msgqueue(panusr[TOP_PAN - 1], message);
                                msg_send(panusr[TOP_PAN - 1], message);
                                cmdhistory();
                                return;
                        }
					char sendbuff[128];
					/* Move name and message to queue */
					if ( msgqueue(panusr[TOP_PAN - 1],"") ) return;
					msgqueue(panusr[TOP_PAN - 1], message);

                                        snprintf(sendbuff, sizeof(sendbuff), "XFR %d SB\r\n", tid);
                                        sendData(0, sendbuff, strlen(sendbuff));
                                        
		}
                cmdhistory();
		return;
	}
        memset(command, '\0', sizeof(command));
        get_element(msgbody, strlen(msgbody),command, 0);
        no_of_elements = count_element(msgbody, strlen(msgbody));	

/* /msg function */
if ( !strcmp(command,"/msg") ){
        if ( no_of_elements < 3){
                cli_msg("Usage: /MSG USER@HOST MESSAGE\n");
                cmdhistory();
                return;
        }
        {
	int ctr2 = 0;
	int ctr = 5;
	int ii = 0;
	char message[255];
	char who[MAX_ACCOUNT_LEN];
	memset(who, '\0', sizeof(who));

	do {
		if (msgbody[ctr] != ' ' && msgbody[ctr] != '\n' ){
			if ( (msgbody[ctr] >= 'A') && (msgbody[ctr] <= 'Z')) who[ctr2] += lc;
			else who[ctr2] = msgbody[ctr];
			ctr2++;
		}
		ctr++;

	} while (msgbody[ctr] != '\0' && msgbody[ctr] != ' ' );

        memset(message, '\0' ,sizeof(message));

	ctr += 1;
        do {
        	if ( msgbody[ctr] != '\n' ) message[strlen(message)] = msgbody[ctr];
        	ctr++;
        } while ( msgbody[ctr] != '\n' && msgbody[ctr] != '\0' );

	/* Check if a conversation is open, if not open it */
	for(ii = 0;ii<MAX_TCP_CON;ii++){
		if ( !strcmp(convo_users[ii], who)){
                	msgqueue(who, message);
                        msg_send(who, message);

                        cmdhistory();
                        return;
                }
        }
	char sendbuff[128];
	msgqueue(who,"");

	snprintf(sendbuff, sizeof(sendbuff), "XFR %d SB\r\n", tid);
	sendData(0, sendbuff, strlen(sendbuff));

	/* Move name and message to queue */
	msgqueue(who, message);

        cmdhistory();
        return;
}
}	

/* /dump */

if ( !strcmp(command,"/dump") ){
	queuedump();
        cmdhistory();
	return;
}

/* /rem */

if ( !strcmp(command,"/rem") ){
        if ( no_of_elements != 2){
                cli_msg("Usage: /REM USER@HOST\n");
                cmdhistory();
                return;
        }
        else {
		char who[MAX_ACCOUNT_LEN];
		char sendbuff[255];
                int ctr;

                memset(who, '\0', sizeof(who));
                get_element(msgbody, strlen(msgbody), who, 1);

		for ( ctr=0;ctr < MAX_LIST_SIZE;ctr++){
			if (! strcmp(who, contactn[ctr]) ){
		                snprintf(sendbuff, sizeof(sendbuff), "REM %d AL %s\r\n", tid, who);
        		        sendData(0, sendbuff, strlen(sendbuff));

				if ( strlen(contactc[ctr]) > 1 ){
	                		snprintf(sendbuff, sizeof(sendbuff), "REM %d FL %s\r\n", tid, contactc[ctr]);
	                		sendData(0, sendbuff, strlen(sendbuff));
				}

				cmdhistory();
				return;
			}
		}
		cli_msg("Non-existent contact: %s\n", who);
                cmdhistory();
                return;
	}
}
/* /Time */
if ( !strcmp(command,"/time") ){
	elapsed_time();
	cmdhistory();
	return;
}

/* /bye */

if ( !strcmp(command,"/bye") ) {
	char sendbuff[16];
	int i = 0;
	cli_msg("Logging off\n");
	memset(sendbuff, '\0', sizeof(sendbuff));
	snprintf(sendbuff, sizeof(sendbuff), "BYE\r\n");
	sendData(0, sendbuff, strlen(sendbuff));

	clear_contact();
	
	cli_msg("Closing connections\n");
	for(i=0;i<MAX_TCP_CON;i++){
		if ( tcp_sock[i] ){
			close(tcp_sock[i]);
			tcp_sock[i] = 0;
		}
	}
	cmdhistory();
	return;
}

/* /connect */

if ( !strcmp(command,"/connect") ) {
	char *server;
	int ctr = 9;
	char sendbuff[16];
	if ( no_of_elements != 2){
                cli_msg("Usage: /CONNECT USER@HOST\n");
                cmdhistory();
                return;
	}
	clear_contact();	/* Clear contact list database for a fresh login */
	online = 0;	/* Reset Online Status */
	memset(username, '\0', sizeof(username));
	memset(password, '\0', sizeof(password));

        get_element(msgbody, strlen(msgbody), username, 1);

        noecho();

        wprintw(_cmd, "Enter Your Password: ");
        wrefresh(_cmd);

        wgetstr(_cmd, password);
        wclear(_cmd);
        wrefresh(_cmd);

	cli_msg("Logging in as %s...\n", username);

	echo();
	i = 0;
	if ( tcp_sock[0] ) tcp_sockdcon(0);
	if (! ( server = dispatch(username)) ){
		cmdhistory();
		return;
	}
	else {
		if ( notification(username, password, server) ) {
			cmdhistory();
			return;
		}
	}

        if ( strlen(nick) > 10 ){
                mvwprintw(_status, 1, 0, "Status: Online\t\tNickName: %c%c%c%c%c%c%c%c%c%c...\n", nick[0], \
                nick[1], nick[2], nick[3], nick[4], nick[5], nick[6], nick[7], nick[8], nick[9]);
        }
        else {
                mvwprintw(_status, 1, 0, "Status: Online\t\tNickName: %s\n", nick);
        }

        wrefresh(_status);
	cli_msg("Login Successful, %s online\n", username);
	cmdhistory();
	return;
}

/* /hide contacts */
if ( !strcmp(command,"/hide") ){
	contacts_hide();
	cmdhistory();
	return;
}

/* /show contacts */
if ( !strcmp(command,"/show") ){
        contacts_show();
        cmdhistory();
	return;
}


/* /Refresh */
if ( !strcmp(command,"/refresh") ){
        redrawwin(_cmdph);
        wrefresh(_cmdph);
	redrawwin(console);
//        wrefresh(console);
      redrawwin(_status);
        wrefresh(_status);
	if ( contact_list_show ){
		 redrawwin(_contacts);
		wrefresh(_contacts);
	}
/*	redrawwin(_cmd); */
	wrefresh(_cmd); 
	cli_msg("Terminal Refreshed\n");
	cmdhistory();
	return;
}
/* Auto Away */
if ( !strcmp(command,"/aaway") ){
	if ( no_of_elements < 2) {
		cli_msg("Usage: /aaway [1-60]\n");
		cmdhistory();
		return;
	}
	{
		char timer[3];
		memset(timer, '\0', sizeof(timer));
		get_element(msgbody, strlen(msgbody), timer, 1);
		if ( atoi(timer) > 60 || atoi(timer) < 1) {
	                if ( atoi(timer) == 0 ){
				cli_msg("Auto Away Disabled\n");
				args[3][0] = atoi(timer);
			}
			else cli_msg("Invalid Time, must be between 1 and 60 minutes\n");
	                cmdhistory();
	                return;
		}
		args[3][0] = atoi(timer) * 60;
		cli_msg("Auto Away Set: %d minutes\n", atoi(timer));
	        cmdhistory();
	        return;
	}
}

/* Away status */
if (!strcmp(command, "/away") ){
	status("AWY");
	cmdhistory();
	return;
}


/* Idle status */
if ( !strcmp(command, "/idle")){
	status("IDL");
        cmdhistory();
        return;
}


/* Online status (back) */

if ( !strcmp(command, "/back") || !strcmp(command,"/online") ){
	status("NLN");
        cmdhistory();
	return;
}

/* Appear Offline status /hidden */

if ( !strcmp(command, "/hidden") ){
	status("HDN");
        cmdhistory();
	return;
}

/* Busy status */

if ( !strcmp(command, "/busy") ){
	status("BSY");
        cmdhistory();
	return;
}

/* Be Right Back status */

if ( !strcmp(command, "/brb") ){
	status("BRB");
        cmdhistory();
	return;
}

/* On The Phone status */

if ( !strcmp(command, "/phn")){
	status("PHN");
        cmdhistory();
	return;
}

/* Out To Lunch status */

if (!strcmp(command, "/out") ){
	status("LUN");
        cmdhistory();
	return;
}

/* File Transfer (send) */

if ( !strcmp(command, "/send") ){
        if ( no_of_elements != 2){
                cli_msg("Usage: /SEND FILE\n");
		cmdhistory();
		return;
        }else {
	if (! TOP_PAN) {
		cli_msg("Files can only be sent within an active conversation\n");
		cmdhistory();
		return;
	}
	char sendbuff[1024];
	char msg[512];
	int ii;
	char filename[255];
        struct stat sb;
	memset(msg, '\0', sizeof(msg));
	memset(sendbuff, '\0', sizeof(sendbuff));
	memset(filename, '\0', sizeof(filename));
	get_element(msgbody, strlen(msgbody), filename, 1);

        stat(filename, &sb);
        srand(time(0));
//        cookie = rand()%7295+1;
	cookie = 11;
	snprintf(msg, sizeof(msg), "MIME-Version: 1.0\r\nContent-Type: text/x-msmsgsinvite; charset=UTF-8\r\n\r\nApplication-Name: File Transfer\r\nApplication-GUID: {5D3E02AB-6190-11d3-BBBB-00C04F795683}\r\nInvitation-Command: INVITE\r\nInvitation-Cookie: %d\r\nApplication-File: %s\r\nApplication-FileSize: %d\r\nConnectivity: Y\r\n", cookie, filename, (int) sb.st_size);
        snprintf(sendbuff, sizeof(sendbuff), "MSG %d A %d\r\n%s", tid, strlen(msg), msg);

        /* Check if a conversation is open, if not open it */
	for(ii = 0;ii<MAX_LIST_SIZE + 1;ii++){
		if ( strcmp(panusr[TOP_PAN - 1], convoa[ii]) == 0 ){
                        i = ii + 1;
                        sendData(i, sendbuff, strlen(sendbuff));
                        c_msg(TOP_PAN-1, "Waiting for %s to accept file send: %s\n", panusr[TOP_PAN - 1], filename);
                        cmdhistory();
                        return;
		}
	}
	return;
	}
}

/* The notorious help command */

if ( !strcmp(command, "/help") ){
	help(msgbody);
	cmdhistory();
	return;
}

/* Logging */
if ( !strcmp(command, "/log")){
        if ( msgbody[5] == '1' ){
                cli_msg("Logging Enabled\n");
                args[2][0] = 1;
        }else if ( msgbody[5] == '0' ){
                cli_msg("Logging Disabled\n");
                args[2][0] = 0;
        }else cli_msg("Usage: /LOG 1 or /LOG 0\n");

        cmdhistory();
        return;
}

/* Debugging */
if ( !strcmp(command, "/debug") ){
	if ( msgbody[7] == '1' ){
		cli_msg("Debugging Enabled\n");
		args[0][0] = 1;
	}else if ( msgbody[7] == '0' ){
                cli_msg("Debugging Disabled\n");
                args[0][0] = 0;
        }else cli_msg("Usage: /DEBUG 1 or /DEBUG 0\n");

	cmdhistory();
	return;
}

/* Add a user to your allow and contact lists */

if (!strcmp(command, "/add") ){
	if ( no_of_elements != 2){
                cli_msg("Usage: /ADD USER@HOST\n");
		cmdhistory();
		return;
        }
        else {
		char sendbuff[255];
		char who[MAX_ACCOUNT_LEN];
		int ctr = 5;
                memset(who, '\0', sizeof(who));

                get_element(msgbody,strlen(msgbody),who,1);

                snprintf(sendbuff, sizeof(sendbuff), "ADC %d AL N=%s\r\n", tid, who);
                sendData(0, sendbuff, strlen(sendbuff));

                snprintf(sendbuff, sizeof(sendbuff), "ADC %d FL N=%s F=%s\r\n", tid, who, who);
                sendData(0, sendbuff, strlen(sendbuff));

		cmdhistory();
		return;
	}
}


/* User@host blocking function */

if ( !strcmp(command, "/block") ){
	if ( no_of_elements != 2){
		cli_msg("Please enter a vaild account name\n");
		cmdhistory();
		return;
	}
	else {
	char sendbuff[255];
	char who[MAX_ACCOUNT_LEN];
	int ctr;

        memset(who, '\0', sizeof(who));

        get_element(msgbody,strlen(msgbody),who,1);
 
		for(ctr=0;ctr <= MAX_LIST_SIZE;ctr++){
			if (! strcmp(contactn[ctr],who) ){
		                snprintf(sendbuff, sizeof(sendbuff), "REM %d AL %s\r\n", tid, who);
		                sendData(0, sendbuff, strlen(sendbuff));

		                snprintf(sendbuff, sizeof(sendbuff), "ADC %d BL N=%s\r\n", tid, who);
		                sendData(0, sendbuff, strlen(sendbuff));

				blocked[ctr] = 1;
		                s_msg("User %s moved to 'block' list\n", who, NULL);
				cmdhistory();
				return;
			}

		}
		s_msg("ERROR: User %s is not in your contact list\n", who);
	}
	cmdhistory();
	return;
}

/* User@host 'UNblocking function /ublock */

if ( !strcmp(command, "/ublock") ){
	if ( no_of_elements != 2){
        	cli_msg("Please enter a vaild account name\n");
		cmdhistory();
		return;
	}
	char who[MAX_ACCOUNT_LEN];
        int ctr;
	char sendbuff[255];

       	memset(who, '\0', sizeof(who));

        get_element(msgbody,strlen(msgbody),who,1);

                for (ctr=0;ctr<MAX_LIST_SIZE;ctr++){
                        if(! ( strcmp(who, contactn[ctr]) ) ){
				if ( blocked[ctr] == 0 ){
					s_msg("User %s is not in your block list\n", who);					
					
					cmdhistory();
					return;
				}
                                break;
                        }
                }
		blocked[ctr] = 0;

                snprintf(sendbuff, sizeof(sendbuff), "REM %d BL %s\r\n", tid, who);
                sendData(0, sendbuff, strlen(sendbuff));

                snprintf(sendbuff, sizeof(sendbuff), "ADC %d AL N=%s\r\n", tid, who);
                sendData(0, sendbuff, strlen(sendbuff));

                s_msg("User %s moved to 'Allow' list\n", who, NULL);


	cmdhistory();
	return;
}

/* /nick Nick change function */
	
if ( !strcmp(command, "/nick")){
	if ( no_of_elements < 2 ||  strlen(msgbody) > MAX_NICK_LEN + 6){
        	cli_msg("Please enter a valid nickname (Maximum length 129 characters)\n");
		cli_msg("Current Nick: %s\n", nick);
		cmdhistory();
		return;
	}
        else {
                int ctr = 1;
                char part[MAX_NICK_LEN];
		char msg[MAX_NICK_LEN];
		char sendbuff[255];
		memset(msg, '\0', sizeof(msg));
                do {
                        memset(part, '\0', sizeof(part));
                        if ( ctr > 1 ) strcat(msg, "%20");
                        get_element(msgbody,strlen(msgbody), part, ctr);
                        strcat(msg,part);
                        ctr++;
                } while (ctr < no_of_elements );

                memset(sendbuff, '\0', sizeof(sendbuff));
                snprintf(sendbuff, sizeof(sendbuff), "PRP %d MFN %s\r\n", tid, msg);
                sendData(0, sendbuff, strlen(sendbuff));

		/* Stores NickName for later usage (NON-URL ENCODED) */
                memset(nick, '\0', sizeof(nick));
                
                do {
                        get_element(msgbody,strlen(msgbody), part, ctr);
                        strcat(nick,part);
                        ctr++;
                } while (ctr < no_of_elements );


	}
	cmdhistory();
	return;
}

/* /listn */
if ( !strcmp(command, "/listn")){
	int ctr;
	box_title("\nFriendly Nickname List\n\n");
	for(ctr=0;ctr<150;ctr++){
		if ( strlen(contactp[ctr]) > 0 ) {
			box_title(contactf[ctr]);
	               	wprintw(console, "\n%s\n", contactn[ctr]);
		}
	}
	wprintw(console, "\n");

	switchwin(TOP_PAN);

	cmdhistory();
	return;
}
/* /list */
if ( !strcmp(command, "/list")){
	outlist();
	cmdhistory();
	return;
}
/* /lista */
if ( !strcmp(command, "/lista")){
	int ctr = 0;
	box_title("\nFull Contact List\n");
	for(ctr=0;ctr <MAX_LIST_SIZE;ctr++){
		if ( strlen(contactn[ctr]) > 0 ) {
			box_title(contactf[ctr]);
			wprintw(console, "\n");
			if ( strlen(contactf[ctr]) > 0 ) wprintw(console, "%s\n", contactn[ctr]);
			if ( strlen(contactc[ctr]) > 0 ) wprintw(console, "%s\n", contactc[ctr]);
		}
	}

	switchwin(TOP_PAN);

        cmdhistory();
        return;
}

/* Close specified switchboard connection upon /CLOSE */

if ( !strcmp(command, "/close") ){
	if ( strlen(msgbody) <= 7 || msgbody[6] != ' ' ){
		if ( TOP_PAN ){
			int ctr;
                        int panel = TOP_PAN-1;
			char sendbuff[255];
			char user[MAX_ACCOUNT_LEN];
			strcpy(user, panusr[TOP_PAN-1]);
			close_convo(panusr[TOP_PAN-1]);
                        	if ( ! strcmp(convo_users[panel],user) ){
                                        cli_msg("Closed panel: %d\n", convo_socket[panel][0]);
                                        snprintf(sendbuff, sizeof(sendbuff), "OUT\r\n");
                                        sendData(convo_socket[panel][0], sendbuff, strlen(sendbuff));

                                        tcp_sockdcon(convo_socket[panel][0]);
                                        memset(convo_type[panel], '\0', sizeof(convo_type[panel]));
                                        memset(convo_users[panel], '\0', sizeof(convo_users[panel]));
                                        memset(convo_socket[panel], '\0', sizeof(convo_socket[panel]));

                                }
                        cmdhistory();
                        return;
		}
		cli_msg("Usage: /CLOSE USER@HOST\n");	
		cmdhistory();
		return;
	} else {
		char sendbuff[255];
		char user[128];	
		int ctr, ctr2;

		do {
		        if ( (msgbody[ctr] >= 'A') && (msgbody[ctr] <= 'Z')){
		                msgbody[ctr] += lc;
		        }
		        ctr++;
	        } while ( msgbody[ctr] != '\n');
		memset(user, '\0', sizeof(user));
		get_element(msgbody, strlen(msgbody), user, 1);
		for(ctr=0;ctr < MAX_TCP_CON;ctr++){
			if ( ! strcmp(panusr[ctr],user) ){ 
				close_convo(user);
		                for(ctr2=0;ctr2 < MAX_TCP_CON;ctr2++){
		                        if ( ! strcmp(convoa[ctr2],user) ){
					snprintf(sendbuff, sizeof(sendbuff), "OUT\r\n");
					sendData(ctr2+1, sendbuff, strlen(sendbuff));

                                        tcp_sock[ctr2+1] = 0;
                                        memset(convoa[ctr2], '\0', sizeof(convoa[ctr2]));
                                        memset(convot[ctr2], '\0', sizeof(convot[ctr2]));
		                        }
		                }
/* by jaacoppi: we don't want to see this */
/*				s_msg("Conversation closed with %s\n", user); */
				cmdhistory();
				return;
			}
		}
		s_msg("Close Error: No open conversations with %s\n", user);
		cmdhistory();
		return;
	}
}

/* /Plist function lists open conversation panels */

if ( !strcmp(command, "/plist") ){
	int ctr;
	box_title("\nOpen Conversation List\n");
	box_title("User\t\t\t\tConversation #\n");
	for(ctr=0;ctr < MAX_TCP_CON;ctr++){
		if ( strlen(panusr[ctr])) wprintw(console, "%s\t\t%d\t\n", panusr[ctr], ctr+1);
	}
        waddch(console, '\n');
	switchwin(TOP_PAN);

        cmdhistory();
	return;
}

/* /Clist function lists open connections */

if ( !strcmp(command, "/clist") ){
	char who[MAX_ACCOUNT_LEN];
	int ii;
	int ctr;
	int users; /* Number of users */
	box_title("\nOpen Connection List\n");
	box_title("User\t\t\t\tSocket #\tType\n");
	for(ii=0;ii<MAX_TCP_CON;ii++){
		if ( strlen(convo_users[ii]) > 1 ){
			users = count_element(convo_users[ii], strlen(convo_users[ii]));
			for(ctr=0;ctr < users;ctr++){
				memset(who, '\0', sizeof(who));
				get_element(convo_users[ii], strlen(convo_users[ii]), who, ctr);
				wprintw(console, "%s\t\t%d\t\t%s\n", who, convo_socket[ii][0], convo_type[ii]);
			}
		}
	}
	waddch(console, '\n');

	switchwin(TOP_PAN);

	cmdhistory();
	return;
}

/* BLP */

if ( !strcmp(command, "/blp") ){
	if ( (! (msgbody[5] == 'a' || msgbody[5] == 'b')) || no_of_elements != 2 ){
		cli_msg("Please enter a valid list type\n");
		cli_msg("Usage: \"/blp a\" OR \"/blp b\"\n");
		cmdhistory();
		return;
	}{
	char sendbuff[16];
	
	if ( msgbody[5] == 'a' ){
		snprintf(sendbuff, sizeof(sendbuff), "BLP %d AL\r\n", tid);
		sendData(0, sendbuff, strlen(sendbuff));
	}
	if ( msgbody[5] == 'b' ){
		snprintf(sendbuff, sizeof(sendbuff), "BLP %d BL\r\n", tid);
		sendData(0, sendbuff, strlen(sendbuff));
	}
	}
	cmdhistory();
	return;
}

/* /quit */

if (!strcmp(command, "/quit") || !strcmp(command, "/exit") ){
	elapsed_time();
	cli_msg("Exiting\n");
	destroy_window();
	exit(0);
}

/* /Invite a user into an already open conversation */

if ( !strcmp(command, "/invite") ){
        if ( no_of_elements != 2 || ! TOP_PAN){
                c_msg(TOP_PAN, "Usage: /INVITE USER@HOST\n");
		cmdhistory();
		return;
        }
	else {
                if ( TOP_PAN ){
	                int ctr = 0;
                        char user[MAX_ACCOUNT_LEN];
                        char sendbuff[255];
	                do {
	                        if ( (msgbody[ctr] >= 'A') && (msgbody[ctr] <= 'Z')){
	                                msgbody[ctr] += lc;
	                        }
	                        ctr++;
	                } while ( msgbody[ctr] != '\n');
	                memset(user, '\0', sizeof(user));
	                get_element(msgbody, strlen(msgbody), user, 1);
			ctr = 0;
			do { 
                                if ( strcmp(panusr[TOP_PAN - 1], convo_users[ctr]) == 0 ){
	        	                c_msg(TOP_PAN-1, "Inviting %s into conversation\n", user);
					memset(sendbuff, '\0', sizeof(sendbuff));

	        	                snprintf(sendbuff, sizeof(sendbuff), "CAL %d %s\r\n", tid, user);
	        	                sendData(ctr+1, sendbuff, strlen(sendbuff));
					cmdhistory();
					return;
	        	        }
				ctr++;
			} while ( ctr < MAX_TCP_CON );
                        cmdhistory();
                        return;
                }
                cli_msg("Usage: /INVITE USER@HOST\nMust be performed in a conversation window\n");
                cmdhistory();
                return;
	}
}

/* /raw */
if ( !strcmp(command, "/raw") && strlen(msgbody) < 64){
	char sendbuff[64];
	int ctr = 5;
	memset(sendbuff, '\0', sizeof(sendbuff));
	do {
		sendbuff[strlen(sendbuff)] = msgbody[ctr];
		ctr++;
	} while ( msgbody[ctr] != '\n' && msgbody[ctr] != '\0');
	strcat(sendbuff, "\r\n");
	sendData(0, sendbuff, strlen(sendbuff));
	cmdhistory();
	return;
}

/* Switch window */

if ( !strcmp(command, "/s") || !strcmp(command, "/win") || !strcmp(command, "/window") ){
       if ( no_of_elements != 2 ){
                cli_msg("Please enter a vaild window number\n");
                cmdhistory();
                return;
        }
        char window[2];
        get_element(msgbody, strlen(msgbody), window, 1);
	switchwin(window[0] - 48);
	return;
}
/* /query Requesting a switchboard session */

if ( !strcmp(command, "/query") ){
	if ( no_of_elements != 2 ){
		cli_msg("Please enter a vaild account name\n");
		cmdhistory();
		return;
	}
	else {

	char who[MAX_ACCOUNT_LEN];
	int ii;
	char sendbuff[255];
        memset(who, '\0', sizeof(who));

	get_element(msgbody, strlen(msgbody), who, 1);

	/* is there already a panel open? */
	if ( panopen(who) ){
		switchwin(utop(who)+1);
		s_msg("There's already a conversation open with %s, please check /qlist\n",who);
		cmdhistory();
		return;
	}
/*	by jaacoppi: as usual, we don't want to see that */
/*	cli_msg("Opening conversation with %s\n", who); */

        for(ii = 0;ii<MAX_TCP_CON;ii++){
                if (! strcmp(who, convoa[ii]) ){
		        /* is there already a panel open? */
        		if ( panopen(who) ){
                		switchwin(utop(who)+1);
                		s_msg("There's already a conversation open with %s, please check /qlist\n",who);
                		cmdhistory();
                		return;
        		}

			return;
		}
        }
        msgqueue(who,"");
        snprintf(sendbuff, sizeof(sendbuff), "XFR %d SB\r\n", tid);
        sendData(0, sendbuff, strlen(sendbuff));

        cmdhistory();
        return;


	}
}
	if ( msgbody[0] == '/' ){
		cli_msg("Unknown Command: %s\n", msgbody, NULL, NULL, NULL);
		return;
	}
	return;
}

void outlist()
{
        int tmp;
	int ctr;
	int set = 0;
        box_title("\nStatus\t\t\tNick\t\t\tUser\n\n");
        for(tmp=0;tmp<MAX_LIST_SIZE;tmp++){
                if ( strlen(contactn[tmp]) > 0 && strlen(contactp[tmp]) > 0) {
			if (! strcmp("NLN", contactp[tmp]) ){
				wprintw(console, "Online ");
				set += 1;
			}
			if (! strcmp("BSY", contactp[tmp]) ) wprintw(console, "Busy ");
			if (! strcmp("IDL", contactp[tmp]) ) wprintw(console, "Idle ");
			if (! strcmp("BRB", contactp[tmp]) ) wprintw(console, "Brb ");
			if (! strcmp("AWY", contactp[tmp]) ) wprintw(console, "Away ");
			if (! strcmp("PHN", contactp[tmp]) ) wprintw(console, "Phone ");
			if (! strcmp("LUN", contactp[tmp]) ) wprintw(console, "Lunch ");

			if ( blocked[tmp] == 1){
				wprintw(console,"(Blocked)");
				set += 10;
			}

			if ( set == 11 ) wprintw(console, "\t");
			if ( set == 10 ) wprintw(console, "\t\t");
			if ( set == 0 || set == 1 ) wprintw(console, "\t\t\t");
			set = 0;
                        if ( strlen(contactf[tmp]) > 10 ){
                                wprintw(console, "%.10s...\t\t%s\n", contactf[tmp], contactn[tmp]);
                        }
                        else if ( strlen(contactf[tmp]) > 7 && strlen(contactf[tmp]) <= 10 ){
                                wprintw(console, "%s\t\t%s\n", contactf[tmp], contactn[tmp]);
                        }
                        else wprintw(console, "%s\t\t\t%s\n", contactf[tmp], contactn[tmp]);
                }
        }

	switchwin(TOP_PAN);
	return;
}
