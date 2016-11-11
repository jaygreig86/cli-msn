/* MSN Switchboard Sock-read Header
 * filename: /src/switch.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/06/03
 */
#include <ncurses.h>
#include <string.h>
#include "headers/defs.h"
#include "headers/switch.h"
int switch_b(int sock_num)
{
	char recvbuff[4096];
	char sendbuff[512];
	int errorno;
	int ctr, tmp;
	errorno = readData(sock_num, recvbuff, sizeof(recvbuff));
	if ( errorno == -1 || errorno == 0 ){
		/* nothing to see here */
	}

/* Closing a conversation */

if ( recvbuff[0] == 'B' && recvbuff[1] == 'Y' && recvbuff[2] == 'E'){
	char user[MAX_ACCOUNT_LEN];

	memset(user, '\0', sizeof(user));
	
	get_element(recvbuff, strlen(recvbuff), user, 1);

        for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                if ( convo_socket[ctr][0] == sock_num ){
                        if (!strcmp(convo_users[ctr],user)){
                                memset(convo_type[ctr], '\0', sizeof(convo_type[ctr]));
                                memset(convo_users[ctr], '\0', sizeof(convo_users[ctr]));
                                memset(convo_socket[ctr], '\0', sizeof(convo_socket[ctr]));
                                tcp_sockdcon(sock_num);
                                return 0;
                        }
                        c_msg(ctr, "Participant %s has left conversation\n", user);
                        dufromp(ctr, user);
                        return 0;
                }       
        }
	return 0;
}

/* Opening a conversation */

if( recvbuff[0] == 'I' && recvbuff[1] == 'R' && recvbuff[2] == 'O'){
	char user[MAX_ACCOUNT_LEN];
        char type[2];
        int panel;
        int ctr;
	memset(user, '\0', sizeof(user));
	get_element(recvbuff, strlen(recvbuff), user, 4);

        get_element(recvbuff,strlen(recvbuff),type,3);

        if (type[0] == 50){
                /* someone's joining an already open convo*/
                for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                        if ( convo_socket[ctr][0] == sock_num){
                                autop(convo_users[ctr],user);
                                strcat(convo_users[ctr]," ");
                                strcat(convo_users[ctr],user);
                                strcpy(convo_type[ctr],"Public");
                        }
                }
        } else {
                panel = utop(user);
                strcpy(convo_users[panel], user);
                strcpy(convo_type[panel],"Private");
                convo_socket[panel][0] = sock_num;                
                switchwin(panel+1);
        }
        update_status();

	return 0;
}

/* Conversation opening error Occurs when ANS is incorrect */

if ( recvbuff[0] == '9' && recvbuff[1] == '1' && recvbuff[2] == '1' ){
        s_msg("ERROR: Connection requested failed\n");
	tcp_sockdcon(sock_num);
	return 0;
}

/* MSG Message Recieved */
if ( recvbuff[0] == 'M' && recvbuff[1] == 'S' && recvbuff[2] == 'G' ) {
        char content[128];
	char ipaddressx[64];
	char portx[12];
	int ipptr = 0, portptr = 0, fportx = 0;
        ctr = 0;
        tmp = 0;

        memset(content, '\0', sizeof(content));

        do {
                if ( recvbuff[ctr] == '\r' ) tmp++;
                if ( tmp == 2 ){
                        tmp++;
                        ctr += 2;
                }
                if ( tmp == 3 ){
                        content[strlen(content)] = recvbuff[ctr];
                }
                ctr++;
        } while( recvbuff[ctr] != '\0' && tmp < 4);

        if (! strcmp(content, "Content-Type: text/x-msmsgsinvite; charset=UTF-8") ) {
		char msg[512];
	        snprintf(msg, sizeof(msg), "MIME-Version: 1.0\r\nContent-Type: text/x-msmsgsinvite; charset=UTF-8\r\n\r\nInvitation-Command: ACCEPT\r\nInvitation-Cookie: 11\r\nIP-Address: 82.25.141.173\r\nPort: 6891\r\nAuthCookie: 4\r\nLaunch-Application: FALSE\r\n");

        	snprintf(sendbuff, sizeof(sendbuff), "MSG %d A %d\r\n%s", tid, strlen(msg), msg);
		sendData(sock_num, sendbuff, strlen(sendbuff));
                return 0;
        }
	if (! strcmp(content, "Content-Type: text/x-msmsgscontrol") ) {
		return 0;
	}
	if (! strcmp(content, "Content-Type: application/x-msnmsgrp2p") ) {
		return 0;
	}
	if ( strstr(content, "Content-Type: text/plain") ) {
		char user[MAX_ACCOUNT_LEN];
		char says[MAX_MESSAGE_LEN];                                 /* Buffer for user messages received blah says */
		int space_count = 0;
		int ctr = 0;
		memset(says, '\0', sizeof(says));
		memset(user, '\0', sizeof(user));
		get_element(recvbuff, strlen(recvbuff), user, 1);

		do {
			if ( recvbuff[ctr] == '\r' ) space_count++;
			if ( space_count >= 5 ) break;
			ctr++;
		} while ( recvbuff[ctr] != '\0');

		ctr += 2;

		do {
			if ( recvbuff[ctr] != '\r' ) says[strlen(says)] = recvbuff[ctr];
			ctr++;
		} while ( recvbuff[ctr] != '\0');

        	if( says[1] != '\n'){
                        for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                                if ( convo_socket[ctr][0] == sock_num){
                                        u_msg(says, user, ctr);
                                }
                        }
	        }
		return 0;
	}

	return 0;
}	

