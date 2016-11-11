/* TCP Socket header
 * filename: /src/tcp_sock.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 16/06/03
 * Returns soc on sucess
 * Returns -1 on failure
 *
 */
/* TCP Socket construction */
#include "headers/tcp_sock.h"
#include "headers/defs.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define LISTENQ 1
extern void destroy_window();
FILE *debugout;
int i = 0;
int prncnt;
extern void *cli_msg();
extern char debugfile[MAX_ACCOUNT_LEN];
extern int tid;
int tcp_socket(char *host, int port)
{
	int soc = 0;
	if(isdigit(host[0])) {
		if ((address = inet_addr(host)) == -1) {
	                cli_msg("Error: Unable to connect - Invalid Host \"%s:%d\"\n", host, (char *) port, NULL, NULL);
			return -1;
			}

	        sock.sin_addr.s_addr = address;
	        
		}
        else {
                if((ph = gethostbyname(host)) == NULL) {
	                cli_msg("Error: Unable to connect - Invalid Host \"%s:%d\"\n", host, (char *) port, NULL, NULL);
		return -1;
                }

		pptr = (struct in_addr **) ph->h_addr_list;
		memcpy(&sock.sin_addr, *pptr, sizeof(struct in_addr));
        }

	sock.sin_family = AF_INET;

	/* Place port in sock information structure */

	sock.sin_port = htons(port);

	/* open socket */
	for(soc=0;soc<MAX_TCP_CON;soc++){
		if(! tcp_sock[soc]) {
			if((tcp_sock[soc] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	        		cli_msg("Unable to connect: Error at socket build\n");
				/* exit(1); */
				exit(0);
	        	}
			/* Connet to remote host */
	
			if(connect(tcp_sock[soc], (struct sockaddr *) &sock, sizeof (sock)) < 0) {
	        		cli_msg("Unable to connect: Socket Connect Error\n");
				return -1;
		        }
			
			return soc;
		}
	}
	return soc;
}
/* function to format lines for debugging mode */
void formatData(char *buffer, int buffsize, int transferdir, int debugtype)
{
	char print[buffsize * 2];
	int ctr = 0;
	strcpy(print, buffer);
	do {
		if ( print[ctr] == '\r' ) print[ctr] = ' ';
		ctr++;
	}  while ( print[ctr] != '\0' );
	if ( debugtype & 1 ){
		if (!transferdir){
			wprintw(console, "<<"); 
		}
		if (transferdir){
			wprintw(console, ">>"); 
		}
		wprintw(console, " %s", print);
		win_update();
	}
	if ( debugtype & 10 ){
		debugout = fopen(debugfile, "a");
		if (!transferdir){
			fputs("<<", debugout);
			fputs(buffer, debugout);
		}
		if (transferdir){
			fputs(">>", debugout);
			fputs(buffer, debugout);
		}
		fclose(debugout);
	}
	return;
}

int tcp_bind(char *host, int port)
{
	memset(&servaddr, '\0', sizeof(servaddr));
        if(isdigit(host[0])) {
                if ((address = inet_addr(host)) == -1) {
                        cli_msg("Error: Unable to connect - Invalid Host\n", NULL, NULL, NULL, NULL);
                        return 1;
                        }

                servaddr.sin_addr.s_addr = address;

                }
        else {
                if((ph = gethostbyname(host)) == NULL) {
                cli_msg("Error: Unable to connect - Invalid Host\n", NULL, NULL, NULL, NULL);
                return 1;
                }

                pptr = (struct in_addr **) ph->h_addr_list;
                memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
        }

        servaddr.sin_family = AF_INET;

        /* Place port in sock information structure */

        servaddr.sin_port = htons(port);

        if(tcp_bindsock[i] == 0) {
                if((tcp_bindsock[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        cli_msg("Unable to bind: Error at socket build\n", NULL, NULL, NULL, NULL);
			return 2;
                }

		if( bind(tcp_bindsock[i], (struct sockaddr *) &servaddr, sizeof(servaddr))){
			cli_msg("Unable to bind: Error at bind build\n", NULL, NULL, NULL, NULL);
			return 2;
		}
                if( listen(tcp_bindsock[i], LISTENQ) ) {
                        cli_msg("Unable to listen: Socket listen Error\n", NULL, NULL, NULL, NULL);
                        return 2;
                }
        }
	
	return 0;
}
/* TCP Socket Disconnect */

void tcp_sockdcon(int sock_num)
{

	close((int) tcp_sock[sock_num]);
/* (int) commented out by jaacoppi */
/*	(int) */ tcp_sock[sock_num] = 0;

}

/* Read Data Function (Data IN)*/

int readData(int sock_num, char *readbuff, int readsize)
{
        int bytes_read;
        int ctr;
        memset(readbuff, '\0', readsize);       /* Clear read buffer (null) */
        bytes_read = read((int) tcp_sock[sock_num], readbuff, (int) readsize);
//        cli_msg("bytes read: %d\n", ERRORNO);
	if ( bytes_read == 0 || bytes_read == -1 ){
		if (! sock_num ){
			cli_msg("MSN has disconnected you, this may be due to a command that has been issued\n");
			clear_contact();
		} else {
			cli_msg("Socket '%d' error: %s\n", sock_num, strerror( errno ));
                        for(ctr=0;ctr < MAX_TCP_CON;ctr++){
                                if ( convo_socket[ctr][0] == sock_num ){
                                        memset(convo_type[ctr], '\0', sizeof(convo_type[ctr]));
                                        memset(convo_users[ctr], '\0', sizeof(convo_users[ctr]));
                                        memset(convo_socket[ctr], '\0', sizeof(convo_socket[ctr]));
                                }
                        }
                        tcp_sockdcon(sock_num);
		}
		return 0;
	} 
	if ( args[0][0] ){
		formatData( readbuff, (int) readsize, 1, args[0][0]);
	}
//	strcpy(readbuff, readbuff2);
	return bytes_read;
}

/* Send Data Function (Data OUT) */

void sendData(int sock_num, char *sendbuff, int sendsize)
{
	if ( tcp_sock[0] == 0 ) {
		cli_msg("Not Connected\n");
		return;
	}	
	if ( tcp_sock[sock_num] == 0 ){
		close_connection(sock_num-1);
		return;
	}
	tid += 1;
	write((int) tcp_sock[sock_num], (char *) sendbuff, (int) sendsize);
        if ( args[0][0]  ){
                formatData((char *) sendbuff, (int) sendsize, 0, args[0][0]);
        }
}
