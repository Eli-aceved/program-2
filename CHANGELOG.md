## Version 0.1.18 - Missing prototype
- Added prototype to cclient

## Version 0.1.17 - Finished Program#2 & all commands are successful
- Handled printing the handles that don't exist on client sender terminal
- Broadcast and list of handles now work (%B, %L)
- Code takes care of removing key-sock pair when a client disconnects
- Broadcast doesn't send to sender
- Cleaned up code for submission

## Version 0.1.16 - %M & %C command testing
- Created table to be globally accesible
- Deleted no longer needed lines of code in pdu_io.c
- At server, created functions flagCheck(), processMCMsgs() to handle flags and send individual packets to each client for %M & %C
- At cclient, created flagCheck(), sendFlag1(), and printMCMsgs() which checks for flags in packets received from the server, sends initial flag when client attempts to connect to the server (sends flag to server), and prints the sende's handle and message received from the server onto the client terminal 


## Version 0.1.15 - Successfully send %M%C packets to server
- Finished sendMsgCmd(), although needs some cleaning up
- Created new functions in server to handle flags and next steps done in the server 

## Version 0.1.14 - Saving before major line shifting
- Saving progress before major changes
- Fixing buffer allocation in sendMsgCmd()


## Version 0.1.13 - sendMsgCmd() now repurposed for unicast and multicast
- mutlicast code implemented in the same function as unicast
- Fixed some errors
- Tested current code. Correctly checks if input after comman-type is an integer or a handle and prints statement
- Renamed variables and file names for packetFactory

## Version 0.1.12 - Send message function complete
- Renamed commands.c and .h to packetFactory
- Created function to check for command type and call the command function to create a packet
- Program currently sends packet (server doesn't receive yet)
- Output is looking promising during debugging
- Prints packet length
- Cleaned up and modified code in cclient (processStdin())

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