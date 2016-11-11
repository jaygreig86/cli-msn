/* CLI-MSN Interface Construction header File (NCURSES)
 * filename: /src/headers/window.h
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 20/06/03
 */
#include <ncurses.h>
int row, col;
void box_title();
int count;
WINDOW *console, *_cmd, *contactsw, *contactsb, *_status, *_cmdph, *_contacts;
int contact_list_show = 1;
