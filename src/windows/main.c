/* CLI-MSN Interface Construction File (NCURSES)
 * filename: /src/window.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 20/06/03
 */
#include "../headers/window.h"
#include "../headers/defs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <panel.h>
#include <curses.h>
#define OCCUPY_CONTACTS 0.30
#define OCCUPY_SCREEN 0.70
int SIGWINCH_SIG = 0;
extern int current;
extern void cut_string();
char panusr[MAX_TCP_CON][MAX_ACCOUNT_LEN];
char clisto[MAX_LIST_SIZE][MAX_ACCOUNT_LEN]; /*online*/
char clista[MAX_LIST_SIZE][MAX_ACCOUNT_LEN]; /*away*/
void cli_msg();
void contacts_out();
void contacts_show();
int contact_list_size = 0;
extern char msgbody[255];
int offset = 0;
char *datestamp();
char short_date[6];
PANEL *pconsole, *pc[MAX_TCP_CON - 1], *contactlist;
WINDOW *wc[MAX_TCP_CON - 1];
int TOP_PAN = 0;
void update_status();
void switchwin();
int messages[MAX_TCP_CON];
void build_window()
{
	int ctr;
        initscr();
	use_env(TRUE);
        getmaxyx(stdscr,row,col);
        console = newwin(row-3, (col), 0, 0);	/* Main Console */
        _cmd = newwin(1, col-4, row-1, 4);	/* Command Line */
	_status = newwin(2, col, row-3, 0);	/* Status bar */
	_cmdph = newwin(1, 4, row-1, 0);	/* Command place holder (bottom left) */
	_contacts = newwin(row-3, (col * OCCUPY_CONTACTS), 0, (col*OCCUPY_SCREEN));
	contact_list_size = (col * OCCUPY_CONTACTS);
	pconsole = new_panel(console);
        contactlist = new_panel(_contacts);
	for (ctr=0;ctr < 10;ctr++){
		wc[ctr] = newwin(row-3, col, 0, 0);
		scrollok(wc[ctr], 1);
		pc[ctr] = new_panel(wc[ctr]);
	        update_panels();
	        doupdate();
	        refresh();
	}

        scrollok(console, 1);
        scrollok(_cmd,1);
        raw();
	start_color();
	keypad(_cmd, 1);
        use_default_colors();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_RED, -1);	/* Client messages */
	init_pair(4, COLOR_YELLOW, -1);	/* User messages */
	init_pair(5, COLOR_GREEN, -1);
	init_pair(6, COLOR_GREEN, COLOR_BLUE);
	init_pair(7, COLOR_BLUE, COLOR_YELLOW);
	wbkgd(_status, COLOR_PAIR(1));
	wrefresh(_status);
	cli_msg("%s\n", VERSION);
	mvwprintw(_status, 0, 0,  "[lag ??]");
	mvwprintw(_status, 1, 0, "Status: Offline\n");
	wrefresh(_status);
	wprintw(_cmdph, "[%%]");
	wrefresh(_cmdph);
	mvwprintw(_status, 0, 27, "[ Active: ]\n");
	wrefresh(_status);
	mvwprintw(_contacts, 0, 0, "[ Contacts ]\n");
	wrefresh(_contacts);
}

void destroy_window()
{
	endwin();
	exit(0);
}

void print_help(char *str1, char *str2, char *str3)
{
	int row, col;
	getyx(console, row, col); 
	mvwprintw(console, row, 7, str1);
	mvwprintw(console, row, 20, str2);
	mvwprintw(console, row, 30, str3); 
	waddch(console, '\n');
}	
void ctrl_c()
{
                wprintw(_cmd, "\b \b\b");
                wattron(_cmd, A_STANDOUT);
                waddch(_cmd, 'C');
                wrefresh(_cmd);
		wattroff(_cmd, A_STANDOUT);
}

/* Conversation Info */
void c_msg(int window, char *msg, char *marg, char *marg2, char *marg3, char *marg4)
{
        datestamp();
        wattron(wc[window], COLOR_PAIR(3));
        wprintw(wc[window], "[%s][@ CLIMSN] ", short_date);
        wattroff(wc[window], COLOR_PAIR(3));
        wprintw(wc[window], (char *) msg, (char *) marg, (char *) marg2, (char *) marg3, (char *) marg4);
        switchwin(TOP_PAN);

        return;

}

/* Client message */
void cli_msg(char *msg, char *marg, char *marg2, char *marg3, char *marg4)
{
	datestamp();
	wattron(console, COLOR_PAIR(3));
	wprintw(console, "[%s]", short_date);
	wattroff(console, COLOR_PAIR(3));
	wprintw(console, (char *) msg, (char *) marg, (char *) marg2, (char *) marg3, (char *) marg4);
	switchwin(TOP_PAN);

	return;
}

/* Server message */

