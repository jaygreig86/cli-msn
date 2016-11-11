/* TCP Socket header
 * filename: /src/headers/tcp_sock.h
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 16/06/03
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <unistd.h>
#include <ncurses.h>
#include "defs.h"
        struct sockaddr_in sock;        /* Structure for socket address         */
	struct sockaddr_in servaddr;
        long address;                   /* Remote IP (4 octet) address          */
        struct hostent *ph;
        int tcp_sock[MAX_TCP_CON];
	struct in_addr **pptr;
	int tcp_bindsock[10];
	extern WINDOW *console;
	int args[NO_OF_ARGS][1];			/* 4 - Auto Away */
							/* 5 - Auto Reconnect */
extern char convo_type[MAX_TCP_CON][16];                /* Conversation type in open panel (public/private/file) */
extern char convo_users[MAX_TCP_CON][255];              /* Users currently in conversation in open panel */
extern char convo_socket[MAX_TCP_CON][1];               /* Socket number to match with panel, i.e. socket 3 = panel 2 */

