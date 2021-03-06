=========================================================================
Greetz
=========================================================================

Greetz to... wh1sky, phased, high_towe, big dave,
	mister pand0r, phool312, remedy and anyone
		i've missed!

P.S Thanx to phased for the linux porting help (MD5 Library etc.).

=========================================================================
Contents
=========================================================================

1. Version Information
2. What is CLI-MSN?
3. Fixes
4. TO DO
5. Feature Status
6. Usage: Connecting & Logging in
7. Usage: Conversations
8. Contact

=========================================================================
1. VERSION
=========================================================================

CLI-MSN
Version "2.3.2"

MSN Protocol
Version "MSNP11"

=========================================================================
2. What is CLI-MSN?
=========================================================================

CLI-MSN is a UNIX Command Line implementation of the MSN Messenger Client,
coded purely in C with the ability to work in almost any UNIX/Linux variation.
The desire for the creation of CLI-MSN came when little or no command-line
based messengers were found to be available that implemented MSN Messenger.

The client itself accepts commands similar to popular IRC clients (/cmd).

=========================================================================
3. Fixes
=========================================================================

See ChangeLog.

=========================================================================
4. TO DO:
=========================================================================

---
Complete file-sending feature, and incorporate file receiving
---

=========================================================================
5. Feature Status
=========================================================================
```
Fully Working Commands:

/AWAY   -       Sets status to 'Away'
/BACK   -       Sets status to 'Online'
/BRB	-	Sets status to 'Be Right Back'
/HIDDEN -       Sets status to 'Appear Offline'
/BUSY   -       Sets status to 'Busy'
/PHN    -       Sets status to 'On The Phone'
/OUT    -       Sets status to 'Out To Lunch'

/SHOW	-	Show the contact list window
/HIDE	-	Hide the contact list window

/RAW	-	Send a raw msn command
/BYE	-	Disconnect from msn, but keep cli-msn open

/CONNECT USER@HOST	-	Connect to msn as user@host

/BLOCK USER@HOST        -       Blocks user@host
/UBLOCK USER@HOST       -       UnBlocks user@host
/CLIST                  -       List conversations and matching sockets
/LIST                   -       Show current users online
                                and their related status
				Also states whether user is blocked.
/LISTN			- 	Displays list of users along with their
				full friendly names.
/LISTA			-	Displays a complete list of your contact list
				including address, guid and friendly name.
/QUERY USER@HOST        -       Start a conversation with user@host
/MSG USER@HOST MESSAGE  -       Send message to user
                                See README
/ADD USER@HOST		-	Add a user to your contact list
/QLIST			-	List currently open conversations
/CLOSE USER@HOST	-	Leave conversation with user.
/NICK NAME		-	Change your nick-name to the specified one

Partially Working Commands:

/SEND USER@HOST FILE.EXT	-	Sends a file to the specified user using MSNFTP.
					(Currently not fully implemented, upon accept
					program fails).
/INVITE USER@HOST #		-	Invite a user into an already open conversation
					specified by the conversation number.
					The conversation number can be found in /QLIST.
```
=========================================================================
6. Usage: Connecting & Logging in
=========================================================================

Due to the early stages of the client, the interface has not yet been
worked on, but will once the partially working commands are completed.
The interface will be created using the ncurses library.

To start off with, you must first login with a passport account and password.
The passport can either be a user@hotmail.com, a user@msn.com or a personally
created account at passport.net.

To run CLI-MSN, (assuming this was not a make install) simply do the following command:

	% ./msn -u big_dave@hotmail.com

CLI-MSN will then request you for the passports related password. Enter it here.
(The input will not echo back to stdout)

If the login was successful, CLI-MSN will print :

	Login Successful, climsn@mor-pah.net online

If you receive a problem here please contact me at james@mor-pah.net.

If you receive the following error:

	[@ CLIMSN] error setting certificate verify locations:

You will need to specify the location of the curl certificate file via the following command:

	% ./msn -c /etc/curl-ca-bundle.crt	

To enter a command simply enter them in the form /command.

Logging on can also be made by the program reading information from ./msn.conf.
An example msn.conf has been included.

=========================================================================
7. Usage: Conversations
=========================================================================

With msn, a few rules must be followed to ensure that conversations take place.

There are 3 server types:

Dispatch	-	Simply tells the client the current notificaiton server
			IP address and PORT.

Notification	-	The server that holds your online presence (always connected
			to this server).

Switchboard	-	The server type that we use for conversations.

The client is always connected to the Notificaiton server.

To message a user you cannot simply send a message to them through the notification server.
First, the client must connect to 'a' switchboard server and then the ability to send messages
to a 'user' can begin.

the /QUERY command allows you to first create a connection with a user.

Example scenario:

You are logged in and you do /LIST and realise that 'Sam' is online.
Sam has the address, sam@hotmail.com.  Sam has not began talking to you, 
therefore you must be the one to create the connection to Sam.

To do this, you simply carry out the following command:

/QUERY sam@hotmail.com

If successful, CLI-MSN will echo:

Convesation open with sam@hotmail.com

And the conversation will now be listed in /QLIST.

Now that a connection has been created, it is now possible to talk to sam
using the command line as the message area.

[%] Hello Sam!

Example scenario 2:

You are logged in and suddenly you receive:

Conversation open with sam@hotmail.com

This would mean that sam has opened a connection with you 
(opened a window to you using the official client).

Because sam was first to open the connection, you will not need to open one,
however, you may respond using /MSG, or initialize a query using /QUERY so that
messages typed in the command area will be sent to SAM

/MSG sam@hotmail.com hello sam:)

or /QUERY sam@hotmail.com
[%] Hello Sam!

If you unfortunately receive:

Conversation closed with sam@hotmail.com

then typing in the command area and sending them a new message will open
the conversation again or sending /msg will open it again.



