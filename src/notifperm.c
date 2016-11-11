/* MSN Notification Sock-read
 * filename: /src/notifperm.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/06/03
 */
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/defs.h"
#include "headers/tcp_sock.h"
#include "headers/notifperm.h"
extern void c_ent();
extern void get_element();
extern void cut_string();
extern void nick_out();
extern void add_contact();
extern char *get_name();
extern int adjust_contact();
extern void del_contact();
char addrbuff[128];                             /* Buffer for Notification & Swtichboard Host */
char contactn[MAX_LIST_SIZE][MAX_ACCOUNT_LEN];
char contactp[MAX_LIST_SIZE][STATUS_LEN];
char contactf[MAX_LIST_SIZE][MAX_NICK_LEN];
char contactc[MAX_LIST_SIZE][MAX_GUID_LEN];
int blocked[MAX_LIST_SIZE];
int func_format(char *recvbuff, char *format,  int formatsize)
{
        gimp = 0;
        do {
        if ( gimp == 2 || gimp == 0 ){
                for(spuff=0;spuff < COM_LIST;spuff++){
                        if ( (form_ctr == 0 || recvbuff[form_ctr - 1] == '\n' || 
                                ( recvbuff[form_ctr - 1] == '>' && recvbuff[form_ctr - 6] == '/' ) ) && (( recvbuff[form_ctr] == commands[spuff][0] &&
                                recvbuff[form_ctr + 1] == commands[spuff][1] && recvbuff[form_ctr + 2] == commands[spuff][2]) ||
				( recvbuff[form_ctr] == errorlist[spuff][0] && recvbuff[form_ctr + 1] == errorlist[spuff][1]
				 && recvbuff[form_ctr + 2] == errorlist[spuff][2] ))){
                                if ( strlen(format) > 1 ) {
                                        form_ctr--;
                                        return 0;
                                }
                                memset(format, '\0', formatsize);
                                gimp = 1;
                        }
                }
        }
        if ( gimp == 2 || gimp == 1 ) format[strlen(format)] = recvbuff[form_ctr];
        if ( recvbuff[form_ctr] == '\n' ) gimp = 2;
        if ( recvbuff[form_ctr] == '\0' ){
                form_ctr = 0;
                return 1;
        }
                form_ctr++;
        } while ( recvbuff[form_ctr] != '\0' );
        if ( recvbuff[form_ctr - 1] != '\n' ){
                strcat(buff, format);
                return 1;
        }
        else {
                memset(buff, '\0', sizeof(buff));
        }
        form_ctr = 0;

        return 0;
}

