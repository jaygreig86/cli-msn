/* MSN Notification Server handling
 * filename: /src/notification.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/06/03
 */

/* Return codes
 * 0 on sucess
 * 1 on failure
 */

#include "headers/ticket.h"
#include "headers/notification.h"
#include <stdio.h>
#include <string.h>

int notification(char *username, char *password, char *server)
{
	char chls[512];		/* Notification challenge string */
	char sendbuff[1024];
	char recvbuff[512];
	char ch;
	int chctr, chtmp, ctr, tmp;		/* Challenge counter, temp challenge counter, counter, temporary counter */
	cid = 0;

	memset(sendbuff, '\0', sizeof(sendbuff));

	s_msg("Connecting to Notification server %s port %d\n", server, 1863);

	if ( tcp_socket(server, 1863) == -1){
		cli_msg("Notification connection failure\nConsult your coder...\n");
		return 1;
	}
	tid = 0;
	while(tid<5){
                switch(cid){
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
                                /* Password Encryption and passport nexus login */

				chctr = 12;
				chtmp = 0;
				memset(chls, '\0', sizeof(chls));
				do {
					chls[strlen(chls)] = recvbuff[chctr];
					chctr++;
				} while( recvbuff[chctr] != '\0' && recvbuff[chctr] != '\n' && recvbuff[chctr] != '\r');
				memset(ticket, '\0', sizeof(ticket));
				if ( nexus((char *) chls, (char *) username, (char *) password) ){
					return 1;
				}
				chls[strlen(chls)] = '\0';
                                snprintf(sendbuff, sizeof(sendbuff), "USR %d TWN S %s\r\n", tid, ticket);
                                sendData(0, sendbuff, strlen(sendbuff));
                                break;

                        case 4:
				snprintf(sendbuff, sizeof(sendbuff), "SYN %d 0 0\r\n", tid);
                                sendData(0, sendbuff, strlen(sendbuff));
				return 0;
                }
                if ( tid != cid + 1 ){
                        cli_msg("Connection sequence with notification server failed\nConsult your coder...\n");
                        return 1;
                }
                if(tcp_sock[0] != 0){
                        memset(recvbuff, '\0', sizeof(recvbuff));
                        readData(0, recvbuff, sizeof(recvbuff));
                }

		recvbuff[strlen(recvbuff)-2] = '\n';
               	if ( recvbuff[0] == 'V' && recvbuff[1] == 'E' && recvbuff[2] == 'R' ) cid = 1;
                else if ( recvbuff[0] == 'C' && recvbuff[1] == 'V' && recvbuff[2] == 'R' ) cid = 2;
                else if ( recvbuff[0] == 'U' && recvbuff[8] == 'N' && recvbuff[10] == 'S' ) cid = 3;
		else if ( recvbuff[6] == 'O' && recvbuff[7] == 'K' && tid == 4) cid = 4;
                else if ( recvbuff[0] == '9' && recvbuff[1] == '1' && recvbuff[2] == '1' ){
			s_msg("Error: Invalid password for %s\n", username, 0);
			return 1;
                }
		else if ( recvbuff[0] == '6' && recvbuff[1] == '0' && recvbuff[2] == '1' ){
			s_msg("Error 601: Server is unavailable\n");
			return 1;
		}
        }

	return 1;
}
