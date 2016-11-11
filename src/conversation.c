/* Filename: src/conversation.c
   Description:
	Functions to handle message sending,
	message queuing, file sending etc
*/
#include "headers/defs.h"
#include <stdio.h>
#include <string.h>
extern int tid;
extern char convoa[MAX_TCP_CON][255];
extern char convot[MAX_TCP_CON][16];
char msgqn[MAX_TCP_CON][MAX_ACCOUNT_LEN];
char msgqm[MAX_TCP_CON][MAX_MESSAGE_LEN];
extern char contactn[MAX_LIST_SIZE][MAX_ACCOUNT_LEN];
extern char contactp[MAX_LIST_SIZE][STATUS_LEN];
extern int *wc[MAX_TCP_CON];
extern int TOP_PAN;
extern char panusr[MAX_TCP_CON][MAX_ACCOUNT_LEN];
extern char convo_type[MAX_TCP_CON][16];                /* Conversation type in open panel (public/private/file) */
extern char convo_users[MAX_TCP_CON][255];              /* Users currently in conversation in open panel */
extern char convo_socket[MAX_TCP_CON][1];               /* Socket number to match with panel, i.e. socket 3 = panel 2 */
int msg_send();


/* Send the message stored in 'msga' to the party stored in 'who' */

int msg_send(char *who, char *msga)
{
        char sendbuff[512];
        char msg[MAX_MESSAGE_LEN];
        int ii = 0, i = 0;
        snprintf(msg, sizeof(msg), "MIME-Version: 1.0\r\nContent-Type: text/plain\r\nX-MMS-IM-Format: FN=MS%%20Sans%%20Serif; EF=; CO=0; CS=0; PF=22\r\n\r\n%s", msga);
        snprintf(sendbuff, sizeof(sendbuff), "MSG %d A %d\r\n%s", tid, strlen(msg), msg);

        /* Where's the message going? */
        
        for(ii=0;ii< MAX_TCP_CON;ii++){
                if(! strcmp(who, convo_users[ii])){
                        sendData(convo_socket[ii][0], sendbuff, strlen(sendbuff));
                        return 0;
                }
        }

        return 1;
}

/* Add a message to the queue, messages are sent in order FIFO */
/* return 0 on success */
int msgqueue(char *query, char *message)
{
        int ctr;
	int tmp = 0;
	char user1[MAX_ACCOUNT_LEN];
	for ( tmp=0;tmp < MAX_LIST_SIZE; tmp++ ){
		if ( strlen(contactn[tmp]) > 0 && strlen(contactp[tmp]) > 0) {
			memset(user1, '\0', sizeof(user1));
			get_element(query, strlen(query), user1, 0);
			if ( ! strcmp(contactn[tmp],user1) ){
			        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
					if ( ! strcmp(query, msgqn[ctr]) && ! strlen(msgqm[ctr]) && ! strlen(message)){
						cli_msg("Another instance of that operation is already in the queue\n");
						return 1;
					}
	        		}
	        		for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
	        		        if ( msgqn[ctr][0] == '\0' ){
	                		        strcpy(msgqn[ctr],query);
	                        		strcpy(msgqm[ctr],message);
	                        		return 0;
	                		}
	        		}
			}
		}
	}
        cli_msg("Message Queue Error\n");
	if ( TOP_PAN == 0 ) cli_msg("User may not be online\n");
	else c_msg(TOP_PAN - 1, "User may not be online\n");

        return 1;
}

/* Dump the contents of the message queue to the screen */
void queuedump()
{
	int ctr = 0;
        cli_msg("DUMPING\n");
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                cli_msg("IN : \\%s\\%s\\\n", msgqn[ctr], msgqm[ctr]);
        }
        cli_msg("DUMP END\n");
}

/* Match a user to a conversation (user to panel) if there isn't one make one*/
int utop(char *usr)
{
	int ctr=0;
	for(ctr=0;ctr < MAX_TCP_CON;ctr++){
		if (! strcmp(panusr[ctr],usr)) return ctr;
	}
	for(ctr=0;ctr < MAX_TCP_CON;ctr++){
		if ( strlen(panusr[ctr]) == 0 ) {
			strcpy(panusr[ctr], usr);
			c_msg(ctr, "Conversation started with %s\n", usr);
			return ctr;
		}
	}
	return -1;
}
/* Add a user to an already open panel, used for INVITE */
int autop(char *convo, char *usr)
{
        int ctr=0;
        for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                if (! strcmp(panusr[ctr],convo)){
			strcat(panusr[ctr], " ");
        		strcat(panusr[ctr], usr);
			return ctr;
		} 
        }

}
/* Remove a user from an active conversation INVITE */
void dufromp(int convo, char *usr)
{
	int contacts = count_element(convoa[convo], strlen(convoa[convo]));
	char cusr[MAX_ACCOUNT_LEN];	/* current user */
	char temp[255];
	int ctr = 0;
	int ctr2 = 0;
	int user_to_remove = -1;
	if ( contacts <= 1 ) {
//		close_convo(usr);
		return;
	}
	do {
		memset(cusr, '\0', sizeof(cusr));
		get_element(convoa[convo], strlen(convoa[convo]), cusr, ctr);
		if (! strcmp(usr, cusr) ) break;
		ctr++;
	} while( ctr < contacts);
	memset(temp, '\0', sizeof(temp));	
	user_to_remove = ctr;
	ctr = 0;
	do {
		memset(cusr, '\0', sizeof(cusr));
		if ( ctr != user_to_remove ) get_element(convoa[convo], strlen(convoa[convo]), cusr, ctr);
		strcat(temp, cusr);
		if ( ctr == contacts -1 ){
			if ( temp[strlen(temp)-1] == ' ') temp[strlen(temp)-1] = '\0';
		        for(ctr2=0;ctr2 < MAX_TCP_CON;ctr2++){
		                if (! strcmp(panusr[ctr2],convoa[convo])){
					memset(panusr[ctr2], '\0', sizeof(panusr[ctr2]));
					strcpy(panusr[ctr2], temp);
					break;
				}
		        }
                        memset(convoa[convo], '\0', sizeof(convoa[convo]));
                        strcpy(convoa[convo], temp);

			return;
		}
		if ( strlen(temp) != 0 ) strcat(temp, " ");
		ctr++;
	} while ( ctr < contacts );
	return;
		
}
/* Is there an already open panel for this user? */
int panopen(char *usr)
{
	int ctr;
	for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                if (! strcmp(panusr[ctr],usr)) return ctr+1;
        }
	return 0;

}
/* clear convoa aswell */
void close_convo(char *usr)
{
	int ctr;
        for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                if (! strcmp(panusr[ctr],usr)) {
			switchwin(0);
			memset(panusr[ctr], '\0', sizeof(panusr[ctr]));
			wclear(wc[ctr]);
			update_status();
			return;
		}
/* need to match to convoa here instead                tcp_sockdcon(ctr+1); */
        }
}

void close_connection(int socket)
{
	memset(convoa[socket], '\0', sizeof(convoa[socket]));
	memset(convot[socket], '\0', sizeof(convot[socket]));
}