void s_msg(char *msg, char *marg, char *marg2, char *marg3)
{
	datestamp();
        wattron(console, COLOR_PAIR(5));
        wprintw(console, "[%s][MSN] ", short_date);
        wattroff(console, COLOR_PAIR(5));
        wprintw(console, (char *) msg, (char *) marg, (char *) marg2, (char *) marg3);
        switchwin(TOP_PAN);

        return;
}
/* Contact list entry */
void nick_out(char *nick, char *status)
{
	if ( strlen(nick) ){
		if ( strlen(nick) > col-33 ){
			mvwprintw(_status, 1, 0, "Status: %s\t\tNickName: %.10s...\n", status, nick);
                }else {
                                mvwprintw(_status, 1, 0, "Status: %s\t\tNickName: %s\n", status, nick);
                        }

                        wrefresh(_status);
                }

	return;
}

/* contact list entry */
void c_ent(char *usr, int type)
{
	if ( contact_list_show ){
		if ( type == 1 ){
			wattron(_contacts, COLOR_PAIR(5));
			wprintw(_contacts, " %s", usr);
			wattroff(_contacts, COLOR_PAIR(5));
		}
		if ( type == 2 ){
	                wattron(_contacts, COLOR_PAIR(4));
	                wprintw(_contacts, " %s", usr);
	                wattroff(_contacts, COLOR_PAIR(4));
	        }
		wrefresh(_contacts);
	}
	return;
}

/* Received message */
/* Message, user, connection */
void u_msg(char *msg, char *usr, int window)
{
	datestamp();
	logging(1,usr,msg);
	wattron(wc[window], COLOR_PAIR(4));
	/* by jaacoppi: %s ]:\n to %s]: " */
	wprintw(wc[window], "[%s][%s]: ", short_date, (char *) usr);
	wattroff(wc[window], COLOR_PAIR(4));
	wprintw(wc[window], "%s", (char *) msg);
	waddch(wc[window], '\n');

	switchwin(TOP_PAN);

	if (TOP_PAN-1 != window){
		messages[window] = 1;
	}

	update_status();

	return;
}

/* Sent Message */
void r_msg(char *usr, char *msg)
{
	int window = utop(usr);
	datestamp();
	logging(0,usr,msg);
        wattron(wc[window], COLOR_PAIR(4));
	/* by jaacoppi: said ]:\n to said]: " */
        wprintw(wc[window], "[%s][You said]: ", short_date);
        wattroff(wc[window], COLOR_PAIR(4));
	wprintw(wc[window], "%s", (char *) msg);
        waddch(wc[window], '\n');

        switchwin(TOP_PAN);
        update_status();
	
	return;
}

void box_title(char *msg)
{
	wattron(console, A_BOLD);
	wprintw(console, "%s",(char *) msg);

	switchwin(TOP_PAN);

	wattroff(console, A_BOLD);
	return;
}
	
void resizeHandler()
{
	int ctr;
	delwin(_cmd);
	ungetch (KEY_RESIZE);
	endwin();
	initscr();
	refresh();
	getmaxyx(stdscr, row, col);
	resizeterm(row, col);
        wresize(console, row-3, col);
        wresize(_contacts, row-3, col * OCCUPY_CONTACTS);
        for (ctr=0;ctr < 10;ctr++){
                wresize(wc[ctr], row-3, col);
		redrawwin(wc[ctr]);
                update_panels();
                doupdate();
         }
        mvwin(_cmdph, row-1, 0);
        mvwin(_status, row-3, 0);
        move_panel(contactlist, 0, (col*OCCUPY_SCREEN));
        update_panels();
        redrawwin(_contacts);
        redrawwin(_cmdph);
        redrawwin(console);
        redrawwin(_status);
	_cmd = newwin(1, col-4, row-1, 4);
        scrollok(_cmd,1);
	keypad(_cmd, 1);
	wprintw(_cmd, msgbody);
	wnoutrefresh(_cmd);
	wnoutrefresh(_status);
	wnoutrefresh(_cmdph);
/*        if ( contact_list_show ){
                cli_msg("Contact window hidden, use /show to view it again\n", NULL, NULL, NULL, NULL);
                contact_list_show = 0;
        } */

	switchwin(TOP_PAN);
        getch();
}

void contacts_hide()
{
/*	delwin(_contacts);
	contact_list_show = 0;
	wresize(console, row-3, col);
	switchwin(TOP_PAN); */
	contact_list_show = 0;
        hide_panel(contactlist);
        update_panels();
}

void contacts_show()
{
	contact_list_show = 1;
/*	wresize(console, row-3, (col * OCCUPY_SCREEN));
	switchwin(TOP_PAN);
	_contacts = newwin(row-3, (col * OCCUPY_CONTACTS), 0, (col*OCCUPY_SCREEN));
	contact_list_size = (col * OCCUPY_CONTACTS);
	contacts_out(NULL, NULL);
*/
        show_panel(contactlist);
        update_panels();
}