/* Message sending success */

if ( recvbuff[0] == 'A' && recvbuff[1] == 'C' && recvbuff[2] == 'K'){
	int ctr;
	if ( cookie ) {
		cookie = 0;
		return 0;
	}
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' ){
			r_msg(msgqn[ctr], msgqm[ctr]);
			memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
			memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
			return 0;
		}
	}
	return 0;
}

/* Message sending failure */

if ( recvbuff[0] == 'N' && recvbuff[1] == 'A' && recvbuff[2] == 'K'){
	int ctr;
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' ){
                        s_msg("FAILED SENDING: %s | to %s\n", msgqm[ctr], msgqn[ctr]);
                        wrefresh(console);
                        memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
                        memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
                        return 0;
                }
        }
        return 0;
}

if ( recvbuff[0] == 'U' && recvbuff[1] == 'S' && recvbuff[2] == 'R' ){
	int ctr;
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' && msgqm[ctr][0] == '\0' && ! strlen(msgqm[ctr]) ){
			snprintf(sendbuff, sizeof(sendbuff), "CAL %d %s\r\n", tid, msgqn[ctr]);
			sendData(sock_num, sendbuff, strlen(sendbuff));

                        return 0;
                }
        }
//	s_msg("ERROR: Lost Swithboard Callee'\n", NULL, NULL);

	return 0;
}

if ( recvbuff[0] == 'J' && recvbuff[1] == 'O' && recvbuff[2] == 'I' ){
	char user[MAX_ACCOUNT_LEN];
        int panel;
        int ctr;
	memset(user, '\0', sizeof(user));

	get_element(recvbuff, strlen(recvbuff), user, 1);

	/* Remove this from the queue as it's successful*/
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if ( msgqn[ctr][0] != '\0' && msgqm[ctr][0] == '\0' && ! strlen(msgqm[ctr]) && ! strcmp(user, msgqn[ctr])) {
                        memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
                        memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
			break;
                }
			
        } 

        /* Is this an additional user? */

        for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                if ( convo_socket[ctr][0] == sock_num){
	                	c_msg(utop(convo_users[ctr]),"%s has joined the conversation\n", user);
                                autop(convo_users[ctr],user);
                                strcat(convo_users[ctr]," ");
                                strcat(convo_users[ctr],user);
                                strcpy(convo_type[ctr],"Public");
                        return 0;
                }
        }
                panel = utop(user);
                strcpy(convo_users[panel], user);
                strcpy(convo_type[panel],"Private");
                convo_socket[panel][0] = sock_num;
		update_status();
		switchwin(utop(user)+1);

	/* Send any messages in the queue */
       for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
        	if (msgqn[ctr][0] != '\0' ){
                	msg_send(msgqn[ctr], msgqm[ctr]);
		}
	}
	return 0;
}
if ( recvbuff[0] == '2' && recvbuff[1] == '1' && recvbuff[2] == '7'){
        int ctr;
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' && msgqm[ctr][0] == '\0' && ! strlen(msgqm[ctr]) ){
                        s_msg("Error 217: Connection request failed, the user may not be Online.\n", msgqm[ctr], msgqn[ctr]);
                        wrefresh(console);
                        memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
                        memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
			tcp_sockdcon(sock_num);
                        return 0;
                }
        }
        return 0;

}
if ( recvbuff[0] == '2' && recvbuff[1] == '1' && recvbuff[2] == '6'){
        int ctr;
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' ){
                        s_msg("Error 216: The user %s is Online, however, you may not contact them\n", msgqm[ctr], msgqn[ctr]);
                        wrefresh(console);
                        memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
                        memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
			tcp_sockdcon(sock_num);
                        return 0;
                }
        }
        return 0;
}

if ( recvbuff[0] == '2' && recvbuff[1] == '1' && recvbuff[2] == '5'){
        int ctr;
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' && msgqm[ctr][0] == '\0' && ! strlen(msgqm[ctr]) ){
                        s_msg("Error 215: Unable to open a conversation with %s\n", msgqm[ctr], msgqn[ctr]);
                        wrefresh(console);
                        memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
                        memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
			tcp_sockdcon(sock_num);
                        return 0;
                }
        }
        return 0;
}

if ( recvbuff[0] == '2' && recvbuff[1] == '0' && recvbuff[2] == '8'){
        int ctr;
        for(ctr=0;ctr < MAX_QUEUE_SIZE;ctr++){
                if (msgqn[ctr][0] != '\0' && msgqm[ctr][0] == '\0' && ! strlen(msgqm[ctr]) ){
                        s_msg("Error 208: Unable to open a conversation with %s as account name is invalid\n", msgqm[ctr], msgqn[ctr]);
                        wrefresh(console);
                        memset(msgqn[ctr], '\0', sizeof(msgqn[ctr]));
                        memset(msgqm[ctr], '\0', sizeof(msgqm[ctr]));
			tcp_sockdcon(sock_num);
                        return 0;
                }
        }
        return 0;
}


}
