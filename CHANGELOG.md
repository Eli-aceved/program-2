## Version 0.1.11 - Client now takes 4 arguments
- Client now takes 4 arguments (includes sender's handle)
- Modified checkArgs to check for right amount of arguments
- tcpClientSetup now takes arguments 2 and 3

## Version 0.1.10 - Finished hashtable testing
- Handle table testing was successful
- file testHT.c was created for testing purposes, resides in different directory now
- Created commands.c and .h for functions that will create packets to send to server
- Deleted anything that was named hastable (typo) [WAS NOT HERE BEFORE]

## Version 0.1.9 - Created hashtable
- Created files handle_table.c and .h
- Testing for handle table worked in file:
    - add, remove, get, print, free
- Fixed errors on server

## Version 0.1.8 - Transition to Program#2
- Any push after this will be part of Program#2 and no longer proogram#2's mini-program

## Version 0.1.7 - Added comments
- Added comments to code
- Modified client to handle the server terminating
- Modified the client to handle both user input and server terminating
- Printed PDU on terminal
- Added print statements to server/client terminating

## Version 0.1.6 - Finished mini-program
- Modified client to handle the server terminating
- Modified the client to handle both user input and server terminating
- Printed PDU on terminal
- Added print statements to server/client terminating

## Version 0.1.5 - Modified server to handle a client termination (partially)
- When server detects a connection closed, it prints the connection was closed
- Fixed issue: message recevied did not print on server terminal

## Version 0.1.4 - Server modified to use poll() to handle multiple clients
- Changes in server.c

## Version 0.1.3 - Server modified to loop until ^c
- Modified server.c (lines 45-48)

## Version 0.1.2 - sendPDU() and recvPDU() complete and passed testing
- Ran into issues, now it should be up to date
- recvPDU() done
- Modifications done on cclient and server (1 line of code change on each)

## Version 0.1.1 - sendPDU() and recvPDU() complete and passed testing
- recvPDU() done
- Modifications done on cclient and server (1 line of code change on each)

## Version 0.1.0 - Ready for testing sendPDU()
- Created pdu_io.c and .h which will hold functions sendPDU and recvPDU
- First few versions will be a part of Lab3 mini program
- Modifications on cclinet for sending
- sendPDU() done in pdu_io.c

*Note: pdu_io.c and .h are not complete.