void contacts_out(char *contact, char *status)
{
	int ctr = 0;
	int output_count = 0;
	int listed_count = 0;
	char cutstring[128];                             /* Pointer for cut string */
	if ( contact != NULL && status != NULL ){
	if (! strcmp(status, "NLN") ){
		/* Are they already in the online list? */
		ctr = 0;
		do {
			if (! strcmp(clisto[ctr], contact)){
				return;
			}
			ctr++;
		} while (ctr < MAX_LIST_SIZE );

		/* Move to online list */
		ctr = 0;
		do {
			if (! strlen(clisto[ctr]) ){
				strcpy(clisto[ctr], contact);
				break;
			}
			ctr++;
		} while (ctr < MAX_LIST_SIZE );
		/* Remove from old list if they're on one */
		ctr = 0;
                do {
                        if (! strcmp(clista[ctr], contact)){
                                memset(clista[ctr], '\0', sizeof(clista[ctr]));
                        }
                        ctr++;
                } while (ctr < MAX_LIST_SIZE );
	} else  {

                /* Are they already in the away list? */
		ctr = 0;
                do {
                        if (! strcmp(clista[ctr], contact) ){
				return;
			}
                        ctr++;
                } while (ctr < MAX_LIST_SIZE );
                /* Move to online list */
		ctr = 0;
                do {
                        if (! strlen(clista[ctr]) ){
                                strcpy(clista[ctr], contact);
				break;
                        }
                        ctr++;
                } while (ctr < MAX_LIST_SIZE );
                /* Remove from old list if they're on one */
		ctr = 0;
                do {
                        if (! strcmp(clisto[ctr], contact)){
                                memset(clisto[ctr], '\0', sizeof(clisto[ctr]));
                        }
                        ctr++;
                } while (ctr < MAX_LIST_SIZE );
        }
	}
        else if (! status && contact){
                ctr = 0;
                do {
                        if (! strcmp(clisto[ctr], contact)){
                                memset(clisto[ctr], '\0', sizeof(clisto[ctr]));
                        }
                        ctr++;
                } while (ctr < MAX_LIST_SIZE );
                ctr = 0;
                do {
                        if (! strcmp(clista[ctr], contact)){
                                memset(clista[ctr], '\0', sizeof(clista[ctr]));
                        }
                        ctr++;
                } while (ctr < MAX_LIST_SIZE );
        }

	if ( contact_list_show ){
		werase(_contacts);
		wprintw(_contacts," [ Contacts ]\n");
	}
	ctr = 0;
	output_count = 0;
	listed_count = 0;
	do {
		if ( strlen(clisto[ctr]) ){
			if ( ctr >= offset ){
				memset(cutstring, '\0', sizeof(cutstring));
				cut_string(clisto[ctr], contact_list_size - 1, cutstring);
				c_ent(cutstring, 1);
				output_count++;
			}
			listed_count++;
		}
		ctr++;
	} while ( ctr < MAX_LIST_SIZE && output_count < contact_list_size - 1);

	ctr = (output_count + offset) - listed_count;
	if ( output_count < contact_list_size - 1) {
	        do {
	                if ( strlen(clista[ctr]) ){
				memset(cutstring, '\0', sizeof(cutstring));
				cut_string(clista[ctr], contact_list_size - 1, cutstring);
				c_ent(cutstring, 2);
			}
	                ctr++;
			output_count++;
	        } while ( ctr < MAX_LIST_SIZE && output_count < contact_list_size - 1);
	}
	return;
}	

/* Return date stamp */

char *datestamp()
{
	int ctr = 11;
	char long_date[18];
	memset(short_date, '\0', sizeof(short_date));
	snprintf(long_date, sizeof(long_date), "%s", ctime(&current));

	do {
		short_date[strlen(short_date)] = long_date[ctr];
		ctr++;
	}while(long_date[ctr] != '\0' && ctr < 16);
	return short_date;
}

void win_update()
{
	switchwin(TOP_PAN);
	return;
}

void switchwin(int win)
{
	if ( win == 0 ){
		top_panel(pconsole);
		TOP_PAN = win;
		update_status();
	}else if (strlen(panusr[win-1]) ){
		top_panel(pc[win-1]);
		TOP_PAN = win;
		messages[win-1] = 0;
		update_status();
        }else {
                cli_msg("No conversation open on panel : %d\n", (char *)win,NULL,NULL,NULL);
                return;
        }
        if (contact_list_show) top_panel(contactlist);
        update_panels(); 
//        refresh();
        return;
}
/* update the status bar for active convo's */
void update_status()
{
	int ctr = 0;
	int count = 0;	/* number of convos open */

        mvwprintw(_status, 0, 37, "           \n");
        wrefresh(_status);

	do {
		if (strlen(panusr[ctr]) ){
			if ( messages[ctr] == 1 ){
				wattron(_status, A_BOLD);
				mvwprintw(_status,0,37+count, "%d", ctr+1);
				printf("\a");
				wattroff(_status, A_BOLD);
			} else mvwprintw(_status,0,37 + count , "%d", ctr+1);
			count++;
		}
		ctr++;
	}while( ctr < MAX_TCP_CON );
        mvwprintw(_status, 0, 37+count, "]\n");
        wrefresh(_status);

	return;
}

void scrollup()
{
}
void scrolldown()
{
}
