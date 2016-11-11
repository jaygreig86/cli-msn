/* MSN Dispatch Server handling
 * filename: /src/dispatch.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/06/03
 */

/* Return codes
 * 0 on failure
 * pointer on success
 */

#include <ncurses.h>
#include <string.h>
#include <stdio.h>

extern int tid, cid;
extern void sendData(int, char *, int);
extern void readData(int, char *, int);
extern int tcp_socket();
extern void s_msg(char *, char *, int);
extern void tcp_sockdcon();
extern void cli_msg();
char addrbuff[128];
extern int tcp_sock[0];
extern int i;
char *dispatch(char *username)
{
	char sendbuff[256];
	char recvbuff[256];
	int ctr;
	char ch;
	cid = 0;
	if ( tcp_socket("messenger.hotmail.com", 1863) == -1) {
		cli_msg("Dispatch connection failure\nConsult your coder...\n");
		return 0;
	}
	tid = 0;
	while(tid<4){
		memset(sendbuff, '\0', sizeof(sendbuff));

	        switch (cid){
	                case 0:
	                        snprintf(sendbuff, sizeof(sendbuff), "VER %d MSNP12 MSNP11 CVR0\r\n", tid);
	                        sendData(0, sendbuff, strlen(sendbuff));

	                        break;

	                case 1:

	                        snprintf(sendbuff, sizeof(sendbuff), "CVR %d 0x0409 winnt 5.1 i386 MSG80BETA 8.0.0566 msmsgs %s\r\n", tid, username);
	                        sendData(0, sendbuff, strlen(sendbuff));

        	                break;

        	        case 2:


                	        snprintf(sendbuff, sizeof(sendbuff), "USR %d TWN I %s\r\n", tid, username);
                	        sendData(0, sendbuff, strlen(sendbuff));

        	                break;
		
                	case 3:
				i = 0;
                	        tcp_sockdcon(0);
                	        ctr = 9;
                	        memset(addrbuff, '\0', sizeof(addrbuff));
                        	do {
                        		ch = recvbuff[ctr];
                        		if ( ch != ':' ) addrbuff[ctr-9] = ch;
                        		ctr++;
                        	} while ( ch != ':' &&  ch != '\0');
				tid++;
				break;
		}

		if ( tid != cid + 1 ){
			cli_msg("Connection sequence with dispatch server failed\nConsult your coder...\n");
			return 0;
		}
		
		if ( tcp_sock[0]){
			memset(recvbuff, '\0', sizeof(recvbuff));
        	        readData(0, recvbuff, sizeof(recvbuff));
		}
                if ( recvbuff[0] == 'V' && recvbuff[1] == 'E' && recvbuff[2] == 'R' ) cid = 1;
                else if ( recvbuff[0] == 'C' && recvbuff[1] == 'V' && recvbuff[2] == 'R' ) cid = 2;
                else if ( recvbuff[0] == 'X' && recvbuff[1] == 'F' && recvbuff[2] == 'R' && tid == 3) cid = 3;
		else if ( recvbuff[0] == '9' && recvbuff[1] == '1' && recvbuff[2] == '1' ) {
			s_msg("Error: Invalid User@host\n", 0, 0);
			return 0;
		}
        }

	return addrbuff;
}