void notifperm(char *recvbuff)
{
	char auth[255];                                 /* Buffer to hold authentication string (switchboard) */
	char msgsid[255];                               /* Session ID storage (switchboard request) */
	char addrbuff[16];
	char sendbuff[512];
	char hash[255];                                 /* Stroage Buffer for MD5 hashes */
	char md5hash[255];                              /* Storage Buffer for resulting MD5 Hashes */
	int ctr, tmp, count = 0;
	char ch;
	/* Lag-O-Meter */

        if ( online == 1){
               snprintf(sendbuff, sizeof(sendbuff), "CHG %d NLN 268435456\r\n", tid);
               sendData(0, sendbuff, strlen(sendbuff));

		online += 1;
        }
	if (! strlen(recvbuff) ) return;

	if ( recvbuff[0] == 'Q' && recvbuff[1] == 'N' && recvbuff[2] == 'G'){
		ping_time();
		strcpy(lag[1], lag[0]);
	        snprintf(lag[1], sizeof(lag[1]), "[lag %d] ", _time - ( (png - 1) * 5 ));
	        if ( strcmp(lag[0], lag[1]) != 0 ) {
	                strcpy(lag[0], lag[1]);
	                mvwprintw(_status, 0, 0, "[lag %d] ", _time - ( (png - 1) * 5 ));
       		        wrefresh(_status);
       		}

		return;
	}

/* NOT msn spaces notification */
if ( recvbuff[0] == 'N' && recvbuff[1] == 'O' && recvbuff[2] == 'T' ){
	s_msg("MSN Notification Ignored\n");
	return;
}

/* GTC */
if ( recvbuff[0] == 'G' && recvbuff[1] == 'T' && recvbuff[2] == 'C' ){
        return;
}

/* LSG */
if ( recvbuff[0] == 'L' && recvbuff[1] == 'S' && recvbuff[2] == 'G' ){
	return;
}

/* BLP */
if ( recvbuff[0] == 'B' && recvbuff[1] == 'L' && recvbuff[2] == 'P' ){
	char list[32];
	memset(list, '\0', sizeof(list));
	if ( recvbuff[4] == 'A' ){
                s_msg("BLP Setting: Allow\n");
		return;
	}
        if ( recvbuff[4] == 'B' ){
        	s_msg("BLP Setting: Blocked\n");
		return;
	}
	get_element(recvbuff, strlen(recvbuff), list, 2);
	if ( list[0] == 'A' ){
                s_msg("BLP Setting: Allow\n");
                return;
        }
        if ( list[0] == 'B' ){
                s_msg("BLP Setting: Blocked\n");
                return;
        }

	return;
}

/* Client IP and Client Port */
if ( recvbuff[0] == 'C' && recvbuff[1] == 'l' && recvbuff[2] == 'i' ){
	int clientport = 0;
	char clientip[16];
	int ctr = 10;
	memset(clientip, '\0', sizeof(clientip));
	if ( recvbuff[0] == 'C' && recvbuff[1] == 'l' && recvbuff[6] == 'I' && recvbuff[7] == 'P' ){
		do {
			clientip[strlen(clientip)] = recvbuff[ctr];
			ctr++;
		} while ( recvbuff[ctr] != '\r' && recvbuff[ctr] != '\n' && recvbuff[ctr] != '\0' );
		if ( strlen(clientip) > 1 ) s_msg("ClientIP: %s\n", clientip);
	}

	ctr = 12;

	if ( recvbuff[0] == 'C' && recvbuff[1] == 'l' && recvbuff[6] == 'P' && recvbuff[9] == 't' ){
		do {
			clientport = (clientport * 10) + (recvbuff[ctr] - 48);
                        ctr++;
                } while( recvbuff[ctr] != '\0' && recvbuff[ctr] != '\n' && recvbuff[ctr] != '\r');

                clientport = ((clientport & 255) * 256) + ((clientport & 65280) / 256);

		if ( clientport > 1 ) s_msg("ClientPort: %d\n", (char *)clientport);
	}
	

	return;
}

/* PRP */
if ( recvbuff[0] == 'P' && recvbuff[1] == 'R' && recvbuff[2] == 'P' ){
	int ctr = 4;
	int count = 0;
	do {
		if ( recvbuff[ctr] == ' ' ) break;
		ctr++;
	} while (recvbuff[ctr] != '\0');
	ctr += 1;
	if (( recvbuff[4] == 'M' && recvbuff[5] == 'F' && recvbuff[6] == 'N' ) || ( recvbuff[ctr] == 'M' && recvbuff[ctr + 1] == 'F' && recvbuff[ctr + 2] == 'N' )){
		char ch;
		if ( recvbuff[4] == 'M' && recvbuff[5] == 'F' && recvbuff[6] == 'N' ) ctr = 8;
		else ctr += 3;
		memset(nick, '\0', sizeof(nick));
		/* Parse the friendly name that was received */
		parse_friendly(recvbuff, strlen(recvbuff), ctr, nick);

		nick_out(nick, "Online");
		return;
	}
	return;
}
/* LST */
if ( recvbuff[0] == 'L' && recvbuff[1] == 'S' && recvbuff[2] == 'T' ){
	char address[MAX_ACCOUNT_LEN];
	char nick[MAX_NICK_LEN];
	char guid[MAX_GUID_LEN];
	int op_value = 0;
	int ctr = 6;
	memset(address, '\0', sizeof(address));
	memset(nick, '\0', sizeof(nick));
	memset(guid, '\0', sizeof(guid));
	if ( !online ) online +=1;
	do {
		address[strlen(address)] = recvbuff[ctr];
		ctr++;
	} while (recvbuff[ctr] != '\0' && recvbuff[ctr] != ' ' && recvbuff[ctr] != '\r');
	
	if ( recvbuff[ctr += 1] == 'F' ){

		ctr +=2;
		memset(nick, '\0', sizeof(nick));
                /* Parse the friendly name that was received */
                ctr = parse_friendly(recvbuff, strlen(recvbuff), ctr, nick);
        	ctr +=3;
	
	        do {
	                guid[strlen(guid)] = recvbuff[ctr];
	                ctr++;
        	} while (recvbuff[ctr] != '\0' && recvbuff[ctr] != ' ' && recvbuff[ctr] != '\r');

	}

	ctr +=1;
	do {
		op_value = (op_value * 10) + (recvbuff[ctr] - 48);
		ctr++;
	} while (recvbuff[ctr] != '\0' && recvbuff[ctr] != ' ' && recvbuff[ctr] != '\r');
	if ( op_value & 4 && op_value & 1 ) add_contact(address, nick, guid, 1);	
	if ( op_value & 1) add_contact(address, nick, guid, 0);
	return;
}

/* Server Challenge Responce */	

if ( recvbuff[0] == 'C' && recvbuff[1] == 'H' && recvbuff[2] == 'L' ){
        memset(hash, '\0', sizeof(hash));
        memset(md5hash, '\0', sizeof(md5hash));
	get_element(recvbuff, strlen(recvbuff), hash, 2);
        MSN_handle_chl(hash, md5hash);
        snprintf(sendbuff, sizeof(sendbuff), "QRY %d PROD0114ES4Z%Q5W 32\r\n%s", tid, md5hash);
        sendData(0, sendbuff, strlen(sendbuff));

	return;
}

if ( recvbuff[0] == 'M' && recvbuff[1] == 'S' && recvbuff[2] == 'G' ){
	char content[128];
	char from[128];
	char subject[128];
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

	if (! strcmp(content, "Content-Type: text/x-msmsgsinitialemailnotification; charset=UTF-8") ) {
		ctr += 17;
		memset(content, '\0', sizeof(content));
		do { 
			content[strlen(content)] = recvbuff[ctr];
			ctr++;
		} while( recvbuff[ctr] != '\r' && recvbuff[ctr] != '\0');

		cli_msg("You have %s unread e-mails in your inbox\n", (char *) content);
		return;
	}

	if (! strcmp(content, "Content-Type: text/x-msmsgsemailnotification; charset=UTF-8") ) {

                ctr += 9;

                memset(from, '\0', sizeof(from));
		memset(subject, '\0', sizeof(subject));
                do {
                        if ( tmp == 3 ) from[strlen(from)] = recvbuff[ctr];
			if ( recvbuff[ctr] == '\r' ) tmp++;
			if ( tmp == 7 ){
				tmp++;
				ctr += 2;
			}
			if ( tmp == 8 ) subject[strlen(subject)] = recvbuff[ctr];
                        ctr++;
                } while( tmp < 9 && recvbuff[ctr] != '\0');

		cli_msg("You have received a new e-mail From: %s Subject: %s\n", from, subject);

                return;
        }
        if (! strcmp(content, "Content-Type: text/x-msmsgsprofile; charset=UTF-8") ) {
                cli_msg("Initial MSN Profile received\n");
		return;
	}
}

/* Check if user is offline FLN */

if ( recvbuff[0] == 'F' && recvbuff[1] == 'L' && recvbuff[2] == 'N' ){
	int tmp;
	char who[MAX_ACCOUNT_LEN];
	recvbuff[strlen(recvbuff) - 2] = '\n';
	memset(who, '\0', sizeof(who));
        ctr = 4;
        do {
	        who[strlen(who)] = recvbuff[ctr];
		ctr++;
	} while ( recvbuff[ctr] != '\n' && recvbuff[ctr] != '\r' && recvbuff[ctr] != '\0');
        for(tmp=0;tmp<150;tmp++){
		if (strcmp(contactn[tmp], who) == 0){
			s_msg("%s is now offline\n", contactn[tmp]);
			if ( panopen(contactn[tmp]) ){
				c_msg(utop(contactn[tmp]),"%s is now offline\n", contactn[tmp]);
				
			}
			contacts_out(contactn[tmp], 0);
			adjust_contact(who, "", "FLN");
			return;
		}
	}
	return;
}
/* Contact list cache (received on connect)*/

if ( recvbuff[0] == 'I' && recvbuff[1] == 'L' && recvbuff[2] == 'N' ){
	char who[MAX_ACCOUNT_LEN];
	char status[STATUS_LEN];
	char friendly[MAX_NICK_LEN];
	int ctr = 0;
	int count = 0;

	memset(who, '\0', sizeof(who));
	memset(status, '\0', sizeof(status));
	memset(friendly, '\0', sizeof(friendly));

	do {
		if ( recvbuff[ctr] == ' ' ){
			count += 1;
			ctr += 1;
		}
		if ( count == 2 ) status[strlen(status)] = recvbuff[ctr];
		if ( count == 3 ) who[strlen(who)] = recvbuff[ctr];
		if ( count == 4 ) {
                        if ( recvbuff[ctr] == '%' && recvbuff[ctr + 1] == '2' && recvbuff[ctr + 2] == '0' ){
                                friendly[strlen(friendly)] = ' ';
                                ctr += 3;
                        }
			friendly[strlen(friendly)] = recvbuff[ctr];
		}
		if ( count == 5 ) break;
		ctr++;
	} while( recvbuff[ctr] != '\0' && recvbuff[ctr] != '\r');

	adjust_contact(who, friendly, status);

	contacts_out(who, status);
	return;
}

/* Contact list cache (changes received while connected)*/
if ( recvbuff[0] == 'N' && recvbuff[1] == 'L' && recvbuff[2] == 'N' ){
        char who[MAX_ACCOUNT_LEN];
        char status[STATUS_LEN];
        char friendly[MAX_NICK_LEN];
        int ctr = 0;
        int count = 0;

        memset(who, '\0', sizeof(who));
        memset(status, '\0', sizeof(status));
        memset(friendly, '\0', sizeof(friendly));

        do {
                if ( recvbuff[ctr] == ' ' ){
                        count += 1;
                        ctr += 1;
                }
                if ( count == 1 ) status[strlen(status)] = recvbuff[ctr];
                if ( count == 2 ) who[strlen(who)] = recvbuff[ctr];
                if ( count == 3 ) {
                        if ( recvbuff[ctr] == '%' && recvbuff[ctr + 1] == '2' && recvbuff[ctr + 2] == '0' ){
                                friendly[strlen(friendly)] = ' ';
                                ctr += 3;
                        }
			if ( recvbuff[ctr] == '%' && recvbuff[ctr + 1] == '2' && recvbuff[ctr + 2] == '5' ){
				friendly[strlen(friendly)] = '%';
				ctr +=3;
			}
                        friendly[strlen(friendly)] = recvbuff[ctr];
		}
                if ( count == 4 ) break;
                ctr++;
        } while( recvbuff[ctr] != '\0' && recvbuff[ctr] != '\r');

        if (! adjust_contact(who, friendly, status) ){
		s_msg("%s is now online (%s)\n", who, status);
		if ( panopen(who) ) c_msg(utop(who),"%s is now online\n", who);
	}
        contacts_out(who, status);
	return;
}	

/* Connection from a user has been attempted (Received RNG(RING)) */

if ( recvbuff[0] == 'R' && recvbuff[1] == 'N' && recvbuff[2] == 'G' ){
	int sock_num;
        int socket;
        char who[MAX_ACCOUNT_LEN];

	/* Switchboard IP */
        memset(addrbuff, '\0', sizeof(addrbuff));
        tmp = 0;
        ctr = 4;
        do {
        ch = recvbuff[ctr];
        if ( ch == ' ' ) tmp = ctr + 2;
        ctr++;
        if ( tmp != 0 && ch != ' ' && ch != ':' ) addrbuff[ctr-tmp] = ch;
        } while ( ch != ':' );

        tmp = ctr + 9;

        /* Session ID */
        memset(msgsid, '\0', sizeof(msgsid));
        get_element(recvbuff, strlen(recvbuff), msgsid,1);

        /* Authetication Code */
        memset(auth, '\0', sizeof(auth));
        get_element(recvbuff, strlen(recvbuff), auth,4);

	if ( (sock_num = tcp_socket(addrbuff, 1863) ) == -1) {
		s_msg("No sockets available\n");
		return;
	}
        snprintf(sendbuff, sizeof(sendbuff), "ANS 1 %s %s %s\r\n", username, auth, msgsid);
        sendData(sock_num, sendbuff, strlen(sendbuff));
	return;
}

/* The following is part of the query (the switchboard server request) */

if ( recvbuff[0] == 'X' && recvbuff[1] == 'F' && recvbuff[2] == 'R' ){
	int ctr = 0;
	int ctr2 = 0;
	int sock_num;
	/* Get Switchboard IP */
	memset(addrbuff, '\0', sizeof(addrbuff));
	do {
		if ( recvbuff[ctr] == ' ' ) ctr2 += 1;
		if ( ctr2 == 3 && recvbuff[ctr] != ':' && recvbuff[ctr] != ' ' ) addrbuff[strlen(addrbuff)] = recvbuff[ctr];

		ctr++;

	} while ( recvbuff[ctr] != ':' && recvbuff[ctr] != '\0' );


	/* AUTH string */

	ctr = ctr + 9;

	memset(auth, '\0', sizeof(auth));

	do {
	if ( recvbuff[ctr] != '\n' && recvbuff[ctr] != '\0' ) auth[strlen(auth)] = recvbuff[ctr];

	ctr++;

	} while ( recvbuff[ctr] != '\n' && recvbuff[ctr] != '\0');

	auth[strlen(auth) - 1] = '\0';

        if ( (sock_num = tcp_socket(addrbuff, 1863) ) == -1) {
                s_msg("No sockets available\n");
                return;
        }

        memset(sendbuff, '\0', sizeof(sendbuff));

        snprintf(sendbuff, sizeof(sendbuff), "USR %d %s %s\r\n", tid, username, auth);
        sendData(sock_num, sendbuff, strlen(sendbuff));

	return;
}

	/* ADC */

        if ( recvbuff[0] == 'A' && recvbuff[1] == 'D' && recvbuff[2] == 'C' && recvbuff[4] == '0' ){
                char user[MAX_ACCOUNT_LEN];

                memset(user, '\0', sizeof(user));
		
                get_contactstr(recvbuff, strlen(recvbuff), user, 3);

                s_msg("%s has added you to their contact list\n", user);
                s_msg("To allow the user to see your online presence use /add\n");

                return;
        }

        if ( recvbuff[0] == 'A' && recvbuff[1] == 'D' && recvbuff[2] == 'C'){
                char user[MAX_ACCOUNT_LEN];
                char list[3];
		char guid[MAX_GUID_LEN];
		char friendly[MAX_NICK_LEN]; /* Before formatted */
		char friendlyf[MAX_NICK_LEN]; /* After formatted */

		memset(user, '\0', sizeof(user));
		memset(list, '\0', sizeof(list));
		memset(friendly, '\0', sizeof(friendly));
		memset(guid, '\0', sizeof(guid));
		get_element(recvbuff, strlen(recvbuff), list, 2);
		get_contactstr(recvbuff, strlen(recvbuff), user, 3);
		get_contactstr(recvbuff, strlen(recvbuff), friendly, 4);
		get_contactstr(recvbuff, strlen(recvbuff), guid, 5);

                if (!strcmp("AL", list)){
                        s_msg("User %s added sucessfully to Allow List\n", user);
                }
                if (!strcmp("FL", list)){
			int ctr = 0;

                        s_msg("User %s added sucessfully to Contact List\n", user);

	                do {
	                        if ( friendly[ctr] == '%' && friendly[ctr + 1] == '2' && friendly[ctr + 2] == '0' ){
	                                friendlyf[strlen(friendlyf)] = ' ';
	                                ctr += 3;
	                        }
	                        friendlyf[strlen(friendlyf)] = friendly[ctr];
	                        ctr++;
	                } while (friendly[ctr] != '\0' && friendly[ctr] != ' ' && friendly[ctr] != '\r');

			add_contact(user, friendlyf, guid, 1);
                }
                return;
        }
	
	if ( recvbuff[0] == 'O' && recvbuff[1] == 'U' && recvbuff[2] == 'T' \
	&& recvbuff[4] == 'S' && recvbuff[5] == 'S' && recvbuff[6] == 'D'){
        	wprintw(console, "\nYou were logged out because the MSN server is going down for Maintenance\n");
        	wrefresh(console);
		destroy_window();
	}
	if ( recvbuff[0] == 'R' && recvbuff[1] == 'E' && recvbuff[2] == 'M'){
		char user[MAX_ACCOUNT_LEN];
		char *name;
		char list_name[8];

		memset(user, '\0', sizeof(user));

		get_element(recvbuff, strlen(recvbuff), user, 3);
		get_element(recvbuff, strlen(recvbuff), list_name, 2);

		if ( list_name[0] == 'A' ) s_msg("User %s sucessfully removed from Allow List\n", user);
		if ( list_name[0] == 'F' ){
			name = get_name(user);
			s_msg("User %s sucessfully removed from Forward List\n", name);
			del_contact(name);
			contacts_out(name, 0);
		}
		return;
	}

        /* ERROR HANDLING */
        {
                char errorbuff[4];
		memset(errorbuff, '\0', sizeof(errorbuff));
                snprintf(errorbuff, sizeof(errorbuff), "%.3s", recvbuff);
        for(ctr=0;ctr<ERROR_LIST;ctr++){
                if (! strcmp(errorbuff, errorlist[ctr]) ){
                        s_msg(errordesc[ctr]);
			return;
                }
        }
//		s_msg("Command \"%s\" received from server not interpreted\n", errorbuff);
	}
}

int notifperm_func()
{
	char *complete;		/* The complete packet */
	char format[2048];
	char packet[1024];
        int bytes_read = 0;
        char final_element[2048];
        int no_of_elements;

	memset(packet, '\0', sizeof(packet));
	bytes_read = readData(0, packet, sizeof(packet));

        no_of_elements = count_element(packet, strlen(packet));
        get_element(packet, strlen(packet), final_element, no_of_elements-1);

	complete = (char*) malloc((strlen(packet)+1)*sizeof(char));        
	strcpy(complete, packet);

	do {
		if ( bytes_read >= 1024){
			memset(packet, '\0', sizeof(packet));
			bytes_read = readData(0, packet, sizeof(packet));
			complete = (char*) realloc(complete, sizeof(char) * (strlen(complete) + (strlen(packet)+1)));
			if (!complete) cli_msg("ERROR: Realloc memory allocation error\n");
			strcat(complete, packet);
		} else break;
	} while ( complete[strlen(complete) - 1] != '\n' && complete);
        do {
                memset(format, '\0', sizeof(format));
                func_format(complete, format, sizeof(format));
                notifperm(format);
        } while( form_ctr != 0 );
        free(complete);
	return 0;

}
