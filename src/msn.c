#include <locale.h>
#include "headers/defs.h"
#include "headers/global.h"
#if HAVE_GETOPT_H
#include <getopt.h>
#endif
#include "libs/md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>
#include "headers/msn.h"

extern void initialize_time();
extern void clear_contact();
char debugfile[MAX_ACCOUNT_LEN];
FILE *clearfile;

int main(int argc, char *argv[])
{
	int ctr;
	int high_sock = 0;			/* Used for select, which socket has the highest value? */
	char *home;
	char *notifserver;		/* Notification server */
	fd_set inputs;			/* Inputs holds the fd sets */
	char optchar;
	struct timeval timeout;		/* Set the timeout structure for select*/
	record_time();			/* Record sign in time */
	setlocale(LC_ALL, "en_US.US-ASCII");
	setenv("TERM", "xterm-color", 1);
	home = getenv("HOME");
	signal(SIGWINCH, resizeHandler);
	initialize_time();		/*  Set the time for the date/time stamp */

	pid = getpid();

	/* Section for retrieving flags, username and password */
	
	memset(certificate, '\0', sizeof(certificate));
	args[0][0] = 0;
        while ( ( optchar = getopt(argc, argv, "hvdfu:c:") ) != -1 ){
                switch(optchar){
			/* Debug mode */
                        case 'd':
				args[0][0] += 1;
				break;
			/* Debug log to file */
			case 'f':
				args[0][0] += 10;
				snprintf(debugfile, sizeof(debugfile), "%d.out", pid);
				clearfile = fopen(debugfile, "w");
				fclose(clearfile);
				break;
			/* Save conversations to log files */
			case 'l':
				args[2][0] += 1;
				cli_msg("Logging: Enabled\n");
				break;
			/* Username */
			case 'u':
				if (strlen(optarg) > MAX_ACCOUNT_LEN) {
					cli_msg("Invalid Username (Invalid Length)\n");
					destroy_window();
				}else strcpy(username,optarg);
				args[1][0] = 1;
				break;
			/* Spefify the config file */
			case 'c':
				strcpy(certificate,optarg);
				
				break;
			/* version */
			case 'v':
				printf("Version: CLI-MSN/%s (Unix)\n", VERSION);
				printf("Coded By: James Greig\nEmail: James@mor-pah.net\n");
				exit(0);
			case 'h':
				printf("\t-d\t\tDebug to stdout\n");
				printf("\t-f filename\tDebug to filename\n");
				printf("\t-l\t\tConversation logging enable\n");
				printf("\t-u username\tLogin with specified username\n");
				printf("\t-c filename\tUse specified config file\n");
				printf("\t-v\t\tPrint version\n");
				printf("\t-h\t\tList these options\n");
				exit(0);
			default:
				exit(0);
		}

	}
	build_window();			/* Construct the interface */

	/* Check conf file (default: msn.conf) */
	conf();	

	if (! strlen(certificate)) snprintf(certificate, sizeof(certificate), "%s/curl-ca-bundle.crt", home);
	cli_msg("Reading certificate from %s\n", certificate);
 	if ( username[0] == 0 ) {
		cli_msg("No username specified, Not Connected\n");
		cli_msg("Use '/connect username' to connect now\n");
	} else {
	        /* Get Password (no echo) */
	
        	if ( password[0] == 0 ) {
			noecho();

			wprintw(_cmd, "Enter Your Password: ");
			wrefresh(_cmd);
	
			wgetstr(_cmd, password);

			wclear(_cmd);
			wrefresh(_cmd);
        	}

		cli_msg("Logging in as %s...\n", username);
		if (! (notifserver = dispatch(username)) ){
			cli_msg("Not Connected\n");
			tcp_sockdcon(0);
		}
		else if ( notification(username, password, notifserver)){		/* Runs the Notification server sequence */
			cli_msg("Not Connected\n");
			tcp_sockdcon(0);
		}
		else cli_msg("Login Successful, %s online\n", username);
	}

	/* Clear the contact lists up */
	clear_contact();


	echo();		/* fault with some OS', after entering password, client no longer echos input */

	memset(msgqn, '\0', sizeof(msgqn));
	memset(msgqm, '\0', sizeof(msgqm)); 

	while(1){
	        /* (select) Delay 1 second */
                timeout.tv_sec = 1;
        	timeout.tv_usec = 0;

		wrefresh(_cmd);

        	FD_ZERO(&inputs);

		for (i=0;i < MAX_TCP_CON;i++){
			if ( tcp_sock[i] != 0 ){
				FD_SET(tcp_sock[i], &inputs);
				if ( tcp_sock[i] > high_sock ){
					high_sock = tcp_sock[i];
				}
			}
		}
		FD_SET(0, &inputs);

		result = select(high_sock+1, &inputs, (fd_set *) 0, (fd_set *) 0, &timeout);
		switch(result){
				case 0:
					if ( tcp_sock[0] ) ping_time();
					break;
				case -1:
//					perror("select");
					break;
				default:

					if ( tcp_sock[0] ) ping_time();

		/* Switchboard connections */
		for(i=1;i<MAX_TCP_CON;i++){
			if(tcp_sock[i] != 0){
				if(FD_ISSET(tcp_sock[i], &inputs)){
					switch_b(i);	
				}
			}
		}
	
		/* NOTIFICATION PERMANENT CONNECTION */

		if(FD_ISSET(tcp_sock[0], &inputs)){
			if ( tcp_sock[0] != 0){
				if ( notifperm_func() ){
				close(tcp_sock[0]);
				tcp_sock[0] = 0; 
				}
			}
			memset(recvbuff, '\0', sizeof(recvbuff));
		}
		/* Read input from stdin */
		if(FD_ISSET(0, &inputs)){
			input();
		}
	}
}
}

