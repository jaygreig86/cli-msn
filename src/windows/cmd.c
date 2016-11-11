/* MSN Input0 read source file
 * filename: /src/_cmd.c
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
#include "../headers/defs.h"
#include "../headers/cmd.h"

/* Command Line History Function */
void cmdhistory()
{
/* Stops the history buffer from overflowing */

        if ( history[49][0] != 0 ){

                strcpy(history[44], history[0]);
                strcpy(history[45], history[1]);
                strcpy(history[46], history[2]);
                strcpy(history[47], history[3]);
                strcpy(history[48], history[4]);
                strcpy(history[49], history[5]);
                cmdhist = 6;
        }
        if ( strncmp(history[cmdhist - 1], msgbody, strlen(msgbody) - 1) != 0 ){
                strcpy(history[cmdhist], msgbody);
                history[cmdhist][strlen(history[cmdhist]) -1] = '\0';
                cmdhist++;
        }
        cmdreq = 1;
        memset(history[cmdhist], '\0', sizeof(history[cmdhist]));

}

/* Input Read from stdin (fd 0) Function */

void input()
{
        int ch, ctr, tmp;
        /* If newline (from last command entry) is in msgbody, clear msgbody */

        if (msgbody[strlen(msgbody)-1] == '\n') {
                memset(msgbody, '\0', sizeof(msgbody));
        }

        ch = wgetch(_cmd);
//	cli_msg("\\%d\\\n", ch);
        switch(ch){
//		case 27+49:	
//			cli_msg("ERROR!!!!!!\n");
//			exit(0);
//			break;
//                case 27:
//                        cli_msg("msgbody is %s\\\n", msgbody);
//                        break;
                case KEY_RESIZE:
                        break;

                /* Up arrow (scroll up command history) */
                case KEY_UP:
                                if ( cmdreq == 0 ) cmdreq++;
                                if ( history[cmdhist-cmdreq][0] == 0) cmdreq--;

                                werase(_cmd);
                                waddstr(_cmd, history[cmdhist-cmdreq]);

                                memset(msgbody, '\0', sizeof(msgbody));

                                strcpy(msgbody, history[cmdhist-cmdreq]);
                                mvcur(0, 0, 0, 0);

                                cmdreq++;
                        break;
		/* Left arrow (scroll the contact list) */
//                case KEY_LEFT:
//			if ( offset > 0 ){
//				offset -=1;
//				contacts_out(NULL, NULL);
//			}
//                        break;
                case KEY_NPAGE:
                        scrollup();
                        break;
                case KEY_PPAGE:
                        scrolldown();
                        break;

                case 9:
                        /* nice function to complete a contact address using tab,
                         *enter the first few chars and tab does the rest
                         */

                        werase(_cmd);
                        wprintw(_cmd, msgbody);

                        memset(temp, '\0', sizeof(temp));

			get_element(msgbody, strlen(msgbody), temp, 1);
	
                        ctr = 0;

                        do {
                                if ( strstr(contactn[ctr], temp) ){
                                        tmp = 0;
                                        do {
                                                if ( contactn[ctr][tmp] != temp[tmp] ) return;
                                                tmp++;
                                        } while ( tmp != (int) strlen(temp) && temp[tmp] != '\0' );

                                        msgbody[strlen(msgbody) - strlen(temp)] = '\0';

                                        strcat(msgbody, contactn[ctr]);
//                                        strcat(msgbody, " ");
                                        werase(_cmd);
                                        wprintw(_cmd, msgbody);
                                        break;
                                }
                                ctr++;
                        } while ( ctr < 150 && ctr > 0);

                        break;
		/* Right arrow (scroll down contact list) */
//                case KEY_RIGHT:
//			if ( offset <= MAX_LIST_SIZE ){ 
//				offset +=1;
//				contacts_out(NULL, NULL);
//			}
 //                       break;

		/* Down arrow (scrolling down command history) */
                case KEY_DOWN:
                                if ( cmdreq == 0 ){
                                        werase(_cmd);
                                        memset(msgbody, '\0', sizeof(msgbody));

                                        cmdreq = 0;
                                }

                                else {
                                        cmdreq -= 2;

                                        werase(_cmd);

                                        waddstr(_cmd, history[cmdhist-cmdreq]);

                                        memset(msgbody, '\0', sizeof(msgbody));
                                        strcpy(msgbody, history[cmdhist-cmdreq]);

                                        cmdreq++;
                                }

                        break;

                /* Signal to kill (CTRL + C) */
                case 3:

	                msgbody[strlen(msgbody)] = ch;

                        if ( ch == 3 ) ctrl_c();

                        /* Three strikes... you're quit */
                        if ( msgbody[strlen(msgbody) - 3] == 3 &&  msgbody[strlen(msgbody) - 2] == 3 &&  msgbody[strlen(msgbody) - 1] == 3){
                                elapsed_time();
                                cli_msg("Exiting\n");
                                destroy_window();
				exit(0);
                        }

                        break;

                case 10:

	                msgbody[strlen(msgbody)] = ch;
                        werase(_cmd);

                        ctr = 0;

                        do {
                                ch = msgbody[ctr];
                                if ( msgbody[0] !='/' ){
					input_case(msgbody);
					return;
				}
				if ( (msgbody[ctr] >= 'A') && (msgbody[ctr] <= 'Z')) msgbody[ctr] += lc;
                                ctr++;

                        } while ( ch != ' ' && ch != '\n');
			input_case(msgbody);
			return;

		/* Backspace */
		case 8:
			wprintw(_cmd, " \b");
                        if ( strlen(msgbody) == 1 ){
                                memset(msgbody, '\0', strlen(msgbody));
                        } else {
                                msgbody[strlen(msgbody)-1] = '\0';
                        }

                        break;

		case KEY_BACKSPACE:
			wprintw(_cmd, " \b");
                        if ( strlen(msgbody) == 1 ){
                                memset(msgbody, '\0', strlen(msgbody));
                        } else {
                                msgbody[strlen(msgbody)-1] = '\0';
                        }

			break;
		case KEY_F(1):
			switchwin(0);
			break;
                case KEY_F(2):
                        switchwin(1);
                        break;
                case KEY_F(3):
                        switchwin(2);
                        break;
                case KEY_F(4):
                        switchwin(3);
                        break;
                case KEY_F(5):
                        switchwin(4);
                        break;
                case KEY_F(6):
                        switchwin(5);
                        break;
                case KEY_F(7):
                        switchwin(6);
                        break;
                case KEY_F(8):
                        switchwin(7);
                        break;
		case 127:
			wprintw(_cmd, "\b \b\b \b\b \b");
                        if ( strlen(msgbody) == 1 ){
                                memset(msgbody, '\0', strlen(msgbody));
                        } else {
                                msgbody[strlen(msgbody)-1] = '\0';
                        }
			break;

		case KEY_DC:
			wprintw(_cmd, "\b \b");
			if ( strlen(msgbody) == 1 ){
				memset(msgbody, '\0', strlen(msgbody));
			} else {
				msgbody[strlen(msgbody)-1] = '\0';
			}

			break;
		
                default:
			//DO A CASE HERE WHERE MUST BE / \ 1-9 a-z A-Z etc
	                msgbody[strlen(msgbody)] = ch;

                        return;

        }

        return;

}
