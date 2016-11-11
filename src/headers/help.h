#define FUNC_COUNT 39
/* CLI-MSN Command/Function Help header
 * filename: /src/headers/help.h
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/10/10
 */
#include <ncurses.h>
char help_funcs[FUNC_COUNT][8] = {
{"ADD"},
{"AWAY"},
{"AAWAY"},
{"BACK"},
{"BLOCK"},
{"BLP"},
{"BRB"},
{"BUSY"},
{"BYE"},
{"CLIST"},
{"CLOSE"},
{"CONNECT"},
{"DEBUG"},
{"DUMP"},
{"HIDE"},
{"HIDDEN"},
{"IDLE"},
{"INVITE"},
{"LIST"},
{"LISTA"},
{"LISTN"},
{"LOG"},
{"MSG"},
{"NICK"},
{"ONLINE"}, 
{"OUT"},
{"PHN"},
{"PLIST"},
{"QUERY"},
{"QUIT"},
{"RAW"},
{"REFRESH"},
{"REM"},
{"S"},
{"SHOW"},
{"TIME"},
{"UBLOCK"},
{"WIN"},
{"WINDOW"}
};

char help_usage[FUNC_COUNT][128] = {
"/add [user@host] - Add the user@host to your contact list",
"/away - Sets online status to Away",
"/aaway [0 - 60] - Enables/disables autoaway in minutes. 0 minutes disables.",
"/back - Sets online status to Online",
"/block [user@host] - Blocks the specified user@host",
"/blp - Default setting for users who add you to their list, blocked or allowed. \"/blp a\" or \"/blp b\"",
"/brb - Sets online status to Be Right Back",
"/busy - Sets online status to Busy",
"/bye - Log off and close connections",
"/clist - Lists open connections, not to be confused with open conversations",
"/close [user@host] - Closes an open conversation with the specified user@host",
"/connect [user@host] - Connect to MSN as user@host, if already connected, reconnect",
"/debug [1 or 2] - enable/disable debugging",
"/dump - Dump the contents of the message queue to the screen",
"/hide - Hide contact list",
"/hidden - Sets online status to Appear Offline",
"/idle - Sets online status to Appear Idle",
"/invite [user@host] - Invite the user@host into the current conversation",
"/list - Display users currently in your Online list",
"/lista - Display entire contact list with GUID (Global Unique Identifier)",
"/listn - Display users currently in your online list along with their full friendly name",
"/log [1 or 2] - Enable or disable logging of conversations to ~/msnlogs/",
"/msg [user@host] [message] - Send a message to user@host and open a conversation if not already open",
"/nick [new friendly name] - Changes your friendly name",
"/online - Sets online status to Online",
"/out - Sets online status to Out To Lunch",
"/phn - Sets online status to On The Phone",
"/plist - List the open conversations",
"/query [user@host] - Open a conversation with the specified user@host",
"/quit - Quit CLI-MSN",
"/raw [command] - Send a raw command with a max length of 64 bytes",
"/refresh - Refresh the terminal",
"/rem [user@host] - Remove the user@host from your contact list",
"/s [0-9] - Switch to conversation, 0 = main console",
"/show - Show contact list",
"/time - Display the current time statistics",
"/ublock [user@host] - Un-Blocks the specified user@host",
"/win [0-9] - Switch to conversation, 0 = main console",
"/window [0-9] - Switch to conversation, 0 = main console"
};

void help(char *msgbody);
extern WINDOW *console;

